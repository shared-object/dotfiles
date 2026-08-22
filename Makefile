CC := cc
PW_CFLAGS := $(shell pkg-config --cflags libpipewire-0.3 2>/dev/null)
PW_LIBS := $(shell pkg-config --libs libpipewire-0.3 2>/dev/null) -lm

.PHONY: all waybar yambar clean

all: waybar yambar

waybar:
	@echo "==> Building Waybar scripts..."
	$(MAKE) -C .config/waybar/scripts

yambar:
	@echo "==> Building Yambar scripts..."
	$(MAKE) -C .config/yambar/scripts

clean:
	$(MAKE) -C .config/waybar/scripts clean
	$(MAKE) -C .config/yambar/scripts clean
