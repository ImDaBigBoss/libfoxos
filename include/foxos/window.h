#pragma once

#include <sys/ipc.h>

#include <foxos/fox_graphics.h>

#include <stdint.h>

#define standard_foxos_de_ipc_name (char*) "foxos_de"

enum window_ipc_command {
    IPC_GET_WINDOW_INFO,
	IPC_CREATE_WINDOW,
    IPC_DESTORY_WINDOW,
};

typedef struct {
    uint8_t buffer_width_diff; //buffer_width = width - buffer_width_diff
    uint8_t buffer_height_diff; //buffer_height = height - buffer_height_diff

    uint8_t min_width;
    uint8_t min_height;

    uint32_t background_colour;
} standard_foxos_window_info_t;

class standard_foxos_window_t {
    public:
        standard_foxos_window_t(int64_t x, int64_t y, int64_t width, int64_t height, char* title);
        ~standard_foxos_window_t();

        int64_t get_x();
        int64_t get_y();
        int64_t get_width();
        int64_t get_height();

        int64_t get_buffer_x();
        int64_t get_buffer_y();
        int64_t get_buffer_width();
        int64_t get_buffer_height();

        void move(int64_t x, int64_t y);
        void resize(int64_t width, int64_t height);

        void set_title(char* title);
        char* get_title();
        uint8_t get_title_length();

        graphics_buffer_info_t get_buffer_info();

        uint32_t* buffer = 0;
        size_t buffer_size = 0;

		bool frame_ready = false;

    private:
        standard_foxos_window_info_t window_info;

        void calculate_buffer_size();

        int64_t window_x = 0;
        int64_t window_y = 0;
        int64_t window_width = 0;
        int64_t window_height = 0;

        int64_t buffer_x = 0;
        int64_t buffer_y = 0;
        int64_t buffer_width = 0;
        int64_t buffer_height = 0;

        char* window_title = 0;
        uint8_t title_length = 0;
};

bool de_running();
bool fox_register_window(standard_foxos_window_t* window);
bool fox_unregister_window(standard_foxos_window_t* window);