#pragma once

#include <extern.h>

typedef void (*ipc_callback_f)(int func, void* data);

EXPOSEC int ipc_register(char* name, ipc_callback_f callback);
EXPOSEC void ipc_unregister(int hid);
EXPOSEC int ipc_get_hid(char* name);
EXPOSEC void ipc_send_message(int hid, int func, void* data);