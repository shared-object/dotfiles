#!/usr/bin/env bash
set -e

DOTFILES_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
CONFIG_DIR="$HOME/.config"

echo "=========================================="
echo "   Niri Wayland Dotfiles Installer"
echo "=========================================="

echo "[1/4] Creating ~/.config directory..."
mkdir -p "$CONFIG_DIR"

echo "[2/4] Copying configuration files..."
for item in "$DOTFILES_DIR/.config"/*; do
    name=$(basename "$item")
    echo "  -> Copying $name to $CONFIG_DIR/$name"
    cp -rf "$item" "$CONFIG_DIR/"
done

echo "[3/4] Setting execute permissions on scripts..."
chmod +x "$CONFIG_DIR/scripts"/*.sh 2>/dev/null || true
chmod +x "$CONFIG_DIR/niri/scripts"/* 2>/dev/null || true

echo "[4/4] Compiling status bar helper binaries..."
make -C "$DOTFILES_DIR"
cp -f "$DOTFILES_DIR/.config/waybar/scripts/layout_waybar" "$CONFIG_DIR/waybar/scripts/" 2>/dev/null || true
cp -f "$DOTFILES_DIR/.config/yambar/scripts/layout" "$CONFIG_DIR/yambar/scripts/" 2>/dev/null || true
cp -f "$DOTFILES_DIR/.config/yambar/scripts/layout_waybar" "$CONFIG_DIR/yambar/scripts/" 2>/dev/null || true
cp -f "$DOTFILES_DIR/.config/yambar/scripts/mic" "$CONFIG_DIR/yambar/scripts/" 2>/dev/null || true
cp -f "$DOTFILES_DIR/.config/yambar/scripts/volume" "$CONFIG_DIR/yambar/scripts/" 2>/dev/null || true

echo ""
echo "==> Installation complete!"
echo "    To install required Void Linux packages:"
echo "      sudo xbps-install -S \$(cat \"$DOTFILES_DIR/xbps-packages.txt\")"
echo ""
echo "    To generate themes from a wallpaper:"
echo "      wallust run ~/Pictures/Wallpapers/Gruvbox/Dark/some_image.png"
echo ""
