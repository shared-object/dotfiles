------------------------------------------------------------------------------
-- Luakit Theme integrated with Wallust Dynamic Colors
------------------------------------------------------------------------------

local colors_ok, colors = pcall(require, "colors")
if not colors_ok then
    colors = {
        bg = "#1d1f21",
        fg = "#c5c8c6",
        black = "#282a2e",
        red = "#cc6666",
        green = "#b5bd68",
        yellow = "#f0c674",
        blue = "#81a2be",
        magenta = "#b294bb",
        cyan = "#8abeb7",
        white = "#ffffff",
        bright_black = "#373b41",
        bright_red = "#cc6666",
        bright_green = "#b5bd68",
        bright_yellow = "#f0c674",
        bright_blue = "#81a2be",
        bright_magenta = "#b294bb",
        bright_cyan = "#8abeb7",
        bright_white = "#ffffff",
    }
end

local theme = {}

-- Typography
theme.font = "11pt 0xProto Nerd Font, JetBrainsMono Nerd Font, monospace"
theme.fg   = colors.fg
theme.bg   = colors.bg

-- General colours
theme.success_fg = colors.green
theme.loaded_fg  = colors.blue
theme.error_fg   = colors.fg
theme.error_bg   = colors.red

-- Warning colours
theme.warning_fg = colors.bg
theme.warning_bg = colors.yellow

-- Notification colours
theme.notif_fg   = colors.fg
theme.notif_bg   = colors.black

-- Menu / Completion colours
theme.menu_fg                   = colors.fg
theme.menu_bg                   = colors.bg
theme.menu_selected_fg          = colors.bg
theme.menu_selected_bg          = colors.blue
theme.menu_title_bg             = colors.black
theme.menu_primary_title_fg     = colors.magenta
theme.menu_secondary_title_fg   = colors.cyan

theme.menu_disabled_fg          = colors.bright_black
theme.menu_disabled_bg          = colors.bg
theme.menu_enabled_fg           = colors.fg
theme.menu_enabled_bg           = colors.bg
theme.menu_active_fg            = colors.green
theme.menu_active_bg            = colors.black

-- Proxy manager
theme.proxy_active_menu_fg      = colors.bg
theme.proxy_active_menu_bg      = colors.green
theme.proxy_inactive_menu_fg    = colors.bright_black
theme.proxy_inactive_menu_bg    = colors.bg

-- Statusbar specific
theme.sbar_fg         = colors.fg
theme.sbar_bg         = colors.black

-- Downloadbar specific
theme.dbar_fg         = colors.fg
theme.dbar_bg         = colors.black
theme.dbar_error_fg   = colors.red

-- Input bar specific
theme.ibar_fg         = colors.fg
theme.ibar_bg         = colors.black

-- Tab label & Bar styling
theme.tab_fg            = colors.bright_black
theme.tab_bg            = colors.black
theme.tab_hover_bg      = colors.bright_black
theme.tab_ntheme        = colors.fg
theme.selected_fg       = colors.bg
theme.selected_bg       = colors.blue
theme.selected_ntheme   = colors.fg
theme.loading_fg        = colors.cyan
theme.loading_bg        = colors.black

theme.selected_private_tab_bg = colors.magenta
theme.private_tab_bg          = colors.bright_black

-- Trusted/untrusted SSL colours
theme.trust_fg          = colors.green
theme.notrust_fg        = colors.red

-- Follow mode hints
theme.hint_font         = "bold 10pt 0xProto Nerd Font, monospace"
theme.hint_fg           = colors.bg
theme.hint_bg           = colors.yellow
theme.hint_border       = "1px solid " .. colors.bright_yellow
theme.hint_opacity      = "0.9"
theme.hint_overlay_bg   = "rgba(100, 150, 240, 0.3)"
theme.hint_overlay_border = "1px solid " .. colors.blue
theme.hint_overlay_selected_bg = "rgba(100, 240, 150, 0.4)"
theme.hint_overlay_selected_border = "1px solid " .. colors.green

-- General colour pairings
theme.ok    = { fg = colors.bg, bg = colors.green }
theme.warn  = { fg = colors.bg, bg = colors.yellow }
theme.error = { fg = colors.white, bg = colors.red }

-- Gopher page style
theme.gopher_light = { bg = colors.white, fg = colors.black, link = colors.blue }
theme.gopher_dark  = { bg = colors.bg, fg = colors.fg, link = colors.cyan }

return theme
