#include "interpreter.h"
#include "opcodes.h"
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>

typedef union _cell_t {
  uintptr_t u;
  intptr_t i;
  union _cell_t *a;
  unsigned char *b;
  uint16_t *s;
  uint32_t *l;
  uint8_t *ip;
  void *v;
  union _cell_t (*f0)(void);
  union _cell_t (*f1)(union _cell_t);
  union _cell_t (*f2)(union _cell_t, union _cell_t);
  union _cell_t (*f3)(union _cell_t, union _cell_t, union _cell_t);
  union _cell_t (*f4)(union _cell_t, union _cell_t, union _cell_t,
                      union _cell_t);
  union _cell_t (*f5)(union _cell_t, union _cell_t, union _cell_t,
                      union _cell_t, union _cell_t);
  union _cell_t (*f6)(union _cell_t, union _cell_t, union _cell_t,
                      union _cell_t, union _cell_t, union _cell_t);
  union _cell_t (*f7)(union _cell_t, union _cell_t, union _cell_t,
                      union _cell_t, union _cell_t, union _cell_t,
                      union _cell_t);
  union _cell_t (*f8)(union _cell_t, union _cell_t, union _cell_t,
                      union _cell_t, union _cell_t, union _cell_t,
                      union _cell_t, union _cell_t);
} cell_t;

void interpret(void *code, void *dstack, void *rstack, void *user) {
  cell_t tos, *sp = dstack, *rp = rstack, *up = user, t;
  uint8_t *ip = code, op;
  tos = *sp--;
  for (;;) {
    op = *ip++;
#if 0
#define V(num, _, name, _junk) \
    if (op == num) { \
      fprintf(stderr, "%d:%s\n", op, name); \
    }
    INSTRUCTIONS(V)
#undef V
#endif
    switch (op) {
#define V(num, _, _junk, code) case num: code; break;
			INSTRUCTIONS(V)
#undef V
      default:
        fprintf(stderr, "Bad opcode!!! %d\n", op);
        exit(1);
        break;
    }
  }
}
