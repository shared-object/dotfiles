# 🌌 Niri Wayland Dotfiles

[English](README.md) | [Русский](README.ru.md)

A comprehensive, responsive, and aesthetic desktop environment configuration based on the **Niri (Wayland)** scrollable tiling compositor for **Void Linux**.

Features dynamic wallpaper theming via **Wallust**, translucent Material Design glass styling, native PipeWire IPC audio integration, dual status bars (**Waybar** and **Yambar**), onscreen keypress overlays (**wshowkeys**), quick application launcher (**vicinae**), keyboard-driven browser (**luakit**), modern code editor (**Neovim**), and the **foot** terminal paired with **fish shell**.

---

## 📸 Key Components

| Component | Software | Description |
|---|---|---|
| **Compositor** | [Niri](https://github.com/YaLTeR/niri) | Modern scrollable-tiling Wayland compositor |
| **Status Bar #1** | [Waybar](https://github.com/Alexays/Waybar) | Vertical left bar with drawer sliders & C IPC daemon |
| **Status Bar #2** | [Yambar](https://codeberg.org/dnkl/yambar) | Minimalist floating top bar powered by C PipeWire/Niri daemons |
| **Terminal** | [Foot](https://codeberg.org/dnkl/foot) | Fast, lightweight Wayland terminal with TrueColor support |
| **Shell** | [Fish](https://fishshell.com/) | User-friendly interactive command-line shell |
| **Notifications** | [Mako](https://github.com/emersion/mako) | Material Translucent notifications & OSD panels |
| **Theming** | [Wallust](https://codeberg.org/explosion-mental/wallust) | Automatic palette generation from wallpaper images |
| **Wallpaper** | [awww](https://github.com/phisch/awww) | Animated Wayland wallpaper daemon |
| **Launcher** | [Vicinae](https://github.com/vicinaehq/vicinae) | Fast Wayland application launcher (`MOD+D`) |
| **Lock Screen** | [gtklock](https://github.com/jovanlanik/gtklock) | Lockscreen with dynamic background & Wallust styling (`MOD+SHIFT+Q`) |
| **Keypress OSD** | [wshowkeys](https://github.com/ammgws/wshowkeys) | Onscreen keypress visualizer (`MOD+SHIFT+V`) |
| **Browser** | [Luakit](https://luakit.github.io/) | Keyboard-driven WebKit browser with Wallust theme & `Ctrl+C` copy |
| **Editor** | [Neovim](https://neovim.io/) | Configured via [kickstart.nvim](https://github.com/shared-object/kickstart.nvim) (Git Submodule) |
| **Audio / Video** | [PipeWire](https://pipewire.org/) + WirePlumber | Low-latency audio server with direct IPC to status bars |

---

## 📦 Dependency Installation

### 1. System Packages (Void Linux XBPS)

The full list of installed packages is provided in [`pkglist.txt`](pkglist.txt) (with versions) and [`xbps-packages.txt`](xbps-packages.txt).

Install all necessary packages with a single command:

```bash
sudo xbps-install -Sy \
    niri xwayland-satellite Waybar yambar mako foot fish-shell \
    awww gtklock luakit Thunar pavucontrol playerctl brightnessctl \
    pipewire pipewire-devel wireplumber libpipewire-0.3 \
    base-devel cmake cargo pkg-config gcc jq wl-clipboard \
    hyprpicker fastfetch htop zoxide wmenu \
    papirus-icon-theme breeze-icons arc-theme nerd-fonts-symbols-ttf
```

### 2. External Tools & Utilities

1. **Wallust** (Color palette generator):
   ```bash
   cargo install --locked wallust
   ```

2. **wshowkeys** (Keypress OSD for `MOD+SHIFT+V`):
   ```bash
   git clone https://github.com/ammgws/wshowkeys.git
   cd wshowkeys
   meson setup build --buildtype=release
   ninja -C build
   sudo ninja -C build install
   ```

3. **Vicinae** (Application launcher):
   Prebuilt binary or source build available at [Vicinae GitHub](https://github.com/vicinaehq/vicinae).

4. **0xProto Nerd Font**:
   Download and unpack into `~/.local/share/fonts/`:
   ```bash
   mkdir -p ~/.local/share/fonts
   curl -LO https://github.com/ryanoasis/nerd-fonts/releases/latest/download/0xProto.tar.xz
   tar -xf 0xProto.tar.xz -C ~/.local/share/fonts/
   fc-cache -fv
   ```

---

## 🚀 Dotfiles Installation

### Automated Installation

Clone the repository with submodules and execute `install.sh`:

```bash
git clone --recurse-submodules <REPO_URL> ~/dotfiles
cd ~/dotfiles
./install.sh
```

The script will automatically:
1. Initialize and fetch git submodules (`git submodule update --init --recursive`)
2. Create directories in `~/.config/`
3. Copy all configuration files
4. Create the wallpaper symlink (`~/Pictures/Wallpapers -> ~/.config/wallpapers`)
5. Grant executable permissions to all helper scripts
6. Compile all custom C daemons for status bars (`layout`, `layout_waybar`, `volume`, `mic`)

### Manual Compilation of C Daemons

A top-level `Makefile` is included for convenience:

```bash
make          # Build all status bar helper daemons
make clean    # Remove compiled binaries
```

---

## 📊 Status Bars: Waybar & Yambar

This configuration includes support for two distinct status bars powered by custom lightweight C daemons:

### 1. Waybar (Default)
* Located on the **left (vertical)**.
* Features drawer sliders for volume (`group/audio`) and brightness (`group/brightness`).
* Utilizes `layout_waybar` (`~/.config/waybar/scripts/layout_waybar`), a C daemon connecting directly to Niri's IPC socket to emit JSON updates with zero polling latency.

### 2. Yambar
* Located at the **top (horizontal, floating translucent bar)**.
* Uses modular C daemons in `~/.config/yambar/scripts/`:
  * `layout` — Niri keyboard layout monitor.
  * `volume` — Direct PipeWire IPC volume and mute monitor.
  * `mic` — Direct PipeWire IPC microphone status monitor.

### 🔄 Switching Between Bars
In `~/.config/niri/config.kdl` under the `Startup Applications` section:
```kdl
// For Waybar:
spawn-at-startup "waybar"

// Or for Yambar (comment waybar and uncomment yambar):
// spawn-at-startup "yambar"
```

---

## 🌐 Luakit Web Browser

[Luakit](https://luakit.github.io/) is optimized for keyboard-centric web browsing:
* **Wallust Theming**: Color palettes for tabs, status bar, completion menu, and link hints are automatically synchronized with the active wallpaper through `colors.lua` and `theme.lua`.
* **Selection Copying**: Pressing `Ctrl + C` in normal mode copies the highlighted text directly to the system clipboard (`luakit.selection.clipboard`).
* **Performance**: Hardware-accelerated 2D canvas, WebGL, and smooth scrolling are enabled out of the box.

---

## 🎨 Dynamic Theming & Wallpapers

When a wallpaper changes, the `theme-sync.sh` script:
1. Analyzes the image color distribution.
2. Uses `wallust` to generate coordinated color schemes for **Foot, Mako, Waybar, Yambar, Luakit, Vicinae, GTK, GTKLock, and Zathura**.
3. Reloads running daemons on the fly (`makoctl reload`, `pkill -SIGUSR2 waybar`, GTK CSS reload).

* **Interactive wallpaper switcher**: Press `MOD+W` (launches `bgselector.sh` with live thumbnail grid).
* **Manual wallpaper switch**:
  ```bash
  awww img ~/Pictures/Wallpapers/Gruvbox/Dark/rockman.png
  ~/.config/scripts/theme-sync.sh
  ```

---

## ⌨️ Default Keybindings (Niri)

Default modifier (`MOD`): `Super` (Windows key).

### 🚀 Application Shortcuts
| Keybinding | Action |
|---|---|
| `MOD + Enter` | Open terminal (`foot`) |
| `MOD + D` | Application launcher (`vicinae toggle`) |
| `MOD + B` | Open web browser (`luakit`) |
| `MOD + Alt + E` | File manager (`thunar`) |
| `MOD + W` | Wallpaper selector (`bgselector.sh`) |
| `MOD + Shift + V` | Toggle onscreen keypresses (`wshowkeys`) |
| `MOD + Shift + Q` | Lock screen (`gtklock`) |
| `MOD + Shift + Esc` | Niri keybindings overlay cheatsheet |

### 🪟 Window Management
| Keybinding | Action |
|---|---|
| `MOD + Q` | Close active window |
| `MOD + H` / `MOD + Left` | Focus column to the left |
| `MOD + L` / `MOD + Right` | Focus column to the right |
| `MOD + J` / `MOD + K` | Switch workspaces (down / up) |
| `MOD + Space` | Switch keyboard layout (`us, ru, ua`) |

### 🔊 Media & Audio Controls
| Key | Action |
|---|---|
| `XF86AudioRaiseVolume` | Increase volume (`wpctl`) |
| `XF86AudioLowerVolume` | Decrease volume (`wpctl`) |
| `XF86AudioMute` | Toggle audio mute |
| `XF86AudioMicMute` | Toggle microphone mute |
| `XF86AudioPlay` / `Next` / `Prev` | Media playback control (`playerctl`) |
| `XF86MonBrightnessUp` / `Down` | Adjust display brightness (`brightnessctl`) |

---

## 📁 Repository Structure

```
dotfiles/
├── .config/
│   ├── fastfetch/         # Fastfetch system info configuration
│   ├── fish/              # Fish shell configuration & functions
│   ├── foot/              # Foot terminal emulator config
│   ├── gtk-3.0/ & 4.0/    # GTK styling & themes
│   ├── gtklock/           # Lockscreen config & styles
│   ├── luakit/            # Luakit browser config (Wallust theme + userconf.lua)
│   ├── mako/              # Mako notification daemon (Material Translucent)
│   ├── niri/              # Niri compositor config & scripts
│   ├── nvim/              # Neovim configuration (git submodule kickstart.nvim)
│   ├── nwg-look/          # GTK appearance settings
│   ├── scripts/           # Wallpaper selector, theme-sync, and media scripts
│   ├── vicinae/           # Vicinae launcher configuration & themes
│   ├── wallpapers/        # Curated wallpaper collections
│   ├── wallust/           # Wallust color templates & configuration
│   ├── waybar/            # Waybar styles, modules, and C layout daemon
│   ├── xsettingsd/        # XSettings daemon config
│   ├── yambar/            # Yambar config & C PipeWire/Niri daemons
│   └── zathura/           # Zathura PDF reader config
├── .gitmodules            # Git submodule definitions
├── Makefile               # Top-level build script for all C daemons
├── install.sh             # One-click installation & deployment script
├── xbps-packages.txt      # Clean Void Linux package list
├── pkglist.txt            # Raw package list with versions (xbps-query -m)
├── README.md              # Documentation (English)
└── README.ru.md           # Documentation (Русский)
```
