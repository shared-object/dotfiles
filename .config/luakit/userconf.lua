------------------------------------------------------------------------------
-- Luakit User Configuration
------------------------------------------------------------------------------

local modes = require "modes"
local settings = require "settings"

-- Helper function to copy selection to Wayland & X11 clipboards
local function copy_selection(w)
    if w and w.view then
        local js = [=[
            (function() {
                var active = document.activeElement;
                if (active && (active.tagName === 'INPUT' || active.tagName === 'TEXTAREA')) {
                    var start = active.selectionStart;
                    var end = active.selectionEnd;
                    if (typeof start === 'number' && typeof end === 'number' && start !== end) {
                        return active.value.substring(start, end);
                    }
                }
                var sel = window.getSelection();
                if (sel && sel.toString()) {
                    return sel.toString();
                }
                return '';
            })()
        ]=]
        w.view:eval_js(js, {
            callback = function (res)
                local text = res
                if (not text or text == "" or text == "null") and luakit.selection.primary and #luakit.selection.primary > 0 then
                    text = luakit.selection.primary
                end

                if text and text ~= "" and text ~= "null" then
                    luakit.selection.clipboard = text
                    luakit.selection.primary = text
                    -- Pipe to wl-copy for native Wayland compositor clipboard managers
                    luakit.spawn(string.format("wl-copy -- %q", text))
                    local preview = string.len(text) > 40 and (string.sub(text, 1, 37) .. "...") or text
                    preview = string.gsub(preview, "[\r\n]+", " ")
                    w:notify("Copied to clipboard: " .. preview)
                else
                    -- Fallback: invoke browser document copy command
                    w.view:eval_js("document.execCommand('copy')", { no_return = true })
                end
            end
        })
    elseif luakit.selection.primary and #luakit.selection.primary > 0 then
        local text = luakit.selection.primary
        luakit.selection.clipboard = text
        luakit.spawn(string.format("wl-copy -- %q", text))
        if w and w.notify then
            local preview = string.len(text) > 40 and (string.sub(text, 1, 37) .. "...") or text
            preview = string.gsub(preview, "[\r\n]+", " ")
            w:notify("Copied to clipboard: " .. preview)
        end
    end
end

-- Bind Ctrl+C across all modes (normal, insert, passthrough)
modes.add_binds("all", {
    { "<Control-c>", "Copy selected text to system clipboard.", copy_selection },
    { "<Control-C>", "Copy selected text to system clipboard.", copy_selection },
})

modes.add_binds("normal", {
    { "<Control-c>", "Copy selected text to system clipboard.", copy_selection },
    { "<Control-C>", "Copy selected text to system clipboard.", copy_selection },
})

modes.add_binds("insert", {
    { "<Control-c>", "Copy selected text to system clipboard.", copy_selection },
    { "<Control-C>", "Copy selected text to system clipboard.", copy_selection },
})

modes.add_binds("passthrough", {
    { "<Control-c>", "Copy selected text to system clipboard.", copy_selection },
    { "<Control-C>", "Copy selected text to system clipboard.", copy_selection },
})

-- Additional settings for smooth modern browsing
settings.webview.enable_smooth_scrolling = true
settings.webview.enable_developer_extras = true
settings.webview.enable_webgl = true
settings.webview.enable_accelerated_2d_canvas = true
settings.webview.zoom_level = 100
settings.window.home_page = "about:blank"
settings.window.search_engines = {
    google     = "https://www.google.com/search?q=%s",
    duckduckgo = "https://duckduckgo.com/?q=%s",
    github     = "https://github.com/search?q=%s",
    void       = "https://voidlinux.org/packages/?q=%s",
    default    = "https://www.google.com/search?q=%s",
}
