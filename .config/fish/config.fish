if status is-interactive
    # Dynamic syntax highlighting using the active terminal theme palette
    set -g fish_color_normal normal
    set -g fish_color_command cyan --bold
    set -g fish_color_keyword magenta
    set -g fish_color_quote green
    set -g fish_color_redirection magenta
    set -g fish_color_end blue
    set -g fish_color_error brred --bold --underline
    set -g fish_color_param blue
    set -g fish_color_comment brblack
    set -g fish_color_selection --reverse
    set -g fish_color_search_match --reverse
    set -g fish_color_operator cyan
    set -g fish_color_escape magenta
    set -g fish_color_autosuggestion brblack
    set -g fish_color_cancel brred --reverse
    set -g fish_color_valid_path --underline

    # Pager / Completion Menu Colors
    set -g fish_pager_color_progress cyan
    set -g fish_pager_color_prefix cyan --bold
    set -g fish_pager_color_completion normal
    set -g fish_pager_color_description brblack
    set -g fish_pager_color_selected_background --reverse
end

set -gx PATH "$HOME/.cargo/bin" "$HOME/.local/bin" $PATH
