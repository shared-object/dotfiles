# Niri Wayland Dotfiles

[English](README.md) | [Русский](README.ru.md)

Комплексная, производительная и эстетичная конфигурация рабочего окружения на базе тайлингового композитора **Niri (Wayland)** для **Void Linux**.

Включает динамическую смену тем через **Wallust**, полупрозрачный Material Design стиль, интеграцию с PipeWire, два статус-бара на выбор (**Waybar** и **Yambar**), экранные подсказки клавиш (**wshowkeys**), быстрый лаунчер (**vicinae**), клавиатурный браузер (**luakit**), редактор (**Neovim**) и терминал **foot** с оболочкой **fish**.

---

## Основные компоненты

| Компонент | Программа | Описание |
|---|---|---|
| **Compositor** | [Niri](https://github.com/YaLTeR/niri) | Скроллящийся тайлинговый Wayland-композитор |
| **Status Bar #1** | [Waybar](https://github.com/Alexays/Waybar) | Вертикальный бар слева со слайдерами и drawer-модулями |
| **Status Bar #2** | [Yambar](https://codeberg.org/dnkl/yambar) | Минималистичный плавающий бар сверху |
| **Terminal** | [Foot](https://codeberg.org/dnkl/foot) | Быстрый, легковесный Wayland-терминал с поддержкой TrueColor |
| **Shell** | [Fish](https://fishshell.com/) | Дружелюбная интерактивная командная оболочка |
| **Notifications** | [Mako](https://github.com/emersion/mako) | Material Translucent уведомления и OSD-панели |
| **Theming** | [Wallust](https://codeberg.org/explosion-mental/wallust) | Автоматическая генерация цветовых палитр из обоев |
| **Wallpaper** | [awww](https://github.com/phisch/awww) | Анимированный Wayland wallpaper daemon |
| **Launcher** | [Vicinae](https://github.com/vicinaehq/vicinae) | Современный Wayland Application Launcher (`MOD+D`) |
| **Lock Screen** | [gtklock](https://github.com/jovanlanik/gtklock) | Экран блокировки с поддержкой обоев и тем Wallust (`MOD+SHIFT+Q`) |
| **Keypress OSD** | [wshowkeys](https://github.com/ammgws/wshowkeys) | Экранное отображение нажатых клавиш (`MOD+SHIFT+V`) |
| **Browser** | [Luakit](https://luakit.github.io/) | Клавиатурный WebKit-браузер с темой Wallust и `Ctrl+C` копированием |
| **Editor** | [Neovim](https://neovim.io/) | Конфигурация [kickstart.nvim](https://github.com/shared-object/kickstart.nvim) (Git Submodule) |
| **Audio / Video** | [PipeWire](https://pipewire.org/) + WirePlumber | Современный звуковой сервер с прямым IPC в статус-бары |

---

## Установка зависимостей

### 1. Системные пакеты (Void Linux XBPS)

Полный список пакетов находится в файлах [`pkglist.txt`](pkglist.txt) (с версиями) и [`xbps-packages.txt`](xbps-packages.txt).

Установка всех необходимых пакетов одной командой:

```bash
sudo xbps-install -Sy \
    niri xwayland-satellite Waybar yambar mako foot fish-shell \
    awww gtklock luakit rofi ImageMagick Thunar pavucontrol playerctl brightnessctl \
    pipewire pipewire-devel wireplumber libpipewire-0.3 \
    base-devel cmake cargo pkg-config gcc jq wl-clipboard \
    hyprpicker fastfetch htop zoxide wmenu \
    papirus-icon-theme breeze-icons arc-theme nerd-fonts-symbols-ttf
```

### 2. Внешние утилиты (сборка / установка)

1. **Wallust** (генератор цветовых схем):
   ```bash
   cargo install --locked wallust
   ```

2. **wshowkeys** (экранное отображение клавиш для биндинга `MOD+SHIFT+V`):
   ```bash
   git clone https://github.com/ammgws/wshowkeys.git
   cd wshowkeys
   meson setup build --buildtype=release
   ninja -C build
   sudo ninja -C build install
   ```

3. **Vicinae** (лаунчер приложений):
   Доступен в виде бинарника / сборки: [Vicinae GitHub](https://github.com/vicinaehq/vicinae).

4. **Шрифт 0xProto Nerd Font**:
   Скачайте и распакуйте в `~/.local/share/fonts/`:
   ```bash
   mkdir -p ~/.local/share/fonts
   curl -LO https://github.com/ryanoasis/nerd-fonts/releases/latest/download/0xProto.tar.xz
   tar -xf 0xProto.tar.xz -C ~/.local/share/fonts/
   fc-cache -fv
   ```

---

## Установка дотфайлов

### Автоматическая установка

Склонируйте репозиторий вместе с сабмодулями и запустите скрипт `install.sh`:

```bash
git clone --recurse-submodules <URL_РЕПОЗИТОРИЯ> ~/dotfiles
cd ~/dotfiles
./install.sh
```

Скрипт автоматически:
1. Инициализирует и подтянет сабмодули (`git submodule update --init --recursive`)
2. Создаст директории в `~/.config/`
3. Скопирует конфигурационные файлы
4. Создаст симлинк на обои `~/Pictures/Wallpapers`
5. Выдаст права на исполнение всем вспомогательным скриптам
6. Скомпилирует все вспомогательные C-бинарники для статус-баров (`layout`, `layout_waybar`, `volume`, `mic`)

### Ручная сборка C-скриптов

В корне репозитория есть `Makefile`:

```bash
make          # Собрать скрипты для Waybar и Yambar
make clean    # Очистить скомпилированные бинарники
```

---

## Статус-бары: Waybar и Yambar

В конфигурации подготовлена поддержка двух статус-баров со специально написанными легковесными C-демонами:

### 1. Waybar (по умолчанию)
* Расположен **слева (вертикально)**.
* Содержит Drawer-слайдеры громкости (`group/audio`) и яркости (`group/brightness`).
* Использует C-демон `layout_waybar` (`~/.config/waybar/scripts/layout_waybar`), который подключается к IPC-сокету Niri, выводит язык в JSON и мгновенно обновляется без polling-задержек.

### 2. Yambar
* Расположен **сверху (горизонтально, плавающий полупрозрачный бар)**.
* Использует набор C-демонов в `~/.config/yambar/scripts/`:
  * `layout` — индикатор раскладки клавиатуры Niri.
  * `volume` — прямой мониторинг громкости через PipeWire IPC API.
  * `mic` — прямой мониторинг состояния микрофона через PipeWire IPC API.

### Как переключаться между барами:
В файле `~/.config/niri/config.kdl` в блоке `Startup Applications`:
```kdl
// Для Waybar:
spawn-at-startup "waybar"

// Или для Yambar (закомментируйте waybar и раскомментируйте yambar):
// spawn-at-startup "yambar"
```

---

## Браузер Luakit

Конфигурация [Luakit](https://luakit.github.io/) настроена для удобного серфинга с клавиатуры:
* **Интеграция с Wallust**: Цветовая схема вкладок, строки состояния, меню автодополнения и хинтов ссылок генерируется автоматически из обоев через `~/.config/luakit/colors.lua` и `theme.lua`.
* **Копирование выделенного текста**: Комбинация `Ctrl + C` в обычном режиме копирует выделенный текст в системный буфер обмена (`luakit.selection.clipboard`).
* **Плавный скроллинг & WebGL**: Включены аппаратное ускорение 2D canvas, WebGL и плавная прокрутка.

---

## Динамическая смена тем и обои

При смене обоев запускается скрипт `theme-sync.sh`, который:
1. Анализирует изображение обоев.
2. Генерирует с помощью `wallust` цветовые схемы для **Foot, Mako, Waybar, Yambar, Luakit, Vicinae, GTK, GTKLock и Zathura**.
3. Автоматически перезагружает демоны (`makoctl reload`, `pkill -SIGUSR2 waybar`, GTK theme changes).

* **Сменить обои интерактивно**: нажмите `MOD+W` (запустится `bgselector.sh` с превью).
* **Сменить обои вручную**:
  ```bash
  awww img ~/Pictures/Wallpapers/Gruvbox/Dark/rockman.png
  ~/.config/scripts/theme-sync.sh
  ```

---

## Основные горячие клавиши (Niri)

Модификатор по умолчанию (`MOD`) — `Super` (клавиша Windows).

### Запуск приложений
| Комбинация | Действие |
|---|---|
| `MOD + Enter` | Открыть терминал (`foot`) |
| `MOD + D` | Лаунчер приложений (`vicinae toggle`) |
| `MOD + B` | Открыть браузер (`luakit`) |
| `MOD + Alt + E` | Файловый менеджер (`thunar`) |
| `MOD + W` | Выбор обоев (`bgselector.sh`) |
| `MOD + Shift + V` | Вкл/выкл показ клавиш на экране (`wshowkeys`) |
| `MOD + Shift + Q` | Заблокировать экран (`gtklock`) |
| `MOD + Shift + Esc` | Шпаргалка горячих клавиш Niri (Overlay) |

### Управление окнами
| Комбинация | Действие |
|---|---|
| `MOD + Q` | Закрыть активное окно |
| `MOD + H` / `MOD + Left` | Фокус на колонку влево |
| `MOD + L` / `MOD + Right` | Фокус на колонку вправо |
| `MOD + J` / `MOD + K` | Переключение рабочих столов (вниз / вверх) |
| `MOD + Space` | Переключение раскладки клавиатуры (`us, ru, ua`) |

### Мультимедиа
| Клавиша | Действие |
|---|---|
| `XF86AudioRaiseVolume` | Увеличить громкость (`wpctl`) |
| `XF86AudioLowerVolume` | Уменьшить громкость (`wpctl`) |
| `XF86AudioMute` | Заглушить звук (Mute toggle) |
| `XF86AudioMicMute` | Заглушить микрофон |
| `XF86AudioPlay` / `Next` / `Prev` | Управление медиаплеером (`playerctl`) |
| `XF86MonBrightnessUp` / `Down` | Регулировка яркости экрана (`brightnessctl`) |

---

## Структура репозитория

```
dotfiles/
├── .config/
│   ├── fastfetch/         # Конфигурация fastfetch
│   ├── fish/              # Оболочка fish
│   ├── foot/              # Терминал foot
│   ├── gtk-3.0/ & 4.0/    # Настройки тем GTK
│   ├── gtklock/           # Экран блокировки (стили + обои)
│   ├── luakit/            # Браузер luakit (тема Wallust + userconf.lua)
│   ├── mako/              # Демон уведомлений (Material Translucent)
│   ├── niri/              # Конфиг Niri и скрипты (включая toggle-showkeys)
│   ├── nvim/              # Конфигурация Neovim (git submodule kickstart.nvim)
│   ├── nwg-look/          # Настройки внешнего вида
│   ├── scripts/           # Скрипты синхронизации темы, обоев и управления
│   ├── vicinae/           # Настройки лаунчера
│   ├── wallpapers/        # Коллекция обоев
│   ├── wallust/           # Шаблоны и конфиг Wallust
│   ├── waybar/            # Конфиг, стили и C-скрипт раскладки для Waybar
│   ├── xsettingsd/        # Настройки xsettingsd
│   ├── yambar/            # Конфиг и C-скрипты PipeWire/Niri для Yambar
│   └── zathura/           # Конфигурация PDF-ридера
├── .gitmodules            # Конфигурация Git сабмодулей
├── Makefile               # Сборка всех вспомогательных C-бинарников
├── install.sh             # Скрипт быстрой установки
├── xbps-packages.txt      # Чистый список пакетов для Void Linux
├── pkglist.txt            # Исходный вывод xbps-query -m с версиями
├── README.md              # Документация (English)
└── README.ru.md           # Документация (Русский)
```
