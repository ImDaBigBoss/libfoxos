#include <sys/ipc.h>
#include <sys/get_syscall_id.h>

int sys_ipc_register_id = -1;
int sys_ipc_unregister_id = -1;
int sys_ipc_get_hid_id = -1;
int sys_ipc_send_message_id = -1;

int ipc_register(char* name, ipc_callback_f callback) {
	if (sys_ipc_register_id == -1) {
		sys_ipc_register_id = get_syscall_id("sys_ipc_register");
	}

	int hid;
	__asm__ __volatile__ ("int $0x30" : "=d" (hid) : "a" (sys_ipc_register_id), "b" (name), "c" (callback));

	return hid;
}

void ipc_unregister(int hid) {
	if (sys_ipc_unregister_id == -1) {
		sys_ipc_unregister_id = get_syscall_id("sys_ipc_unregister");
	}

	__asm__ __volatile__ ("int $0x30" : : "a" (sys_ipc_unregister_id), "b" (hid));
}

int ipc_get_hid(char* name) {
	if (sys_ipc_get_hid_id == -1) {
		sys_ipc_get_hid_id = get_syscall_id("sys_ipc_get_hid");
	}

	int hid;
	__asm__ __volatile__ ("int $0x30" : "=d" (hid) : "a" (sys_ipc_get_hid_id), "b" (name));

	return hid;
}

void ipc_send_message(int hid, int func, void* data) {
	if (sys_ipc_send_message_id == -1) {
		sys_ipc_send_message_id = get_syscall_id("sys_ipc_send_message");
	}

	__asm__ __volatile__ ("int $0x30" : : "a" (sys_ipc_send_message_id), "b" (hid), "c" (func), "d" (data));
}