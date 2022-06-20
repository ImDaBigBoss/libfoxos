#include <sys/insmod.h>
#include <sys/get_syscall_id.h>

int sys_insmod_id = -1;

void insmod(void* module, uint32_t size) {
	if (sys_insmod_id == -1) {
		sys_insmod_id = get_syscall_id("sys_insmod");
	}

	__asm__ __volatile__ ("int $0x30" : : "a" (sys_insmod_id), "b" (module), "c" (size));
}