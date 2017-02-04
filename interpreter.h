#ifndef _interpreter_h_
#define _interpreter_h_

#include <stdint.h>

extern void interpret(void *ip, void *dstack, void *rstack, void *user);

#endif
