#include <foxos/window.h>

#include <sys/ipc.h>

#include <stdint.h>
#include <stdbool.h>

bool de_running() {
    int hid = ipc_get_hid(standard_foxos_de_ipc_name);
    return hid != -1;
}

bool fox_register_window(standard_foxos_window_t* window) {
    int hid = ipc_get_hid(standard_foxos_de_ipc_name);
    if (hid == -1) {
        return false;
    }

    ipc_send_message(hid, IPC_CREATE_WINDOW, (void*) window);

    return true;
}

bool fox_unregister_window(standard_foxos_window_t* window) {
    int hid = ipc_get_hid(standard_foxos_de_ipc_name);
    if (hid == -1) {
        return false;
    }

    ipc_send_message(hid, IPC_DESTORY_WINDOW, (void*) window);

    return true;
}