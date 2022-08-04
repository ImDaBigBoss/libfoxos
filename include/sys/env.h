#pragma once
#pragma once
#include <extern.h>

#include <stdbool.h>

#define ENV_ARGV 0
#define ENV_ENVP 1
#define ENV_ERRNO 2
#define ENV_SIGHANDLER 3
#define ENV_SET_CWD 4
#define ENV_GET_CWD 5
#define ENV_ACPI_SHUTDOWN 6
#define ENV_ENVP_SET 7
#define ENV_PIPE_DISABLE_ENABLE 8
#define ENV_GET_TASK 9
#define ENV_READ_RUNNING_TASKS 10
#define ENV_ACPI_REBOOT 11
#define ENV_GET_LOADED_MODULES 12
#define ENV_GET_SPECIAL_KEYS 13

enum signal_handlers {
	SIG_SPECIAL_KEY_UP = 33, // signals before signal 33 are exceptions
	SIG_SPECIAL_KEY_DOWN,
	SIG_INTR
};

typedef struct special_keys_down {
	bool left_shift;
	bool left_ctrl;
	bool left_alt;
	bool left_gui;

	bool right_shift;
	bool right_ctrl;
	bool right_alt;
	bool right_gui;

	bool caps_lock;
	bool num_lock;
	bool scroll_lock;

	bool up_arrow;
	bool down_arrow;
	bool left_arrow;
	bool right_arrow;
} special_keys_down_t;

EXPOSEC char** env(int mode);
EXPOSEC void env2(int mode);
EXPOSEC void env_set(int mode, void* data);

EXPOSEC void env_set2(int mode, int data, void* data2);
// EXPOSEC void env_set3(int mode, int data);
