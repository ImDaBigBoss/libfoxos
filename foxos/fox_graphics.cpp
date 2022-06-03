#include <foxos/fox_graphics.h>

#include <foxos/window.h>
#include <sys/env.h>

standard_foxos_window_t* window;

void window_shutdown_hook() {
	fox_unregister_window(window);
}

extern "C" graphics_buffer_info_t create_window_buffer() {
	window = new standard_foxos_window_t(100, 100, 300, 300, env(ENV_ARGV)[0]);

	bool did_register = fox_register_window(window);
	if (!did_register) {
		printf("Failed to register window, there is no desktop environment running!\n");
		abort();
	}

	graphics_buffer_info_t buffer_info = window->get_buffer_info(); //This is new though

	__libc_set_shutdown_hook(window_shutdown_hook);

	return buffer_info;
}

extern "C" bool is_foxde_running() {
	return de_running();
}