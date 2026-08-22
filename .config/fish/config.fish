if status is-interactive
    # High-contrast syntax highlighting (ensures incomplete/unknown commands and parameters are always clearly visible)
    set -g fish_color_normal c0caf5
    set -g fish_color_command 50fa7b --bold
    set -g fish_color_keyword bb9af7
    set -g fish_color_quote e5c07b
    set -g fish_color_redirection ff79c6
    set -g fish_color_end 7dcfff
    set -g fish_color_error ff5555 --bold
    set -g fish_color_param 7aa2f7
    set -g fish_color_comment 6272a4
    set -g fish_color_selection --reverse
    set -g fish_color_search_match --reverse
    set -g fish_color_operator 73daca
    set -g fish_color_escape bb9af7
    set -g fish_color_autosuggestion 7a889b
    set -g fish_color_cancel ff5555 --reverse
    set -g fish_color_valid_path --underline

    # Pager / Completion Menu Colors
    set -g fish_pager_color_progress 6272a4
    set -g fish_pager_color_prefix 50fa7b --bold
    set -g fish_pager_color_completion c0caf5
    set -g fish_pager_color_description 7a889b
    set -g fish_pager_color_selected_background --background=3b4261
end

set -gx PATH "$HOME/.cargo/bin" "$HOME/.local/bin" $PATH
