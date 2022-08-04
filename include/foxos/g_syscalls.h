#pragma once

#include <stdint.h>
#include <stddef.h>
#include <extern.h>

typedef struct {
	void* base_address;
	size_t buffer_size;
	uint32_t width;
	uint32_t height;
} framebuffer_t;

typedef struct {
	int x;
	int y;
} mouse_position_t;

typedef enum {
	MOUSE_BUTTON_LEFT = 1,
	MOUSE_BUTTON_RIGHT,
	MOUSE_BUTTON_MIDDLE
} mouse_buttons_e;

EXPOSEC framebuffer_t fb_info();

EXPOSEC void copy_to_fb(void* addr);
EXPOSEC void copy_from_fb(void* addr);

EXPOSEC void mouse_reset();
EXPOSEC mouse_position_t mouse_position();
EXPOSEC int mouse_button();