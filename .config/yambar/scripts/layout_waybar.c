#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <poll.h>

#define MAX_LAYOUTS 16
#define MAX_NAME_LEN 128

struct layout_state {
    char names[MAX_LAYOUTS][MAX_NAME_LEN];
    unsigned int count;
    int current_idx;
    int last_idx;
};

/* --------------------------------------------------------- */
/* Helper to derive a 2-letter shortcode for language        */
/* --------------------------------------------------------- */

static void get_short_code(const char *full_name, char *out, size_t out_size)
{
    if (!full_name || !*full_name) {
        snprintf(out, out_size, "--");
        return;
    }

    if (strcasestr(full_name, "russian") || strcasecmp(full_name, "ru") == 0) {
        snprintf(out, out_size, "RU");
        return;
    }
    if (strcasestr(full_name, "ukrainian") || strcasecmp(full_name, "ua") == 0 || strcasecmp(full_name, "uk") == 0) {
        snprintf(out, out_size, "UA");
        return;
    }
    if (strcasestr(full_name, "english") || strcasecmp(full_name, "en") == 0 || strcasecmp(full_name, "us") == 0) {
        snprintf(out, out_size, "EN");
        return;
    }
    if (strcasestr(full_name, "german") || strcasecmp(full_name, "de") == 0) {
        snprintf(out, out_size, "DE");
        return;
    }
    if (strcasestr(full_name, "french") || strcasecmp(full_name, "fr") == 0) {
        snprintf(out, out_size, "FR");
        return;
    }
    if (strcasestr(full_name, "spanish") || strcasecmp(full_name, "es") == 0) {
        snprintf(out, out_size, "ES");
        return;
    }

    /* Check for parenthesized code, e.g. "(US)" -> US */
    const char *paren = strchr(full_name, '(');
    if (paren && paren[1] && paren[2] && (paren[3] == ')' || paren[3] == ',')) {
        out[0] = toupper((unsigned char)paren[1]);
        out[1] = toupper((unsigned char)paren[2]);
        out[2] = '\0';
        return;
    }

    /* Fallback: First two letters uppercase */
    size_t j = 0;
    for (size_t i = 0; full_name[i] && j < 2 && j < out_size - 1; i++) {
        if (isalpha((unsigned char)full_name[i])) {
            out[j++] = toupper((unsigned char)full_name[i]);
        }
    }
    out[j] = '\0';
}

/* --------------------------------------------------------- */
/* Waybar JSON output formatting                             */
/* --------------------------------------------------------- */

static void print_waybar(struct layout_state *state)
{
    if (state->count == 0 || state->current_idx < 0 || (unsigned int)state->current_idx >= state->count) {
        printf("{\"text\":\"--\",\"tooltip\":\"No layout\",\"alt\":\"--\",\"class\":\"unknown\"}\n");
        fflush(stdout);
        return;
    }

    if (state->current_idx == state->last_idx)
        return;

    state->last_idx = state->current_idx;
    const char *name = state->names[state->current_idx];

    char short_code[16];
    get_short_code(name, short_code, sizeof(short_code));

    char class_name[16];
    snprintf(class_name, sizeof(class_name), "%s", short_code);
    for (char *p = class_name; *p; p++) {
        *p = tolower((unsigned char)*p);
    }

    printf("{\"text\":\"%s\",\"tooltip\":\"%s\",\"alt\":\"%s\",\"class\":\"%s\"}\n",
           short_code, name, short_code, class_name);
    fflush(stdout);
}

/* --------------------------------------------------------- */
/* Parse JSON messages from Niri IPC                         */
/* --------------------------------------------------------- */

static void parse_json_event(struct layout_state *state, const char *line)
{
    /* 1. Check for KeyboardLayoutsChanged or KeyboardLayouts */
    const char *kbl = strstr(line, "\"keyboard_layouts\"");
    if (!kbl)
        kbl = strstr(line, "\"KeyboardLayouts\"");

    if (kbl) {
        const char *names_arr = strstr(kbl, "\"names\":[");
        if (names_arr) {
            names_arr += 9;
            state->count = 0;

            while (*names_arr && *names_arr != ']' && state->count < MAX_LAYOUTS) {
                const char *q1 = strchr(names_arr, '"');
                if (!q1 || q1 > strchr(names_arr, ']'))
                    break;
                const char *q2 = strchr(q1 + 1, '"');
                if (!q2)
                    break;

                size_t len = q2 - (q1 + 1);
                if (len >= MAX_NAME_LEN)
                    len = MAX_NAME_LEN - 1;

                strncpy(state->names[state->count], q1 + 1, len);
                state->names[state->count][len] = '\0';
                state->count++;

                names_arr = q2 + 1;
            }
        }

        const char *cur_idx_pos = strstr(kbl, "\"current_idx\":");
        if (cur_idx_pos) {
            state->current_idx = atoi(cur_idx_pos + 14);
        }

        print_waybar(state);
        return;
    }

    /* 2. Check for KeyboardLayoutSwitched */
    const char *switched = strstr(line, "\"KeyboardLayoutSwitched\"");
    if (switched) {
        const char *idx_pos = strstr(switched, "\"idx\":");
        if (idx_pos) {
            state->current_idx = atoi(idx_pos + 6);
            print_waybar(state);
        }
        return;
    }
}

