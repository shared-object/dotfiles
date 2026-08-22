#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <math.h>

#include <pipewire/pipewire.h>
#include <pipewire/node.h>
#include <pipewire/extensions/metadata.h>

#include <spa/param/param.h>
#include <spa/param/props.h>
#include <spa/pod/pod.h>
#include <spa/pod/iter.h>
#include <spa/utils/dict.h>

#define MAX_SOURCES 64

struct source_info {
    uint32_t id;
    char name[256];
};

struct app {
    struct pw_main_loop *loop;
    struct pw_context *context;
    struct pw_core *core;
    struct pw_registry *registry;

    struct pw_metadata *metadata;
    struct pw_node *node;

    struct spa_hook core_listener;
    struct spa_hook registry_listener;
    struct spa_hook metadata_listener;
    struct spa_hook node_listener;

    struct source_info sources[MAX_SOURCES];
    unsigned int n_sources;

    char default_source_name[256];
    uint32_t current_node_id;

    float volume;
    bool muted;
    bool have_volume;
    bool have_mute;

    int last_percent;
    bool last_muted;
    bool last_printed;
};

/* --------------------------------------------------------- */
/* Yambar output formatting                                  */
/* --------------------------------------------------------- */

static void print_yambar(struct app *app)
{
    if (!app->have_volume && !app->have_mute) {
        if (!app->last_printed) {
            printf("mic|string|󰍭 --\n\n");
            fflush(stdout);
            app->last_printed = true;
        }
        return;
    }

    float linear_vol = app->volume;
    if (linear_vol < 0.0f)
        linear_vol = 0.0f;

    /* PipeWire uses linear amplitude; convert to cubic scale matching wpctl */
    float cubic_vol = cbrtf(linear_vol);
    int percent = (int)lroundf(cubic_vol * 100.0f);
    if (percent < 0)
        percent = 0;
    if (percent > 150)
        percent = 150;

    if (app->last_printed && app->last_percent == percent && app->last_muted == app->muted)
        return;

    app->last_percent = percent;
    app->last_muted = app->muted;
    app->last_printed = true;

    if (app->muted || percent == 0) {
        printf("mic|string|󰍭 %d%%\n\n", percent);
    } else {
        printf("mic|string|󰍬 %d%%\n\n", percent);
    }

    fflush(stdout);
}

/* --------------------------------------------------------- */
/* Parse SPA_PARAM_Props                                     */
/* --------------------------------------------------------- */

static void parse_props(struct app *app, const struct spa_pod *pod)
{
    if (!pod || !spa_pod_is_object_type(pod, SPA_TYPE_OBJECT_Props))
        return;

    const struct spa_pod_prop *prop;
    const struct spa_pod_object *obj = (const struct spa_pod_object *)pod;

    SPA_POD_OBJECT_FOREACH(obj, prop) {
        switch (prop->key) {
        case SPA_PROP_volume: {
            float val = 0.0f;
            if (spa_pod_get_float(&prop->value, &val) == 0) {
                app->volume = val;
                app->have_volume = true;
            }
            break;
        }

        case SPA_PROP_channelVolumes: {
            uint32_t n_values = 0;
            const float *values = (const float *)spa_pod_get_array(&prop->value, &n_values);
            if (values && n_values > 0) {
                float max_vol = 0.0f;
                for (uint32_t i = 0; i < n_values; i++) {
                    if (values[i] > max_vol)
                        max_vol = values[i];
                }
                app->volume = max_vol;
                app->have_volume = true;
            }
            break;
        }

        case SPA_PROP_mute: {
            bool mute = false;
            if (spa_pod_get_bool(&prop->value, &mute) == 0) {
                app->muted = mute;
                app->have_mute = true;
            }
            break;
        }

        case SPA_PROP_softMute: {
            bool mute = false;
            if (spa_pod_get_bool(&prop->value, &mute) == 0) {
                app->muted = mute;
                app->have_mute = true;
            }
            break;
        }

        default:
            break;
        }
    }

    print_yambar(app);
}

/* --------------------------------------------------------- */
/* Node management                                           */
/* --------------------------------------------------------- */

static void node_param(void *data,
                       int seq,
                       uint32_t id,
                       uint32_t index,
                       uint32_t next,
                       const struct spa_pod *param)
{
    struct app *app = data;
    (void)seq;
    (void)index;
    (void)next;

    if (id == SPA_PARAM_Props)
        parse_props(app, param);
}

static const struct pw_node_events node_events = {
    PW_VERSION_NODE_EVENTS,
    .param = node_param,
};

static void disconnect_node(struct app *app)
{
    if (!app->node)
        return;

    spa_hook_remove(&app->node_listener);
    pw_proxy_destroy((struct pw_proxy *)app->node);
    app->node = NULL;
    app->current_node_id = SPA_ID_INVALID;
    app->have_volume = false;
    app->have_mute = false;
    app->last_printed = false;
}

static void connect_node(struct app *app, uint32_t id)
{
    uint32_t params[] = { SPA_PARAM_Props };

    if (app->node && app->current_node_id == id)
        return;

    disconnect_node(app);

    app->node = pw_registry_bind(app->registry, id, PW_TYPE_INTERFACE_Node, PW_VERSION_NODE, 0);
    if (!app->node)
        return;

    app->current_node_id = id;
    pw_node_add_listener(app->node, &app->node_listener, &node_events, app);

    pw_node_subscribe_params(app->node, params, 1);
    pw_node_enum_params(app->node, 0, SPA_PARAM_Props, 0, UINT32_MAX, NULL);
}

