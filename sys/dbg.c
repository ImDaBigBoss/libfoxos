#include <sys/dbg.h>
#include <sys/get_syscall_id.h>
#include <stdio.h>

int sys_dbg_id = -1;

void dbg(char* msg) {
	if (sys_dbg_id == -1) {
		sys_dbg_id = get_syscall_id("sys_dbg");
	}

	int socket_id = 0;

	__asm__ __volatile__ ("int $0x30" :: "a" (sys_dbg_id), "b" (msg));
}


int debugf_intrnl(const char *fmt, ...) {
	char printf_buf[1024] = { 0 };
	va_list args;
	int printed;

	va_start(args, fmt);
	printed = vsprintf(printf_buf, fmt, args);
	va_end(args);

	dbg(printf_buf);
}