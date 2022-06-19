#include <foxos/window.h>

foxos_button::foxos_button(int x, int y, int width, int height, foxos_button_callback_t callback) {
	this->x = x;
	this->y = y;
	this->width = width;
	this->height = height;
	this->callback = callback;
}

void foxos_button::draw_outline(uint32_t color, graphics_buffer_info_t* info) {
	fox_draw_line(info, x, y, x + width, y, color);
	fox_draw_line(info, x, y, x, y + height, color);
	fox_draw_line(info, x + width, y, x + width, y + height, color);
	fox_draw_line(info, x, y + height, x + width, y + height, color);
}