#pragma once

#include <stdint.h>
#include <extern.h>

typedef struct {
	uint64_t x;
	uint64_t y;
} point_t;

EXPOSEC void set_color(uint32_t color);
EXPOSEC uint32_t get_color();

EXPOSEC void set_cursor(point_t point);
EXPOSEC point_t get_cursor();

EXPOSEC point_t get_screen_size();

EXPOSEC void clear_screen();
EXPOSEC void clear_line();

EXPOSEC void enable_print_char();
EXPOSEC void disable_print_char();