------------------------------------------------------------------------------
-- Luakit User Configuration
------------------------------------------------------------------------------

local modes = require "modes"
local settings = require "settings"

-- Copy selected text to clipboard on Control-c (Normal Mode)
modes.add_binds("normal", {
    {
        "<Control-c>",
        "Copy selected text to system clipboard.",
        function (w)
            local sel = luakit.selection.primary
            if sel and #sel > 0 then
                luakit.selection.clipboard = sel
                w:notify("Copied to clipboard: " .. (string.len(sel) > 40 and string.sub(sel, 1, 37) .. "..." or sel))
            else
                w:notify("Nothing selected to copy")
            end
        end
    }
})

-- Additional settings for a smooth and modern browsing experience
settings.webview.enable_smooth_scrolling = true
settings.webview.enable_developer_extras = true
settings.webview.enable_webgl = true
settings.webview.enable_accelerated_2d_canvas = true
settings.webview.zoom_level = 100
settings.window.home_page = "about:blank"
settings.window.search_engines = {
    duckduckgo = "https://duckduckgo.com/?q=%s",
    google     = "https://www.google.com/search?q=%s",
    github     = "https://github.com/search?q=%s",
    void       = "https://voidlinux.org/packages/?q=%s",
    default    = "https://duckduckgo.com/?q=%s",
}
