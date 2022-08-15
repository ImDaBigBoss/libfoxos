#pragma once

#include <extern.h>

EXPOSEC void dbg(char* msg);
EXPOSEC int debugf_intrnl(const char *fmt, ...);

#define debugf(fmt, ...) debugf_intrnl("[%s:%d] " fmt,  __FILE__, __LINE__, ##__VA_ARGS__)
