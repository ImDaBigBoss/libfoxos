#pragma once

#include <sys/ipc.h>
#include <sys/env.h>

#include <foxos/fox_graphics.h>
#include <foxos/list.h>

#include <stdint.h>
#include <stdbool.h>

#define standard_foxos_de_ipc_name (char*) "foxos_de"

enum window_ipc_command {
    IPC_GET_WINDOW_INFO,
	IPC_CREATE_WINDOW,
    IPC_DESTORY_WINDOW
};

typedef struct {
    uint8_t buffer_width_diff; //buffer_width = width - buffer_width_diff
    uint8_t buffer_height_diff; //buffer_height = height - buffer_height_diff

    uint8_t min_width;
    uint8_t min_height;

    uint32_t background_colour;
} standard_foxos_window_info_t;

typedef void (*foxos_click_callback_t) (int64_t, int64_t, mouse_buttons_e);
typedef void (*foxos_special_key_down_callback_t) (special_key_e);
typedef void (*foxos_special_key_up_callback_t) (special_key_e);

typedef struct {
    bool entire_window;

    int64_t x;
    int64_t y;
    int64_t width;
    int64_t height;

    foxos_click_callback_t callback;
} foxos_click_callback_list_node_t;

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

		bool exit_button = false;
		bool should_exit = false;

		uint32_t* old_frame;

        int add_click_listener(foxos_click_callback_t callback);
		int add_click_listener(foxos_click_callback_t callback, int64_t x, int64_t y, int64_t width, int64_t height);
        bool remove_click_listener(int id);
		void send_click(int64_t mouse_x, int64_t mouse_y, mouse_buttons_e mouse_button);

        int add_special_key_down_listener(foxos_special_key_down_callback_t callback);
        bool remove_special_key_down_listener(int id);
        void send_special_key_down(special_key_e key);

        int add_special_key_up_listener(foxos_special_key_up_callback_t callback);
        bool remove_special_key_up_listener(int id);
        void send_special_key_up(special_key_e key);
        
        special_keys_down_t* get_special_keys_down();

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

        int64_t exit_button_x = 0;
		int64_t exit_button_y = 0;

		list_t<foxos_click_callback_list_node_t>* click_callback_list;
        list_t<foxos_special_key_down_callback_t>* special_key_down_callback_list;
        list_t<foxos_special_key_up_callback_t>* special_key_up_callback_list;
};

bool de_running();
bool fox_register_window(standard_foxos_window_t* window);
bool fox_unregister_window(standard_foxos_window_t* window);