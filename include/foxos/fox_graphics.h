#pragma once

#include <foxos/psf1_font.h>
#include <foxos/g_syscalls.h>

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <extern.h>

typedef struct {
	size_t buffer_size;
	uint32_t width;
	uint32_t height;
	void* buffer;
} graphics_buffer_info_t;

#ifdef __cplusplus
extern "C" {
#endif

static inline graphics_buffer_info_t create_screen_buffer() {
	framebuffer_t fb = fb_info();
	
	graphics_buffer_info_t graphics_buffer_info;
	graphics_buffer_info.width = fb.width;
	graphics_buffer_info.height = fb.height;
	graphics_buffer_info.buffer_size = fb.buffer_size;
	
	graphics_buffer_info.buffer = malloc(graphics_buffer_info.buffer_size);

	return graphics_buffer_info;
}

static inline void fox_free_framebuffer(graphics_buffer_info_t* graphics_buffer_info) {
	free(graphics_buffer_info->buffer);
}

static inline void fox_set_background(graphics_buffer_info_t* info, uint32_t colour) {
	uint64_t base = (uint64_t) info->buffer;
	uint64_t bytes_per_scanline = info->width * 4;
	uint64_t fb_height = info->height;

	for (int vertical_scanline = 0; vertical_scanline < fb_height; vertical_scanline ++) {
		uint64_t pix_ptr_base = base + (bytes_per_scanline * vertical_scanline);
		for (uint32_t* pixPtr = (uint32_t*)pix_ptr_base; pixPtr < (uint32_t*)(pix_ptr_base + bytes_per_scanline); pixPtr ++) {
			*pixPtr = colour;
		}
	}
}

static inline void fox_start_frame(graphics_buffer_info_t* info, bool empty_fb) {
	if (!empty_fb) {
		copy_from_fb(info->buffer);
	} else {
		fox_set_background(info, 0);
	}
}

static inline void fox_end_frame(graphics_buffer_info_t* info) {
	if (info->buffer == NULL) {
		return;
	}

	copy_to_fb(info->buffer);
}

static inline void fox_set_px_unsafe(graphics_buffer_info_t* info, uint32_t x, uint32_t y, uint32_t colour) {
	*(uint32_t*)((uint64_t) info->buffer + (x * 4) + (y * 4 * info->width)) = colour;
}

static inline void fox_set_px(graphics_buffer_info_t* info, uint32_t x, uint32_t y, uint32_t colour) {
	if (x >= info->width || y >= info->height) {
		return;
	}

	*(uint32_t*)((uint64_t) info->buffer + (x * 4) + (y * 4 * info->width)) = colour;
}

static inline void fox_draw_rect_unsafe(graphics_buffer_info_t* info, uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t colour) {
	for (uint32_t j = 0; j < height; j++) {
		for (uint32_t i = 0; i < width; i++) {
			fox_set_px_unsafe(info, i + x, j + y, colour);
		}
	}
}

static inline void fox_draw_rect(graphics_buffer_info_t* info, uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t colour) {
	if (x >= info->width || y >= info->height) {
		return;
	}

	if (x + width > info->width) {
		width = info->width - x;
	}
	if (y + height > info->height) {
		height = info->height - y;
	}

	fox_draw_rect_unsafe(info, x, y, width, height, colour);
}

static inline void fox_draw_rect_outline(graphics_buffer_info_t* info, uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t colour) {
	if (x >= info->width || y >= info->height) {
		return;
	}

	bool width_too_big = false;
	bool height_too_big = false;

	if (x + width > info->width) {
		width = info->width - x;
		width_too_big = true;
	}
	if (y + height > info->height) {
		height = info->height - y;
		height_too_big = true;
	}

	for (uint32_t i = 0; i < width; i++) {
		fox_set_px(info, x + i, y, colour);
		if (!height_too_big) {
			fox_set_px(info, x + i, y + height - 1, colour);
		}
	}

	for (uint32_t i = 0; i < height; i++) {
		fox_set_px_unsafe(info, x, y + i, colour);
		if (!width_too_big) {
			fox_set_px_unsafe(info, x + width - 1, y + i, colour);
		}
	}
}

static inline void fox_draw_circle(graphics_buffer_info_t* info, uint32_t x, uint32_t y, uint32_t r, uint32_t colour) {
	// x and y are the center of the circle
	// r is the radius


	// Bresenham's circle algorithm
	// https://en.wikipedia.org/wiki/Midpoint_circle_algorithm

	int32_t f;
	int32_t ddF_x;
	int32_t ddF_y;
	int32_t x1;
	int32_t y1;

next_iter:
	f = 1 - r;
	ddF_x = 0;
	ddF_y = -2 * r;
	x1 = 0;
	y1 = r;

	fox_set_px(info, x, y + r, colour);
	fox_set_px(info, x, y - r, colour);
	fox_set_px(info, x + r, y, colour);
	fox_set_px(info, x - r, y, colour);

	while (x1 < y1) {
		if (f >= 0) {
			y1--;
			ddF_y += 2;
			f += ddF_y;
		}

		x1++;
		ddF_x += 2;
		f += ddF_x + 1;

		fox_set_px(info, x + x1, y + y1, colour);
		fox_set_px(info, x - x1, y + y1, colour);
		fox_set_px(info, x + x1, y - y1, colour);
		fox_set_px(info, x - x1, y - y1, colour);

		fox_set_px(info, x + y1, y + x1, colour);
		fox_set_px(info, x - y1, y + x1, colour);
		fox_set_px(info, x + y1, y - x1, colour);
		fox_set_px(info, x - y1, y - x1, colour);
	}

	r--;

	if (r > 0) {
		goto next_iter;
	}
}

static inline void fox_draw_circle_outline(graphics_buffer_info_t* info, uint32_t x, uint32_t y, uint32_t r, uint32_t colour) {
	// x and y are the center of the circle
	// r is the radius


	// Bresenham's circle algorithm
	// https://en.wikipedia.org/wiki/Midpoint_circle_algorithm

	int32_t f = 1 - r;
	int32_t ddF_x = 0;
	int32_t ddF_y = -2 * r;
	int32_t x1 = 0;
	int32_t y1 = r;

	fox_set_px(info, x, y + r, colour);
	fox_set_px(info, x, y - r, colour);
	fox_set_px(info, x + r, y, colour);
	fox_set_px(info, x - r, y, colour);

	while (x1 < y1) {
		if (f >= 0) {
			y1--;
			ddF_y += 2;
			f += ddF_y;
		}

		x1++;
		ddF_x += 2;
		f += ddF_x + 1;

		fox_set_px(info, x + x1, y + y1, colour);
		fox_set_px(info, x - x1, y + y1, colour);
		fox_set_px(info, x + x1, y - y1, colour);
		fox_set_px(info, x - x1, y - y1, colour);

		fox_set_px(info, x + y1, y + x1, colour);
		fox_set_px(info, x - y1, y + x1, colour);
		fox_set_px(info, x + y1, y - x1, colour);
		fox_set_px(info, x - y1, y - x1, colour);
	}
}

static void fox_draw_char(graphics_buffer_info_t* info, uint32_t x, uint32_t y, char c, uint32_t colour, psf1_font_t* font) {
	char* font_ptr = (char*) font->glyph_buffer + (c * font->psf1_Header->charsize);

	for (unsigned long i = y; i < y + 16; i++){
		for (unsigned long j = x; j < x + 8; j++){
			if ((*font_ptr & (0b10000000 >> (j - x))) > 0) {
				fox_set_px(info, j, i, colour);
			}
		}
		font_ptr++;
	}
}

static void fox_draw_string(graphics_buffer_info_t* info, uint32_t x, uint32_t y, char* str, uint32_t colour, psf1_font_t* font) {
	uint32_t i = 0;
	while (str[i] != '\0') {
		fox_draw_char(info, x + (i * 8), y, str[i], colour, font);
		i++;
	}
}

static inline int fox_abs(int x) {
	if (x < 0) {
		return -x;
	}
	return x;
}

static void fox_draw_line(graphics_buffer_info_t* info, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint32_t colour) {
	// Bresenham's line algorithm
	// https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm

	int32_t dx = fox_abs(x2 - x1);
	int32_t dy = fox_abs(y2 - y1);
	int32_t sx = x1 < x2 ? 1 : -1;
	int32_t sy = y1 < y2 ? 1 : -1;
	int32_t err = dx - dy;

	while (1) {
		fox_set_px(info, x1, y1, colour);

		if (x1 == x2 && y1 == y2) {
			break;
		}

		int32_t e2 = 2 * err;

		if (e2 > -dy) {
			err -= dy;
			x1 += sx;
		}

		if (e2 < dx) {
			err += dx;
			y1 += sy;
		}
	}
}

#ifdef __cplusplus
}
#endif