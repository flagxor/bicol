#include "interpreter.h"
#include <stdio.h>
#include <SDL2/SDL.h>
#include <dlfcn.h>

extern uint8_t boot[];
extern uintptr_t boot_size;

int main() {
  intptr_t dstack[1024] = {1234, 5678, 0};
  intptr_t rstack[1024];
  intptr_t user[4096];
  interpret(boot + boot_size - 4, dstack + 2, rstack, user);
  return 0;
}
