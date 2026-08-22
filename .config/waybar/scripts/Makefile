CC := cc
CFLAGS := -O2 -Wall -Wextra

TARGETS := layout_waybar

.PHONY: all clean

all: $(TARGETS)

layout_waybar: layout_waybar.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(TARGETS)