static void update_active_source(struct app *app)
{
    if (app->default_source_name[0]) {
        for (unsigned int i = 0; i < app->n_sources; i++) {
            if (strcmp(app->sources[i].name, app->default_source_name) == 0) {
                connect_node(app, app->sources[i].id);
                return;
            }
        }
    }

    if (app->n_sources > 0 && !app->node) {
        connect_node(app, app->sources[0].id);
    }
}

/* --------------------------------------------------------- */
/* Metadata events                                           */
/* --------------------------------------------------------- */

static int metadata_property(void *data,
                             uint32_t subject,
                             const char *key,
                             const char *type,
                             const char *value)
{
    struct app *app = data;
    (void)subject;
    (void)type;

    if (!key || strcmp(key, "default.audio.source") != 0)
        return 0;

    if (!value || !*value) {
        app->default_source_name[0] = '\0';
        return 0;
    }

    const char *name_pos = strstr(value, "\"name\"");
    if (name_pos) {
        name_pos = strchr(name_pos, ':');
        if (name_pos) {
            name_pos++;
            while (*name_pos == ' ' || *name_pos == '\t' || *name_pos == '"')
                name_pos++;

            size_t i = 0;
            while (*name_pos && *name_pos != '"' && i < sizeof(app->default_source_name) - 1) {
                app->default_source_name[i++] = *name_pos++;
            }
            app->default_source_name[i] = '\0';
        }
    } else {
        strncpy(app->default_source_name, value, sizeof(app->default_source_name) - 1);
        app->default_source_name[sizeof(app->default_source_name) - 1] = '\0';
    }

    update_active_source(app);
    return 0;
}

static const struct pw_metadata_events metadata_events = {
    PW_VERSION_METADATA_EVENTS,
    .property = metadata_property,
};

/* --------------------------------------------------------- */
/* Registry events                                           */
/* --------------------------------------------------------- */

static void registry_global(void *data,
                            uint32_t id,
                            uint32_t permissions,
                            const char *type,
                            uint32_t version,
                            const struct spa_dict *props)
{
    struct app *app = data;
    (void)permissions;
    (void)version;

    if (!props || !type)
        return;

    if (strcmp(type, PW_TYPE_INTERFACE_Metadata) == 0) {
        const char *name = spa_dict_lookup(props, PW_KEY_METADATA_NAME);
        if (name && strcmp(name, "default") == 0 && !app->metadata) {
            app->metadata = pw_registry_bind(app->registry, id, PW_TYPE_INTERFACE_Metadata, PW_VERSION_METADATA, 0);
            if (app->metadata) {
                pw_metadata_add_listener(app->metadata, &app->metadata_listener, &metadata_events, app);
            }
        }
    } else if (strcmp(type, PW_TYPE_INTERFACE_Node) == 0) {
        const char *media_class = spa_dict_lookup(props, PW_KEY_MEDIA_CLASS);
        if (media_class && (strcmp(media_class, "Audio/Source") == 0 || strcmp(media_class, "Audio/Source/Virtual") == 0)) {
            const char *node_name = spa_dict_lookup(props, PW_KEY_NODE_NAME);
            if (node_name && app->n_sources < MAX_SOURCES) {
                app->sources[app->n_sources].id = id;
                strncpy(app->sources[app->n_sources].name, node_name, sizeof(app->sources[app->n_sources].name) - 1);
                app->sources[app->n_sources].name[sizeof(app->sources[app->n_sources].name) - 1] = '\0';
                app->n_sources++;
                update_active_source(app);
            }
        }
    }
}

static void registry_global_remove(void *data, uint32_t id)
{
    struct app *app = data;

    if (app->current_node_id == id)
        disconnect_node(app);

    for (unsigned int i = 0; i < app->n_sources; i++) {
        if (app->sources[i].id == id) {
            if (i != app->n_sources - 1)
                app->sources[i] = app->sources[app->n_sources - 1];
            app->n_sources--;
            break;
        }
    }

    update_active_source(app);
}

static const struct pw_registry_events registry_events = {
    PW_VERSION_REGISTRY_EVENTS,
    .global = registry_global,
    .global_remove = registry_global_remove,
};

/* --------------------------------------------------------- */
/* Main                                                      */
/* --------------------------------------------------------- */

int main(int argc, char **argv)
{
    struct app app = {0};
    app.current_node_id = SPA_ID_INVALID;

    pw_init(&argc, &argv);

    app.loop = pw_main_loop_new(NULL);
    if (!app.loop)
        return 1;

    app.context = pw_context_new(pw_main_loop_get_loop(app.loop), NULL, 0);
    if (!app.context) {
        pw_main_loop_destroy(app.loop);
        return 1;
    }

    app.core = pw_context_connect(app.context, NULL, 0);
    if (!app.core) {
        pw_context_destroy(app.context);
        pw_main_loop_destroy(app.loop);
        return 1;
    }

    app.registry = pw_core_get_registry(app.core, PW_VERSION_REGISTRY, 0);
    if (!app.registry) {
        pw_core_disconnect(app.core);
        pw_context_destroy(app.context);
        pw_main_loop_destroy(app.loop);
        return 1;
    }

    pw_registry_add_listener(app.registry, &app.registry_listener, &registry_events, &app);

    print_yambar(&app);

    pw_main_loop_run(app.loop);

    disconnect_node(&app);

    if (app.metadata) {
        spa_hook_remove(&app.metadata_listener);
        pw_proxy_destroy((struct pw_proxy *)app.metadata);
    }

    spa_hook_remove(&app.registry_listener);
    pw_proxy_destroy((struct pw_proxy *)app.registry);

    pw_core_disconnect(app.core);
    pw_context_destroy(app.context);
    pw_main_loop_destroy(app.loop);
    pw_deinit();

    return 0;
}
