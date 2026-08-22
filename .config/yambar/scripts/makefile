CC := cc

CFLAGS := -O2 -Wall -Wextra \
	$(shell pkg-config --cflags libpipewire-0.3)

PW_LIBS := $(shell pkg-config --libs libpipewire-0.3) -lm

TARGETS := volume mic layout layout_waybar

.PHONY: all clean

all: $(TARGETS)

volume: volume.c
	$(CC) $(CFLAGS) $< -o $@ $(PW_LIBS)

mic: mic.c
	$(CC) $(CFLAGS) $< -o $@ $(PW_LIBS)

layout: layout.c
	$(CC) -O2 -Wall -Wextra $< -o $@

layout_waybar: layout_waybar.c
	$(CC) -O2 -Wall -Wextra $< -o $@

clean:
	rm -f $(TARGETS)
