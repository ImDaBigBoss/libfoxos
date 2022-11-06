#include <sys/clock.h>

#include <sys/get_syscall_id.h>

int sys_clock_id = -1;
int sys_clock_ticks_per_second_id = -1;

uint64_t _clock() {
	if (sys_clock_id == -1) {
		sys_clock_id = get_syscall_id("sys_clock");
	}

	uint64_t ret;
	__asm__ __volatile__ ("int $0x30" : "=b" (ret) : "a" (sys_clock_id));

	return ret;
}

uint64_t _clock_ticks_per_second() {
	if (sys_clock_ticks_per_second_id == -1) {
		sys_clock_ticks_per_second_id = get_syscall_id("sys_clock_ticks_per_second");
	}

	uint64_t ret;
	__asm__ __volatile__ ("int $0x30" : "=b" (ret) : "a" (sys_clock_ticks_per_second_id));

	return ret;
}