/* --------------------------------------------------------- */
/* Find Niri socket path                                     */
/* --------------------------------------------------------- */

static bool get_niri_socket_path(char *out_path, size_t max_len)
{
    const char *env_sock = getenv("NIRI_SOCKET");
    if (env_sock && *env_sock) {
        strncpy(out_path, env_sock, max_len - 1);
        out_path[max_len - 1] = '\0';
        return true;
    }

    const char *runtime_dir = getenv("XDG_RUNTIME_DIR");
    char dir_buf[256];
    if (!runtime_dir || !*runtime_dir) {
        snprintf(dir_buf, sizeof(dir_buf), "/run/user/%u", getuid());
        runtime_dir = dir_buf;
    }

    DIR *d = opendir(runtime_dir);
    if (!d)
        return false;

    struct dirent *entry;
    bool found = false;
    while ((entry = readdir(d)) != NULL) {
        if (strncmp(entry->d_name, "niri.", 5) == 0 && strstr(entry->d_name, ".sock")) {
            char full_path[512];
            int written = snprintf(full_path, sizeof(full_path), "%s/%s", runtime_dir, entry->d_name);
            if (written > 0 && (size_t)written < max_len) {
                memcpy(out_path, full_path, (size_t)written + 1);
                found = true;
                break;
            }
        }
    }
    closedir(d);
    return found;
}

/* --------------------------------------------------------- */
/* Connect to Niri IPC socket                                */
/* --------------------------------------------------------- */

static int connect_niri(void)
{
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;

    if (!get_niri_socket_path(addr.sun_path, sizeof(addr.sun_path)))
        return -1;

    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd < 0)
        return -1;

    if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        close(fd);
        return -1;
    }

    return fd;
}

/* --------------------------------------------------------- */
/* Main                                                      */
/* --------------------------------------------------------- */

int main(void)
{
    struct layout_state state;
    memset(&state, 0, sizeof(state));
    state.current_idx = -1;
    state.last_idx = -2;

    /* Print initial placeholder */
    print_waybar(&state);

    while (1) {
        int fd = connect_niri();
        if (fd < 0) {
            sleep(1);
            continue;
        }

        /* 1. Request current keyboard layouts */
        const char *req_kbl = "\"KeyboardLayouts\"\n";
        (void)write(fd, req_kbl, strlen(req_kbl));

        char buffer[8192];
        size_t buf_len = 0;

        /* 2. Start EventStream */
        const char *req_stream = "\"EventStream\"\n";
        (void)write(fd, req_stream, strlen(req_stream));

        struct pollfd pfd = { .fd = fd, .events = POLLIN };

        while (1) {
            int ret = poll(&pfd, 1, 5000);
            if (ret < 0) {
                if (errno == EINTR)
                    continue;
                break;
            }
            if (ret == 0)
                continue;

            if (pfd.revents & (POLLERR | POLLHUP | POLLNVAL))
                break;

            ssize_t n = read(fd, buffer + buf_len, sizeof(buffer) - buf_len - 1);
            if (n <= 0)
                break;

            buf_len += n;
            buffer[buf_len] = '\0';

            char *line_start = buffer;
            char *newline;
            while ((newline = strchr(line_start, '\n')) != NULL) {
                *newline = '\0';
                if (*line_start) {
                    parse_json_event(&state, line_start);
                }
                line_start = newline + 1;
            }

            size_t remaining = buffer + buf_len - line_start;
            if (remaining > 0 && line_start != buffer) {
                memmove(buffer, line_start, remaining);
            }
            buf_len = remaining;
            buffer[buf_len] = '\0';
        }

        close(fd);
        state.last_idx = -2;
        sleep(1);
    }

    return 0;
}
