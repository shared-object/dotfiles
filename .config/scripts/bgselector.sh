#!/usr/bin/env bash

set -uo pipefail

WALL_DIR="$HOME/Pictures/Wallpapers"
CACHE_DIR="$HOME/.cache/thumbnails/bgselector"
CACHE_INDEX="$CACHE_DIR/.index"
STYLE_FILE="$HOME/.config/rofi/bgselector/style.rasi"

# Ensure awww-daemon is running
if ! pgrep -x awww-daemon >/dev/null 2>&1; then
    awww-daemon &
    sleep 0.3
fi

# Ensure directories exist
mkdir -p "$CACHE_DIR"

if [ ! -d "$WALL_DIR" ]; then
    echo "Wallpaper directory not found: $WALL_DIR" >&2
    exit 1
fi

# Build current wallpaper index
current_index=$(mktemp)
find -L "$WALL_DIR" -type f \( -iname '*.jpg' -o -iname '*.jpeg' -o -iname '*.png' -o -iname '*.webp' -o -iname '*.gif' -o -iname '*.bmp' -o -iname '*.avif' \) | sort > "$current_index"

if [ ! -s "$current_index" ]; then
    echo "No wallpapers found in $WALL_DIR" >&2
    rm -f "$current_index"
    exit 1
fi

# Optional thumbnail generation if ImageMagick is installed
if command -v magick >/dev/null 2>&1 || command -v convert >/dev/null 2>&1; then
    CMD_MAGICK="magick"
    command -v magick >/dev/null 2>&1 || CMD_MAGICK="convert"

    to_generate=$(mktemp)
    while read -r img; do
        rel_path="${img#$WALL_DIR/}"
        cache_name="${rel_path//\//_}"
        cache_name="${cache_name%.*}.jpg"
        cache_file="$CACHE_DIR/$cache_name"

        [ -f "$cache_file" ] || echo "$img" >> "$to_generate"
    done < "$current_index"

    if [ -s "$to_generate" ]; then
        max_jobs=$(nproc 2>/dev/null || echo 4)
        job_count=0
        while read -r img; do
            rel_path="${img#$WALL_DIR/}"
            cache_name="${rel_path//\//_}"
            cache_name="${cache_name%.*}.jpg"
            cache_file="$CACHE_DIR/$cache_name"

            if [[ "$img" =~ \.(gif|GIF)$ ]]; then
                $CMD_MAGICK "$img[0]" -thumbnail 330x540^ -gravity center -extent 330x540 -quality 80 +repage "$cache_file" 2>/dev/null &
            else
                $CMD_MAGICK "$img" -thumbnail 330x540^ -gravity center -extent 330x540 -quality 80 +repage "$cache_file" 2>/dev/null &
            fi

            ((job_count++))
            if [ $((job_count % max_jobs)) -eq 0 ]; then
                wait -n 2>/dev/null || wait
            fi
        done < "$to_generate"
        wait
    fi
    rm -f "$to_generate"
fi

# Build menu input list (always populated, with cache icon or original image)
menu_input=$(mktemp)
while read -r img; do
    [ -f "$img" ] || continue
    rel_path="${img#$WALL_DIR/}"
    cache_name="${rel_path//\//_}"
    cache_name="${cache_name%.*}.jpg"
    cache_file="$CACHE_DIR/$cache_name"

    if [ -f "$cache_file" ]; then
        icon="$cache_file"
    else
        icon="$img"
    fi

    printf '%s\0icon\037%s\n' "$rel_path" "$icon"
done < "$current_index" > "$menu_input"

rm -f "$current_index"

# Display selector
selected=""
if command -v rofi >/dev/null 2>&1; then
    if [ -f "$STYLE_FILE" ]; then
        selected=$(rofi -dmenu -i -show-icons -config "$STYLE_FILE" < "$menu_input" || true)
    else
        selected=$(rofi -dmenu -i -p "🌌 Wallpaper:" < "$menu_input" || true)
    fi
elif command -v wmenu >/dev/null 2>&1; then
    selected=$(awk -F'\0' '{print $1}' "$menu_input" | wmenu -i -p "🌌 Wallpaper:" -l 15 || true)
fi

rm -f "$menu_input"

# Apply wallpaper and sync theme
if [ -n "$selected" ]; then
    selected_path="$WALL_DIR/$selected"
    if [ -f "$selected_path" ]; then
        # 1. Apply wallpaper smoothly with awww
        awww img "$selected_path" -t fade --transition-duration 2 --transition-fps 30 &
        
        # 2. Run theme-sync with explicit wallpaper path
        "$HOME/.config/scripts/theme-sync.sh" "$selected_path" &
        wait
    fi
fi
