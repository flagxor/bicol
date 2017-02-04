#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "opcodes.h"

#define MAX_NAMES 100000
#define MAX_NAME 64

static struct {
  uint32_t addr;
  uint32_t is_var;
  char name[MAX_NAME];
} names[MAX_NAMES];
static int names_size = 0;
static int string_mode = 0;
static int comment_mode = 0;
static int32_t mode = OP_CALL;
static uint32_t offset = 0;
static uint32_t user_offset = 0;
static FILE* src;
static FILE* dst;

static void emit(uint8_t value) {
  fputc(value, dst);
  ++offset;
}

static void emit16(uint16_t value) {
  emit(value);
  emit(value >> 8);
}

static void emit32(uint32_t value) {
  emit16(value);
  emit16(value >> 16);
}

static int isdigits(const char *name) {
  while (*name) {
    if (!isdigit(*name)) {
      return 0;
    }
    ++name;
  }
  return 1;
}

static void emit_string(const char *name) {
  emit16(strlen(name) + 1);
  while (*name) {
    emit(*name);
    ++name;
  }
  emit(*name);
}

static void add_word(const char *name, uint32_t addr, uint32_t is_var) {
  memset(&names[names_size], 0, sizeof(names[0]));
  names[names_size].addr = addr;
  names[names_size].is_var = is_var;
  strcpy(names[names_size].name, name);
  ++names_size;
}

static void word(const char *name) {
  int i;

  if (strlen(name) == 0) {
    return;
  }
  if (strcmp(name, "s\"") == 0) {
    string_mode = 1;
    return;
  }
  if (strcmp(name, "(") == 0) {
    comment_mode = 1;
    return;
  }
  if (strcmp(name, "'") == 0) {
    mode = OP_TICK;
    return;
  }
  if (strcmp(name, ":") == 0) {
    mode = OP_DEFINE;
    return;
  }
  if (strcmp(name, "user") == 0) {
    mode = OP_DEFUSER;
    return;
  }
  if (mode == OP_DEFINE) {
    mode = OP_CALL;
    emit(OP_DEFINE);
    emit_string(name);
    add_word(name, offset, 0);
    return;
  }
  if (mode == OP_DEFUSER) {
    mode = OP_CALL;
    emit(OP_DEFUSER);
    emit_string(name);
    add_word(name, user_offset++, 1);
    return;
  }
#define V(num, cname, wname, code) \
  if (strcmp(name, wname) == 0) { \
    emit(num); \
    return; \
  } \
  if (strncmp(name, wname ":", strlen(wname ":")) == 0 && \
    (num == OP_CALL || num == OP_TICK) && \
    sscanf(name + strlen(wname ":"), "%d", &i) == 1) { \
    emit(num); \
    emit16(i); \
    return; \
  }
  INSTRUCTIONS(V)
#undef V
  if (name[0] == '$' &&
      sscanf(name, "$%x", &i) == 1) {
    emit(OP_HEX);
    emit32(i);
    return;
  }
  if ((isdigits(name) ||
       (name[0] == '-' && isdigits(name + 1))) &&
      sscanf(name, "%d", &i) == 1) {
    emit(OP_DEC);
    emit32(i);
    return;
  }
  for (i = 0; i < names_size; ++i) {
    if (strcmp(name, names[i].name) == 0) {
      if (names[i].is_var) {
        emit(OP_USER);
        emit16(names[i].addr);
      } else {
        emit(mode);
        emit16(names[i].addr - (offset + 2));
      }
      mode = OP_CALL;
      return;
    }
  }
  fprintf(stderr, "Undefined word: %s\n", name);
  exit(1);
}

int main(int argc, char *argv[]) {
  char name[MAX_NAME * 10];
  int len;

  if (argc != 3) {
    fprintf(stderr, "Usage: %s <src> <dst>\n", argv[0]);
    return 1;
  }
  strcpy(name, "");
  src = fopen(argv[1], "r");
  if (!src) {
    fprintf(stderr, "Cannot open: %s\n", argv[1]);
    return 1;
  }
  dst = fopen(argv[2], "wb");
  if (!dst) {
    fprintf(stderr, "Cannot open: %s\n", argv[2]);
    return 1;
  }
  for (;;) {
    int ch = fgetc(src);
    if (ch < 0) {
      word(name);
      break;
    }
    if (ch == '"' && string_mode) {
      emit(OP_STRING);
      emit_string(name);
      strcpy(name, "");
      string_mode = 0;
      continue;
    }
    if (ch == ')' && comment_mode) {
      emit(OP_COMMENT);
      emit_string(name);
      strcpy(name, "");
      comment_mode = 0;
      continue;
    }
    if (!string_mode && !comment_mode &&
        (ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n')) {
      word(name);
      strcpy(name, "");
    } else {
      len = strlen(name);
      name[len] = ch;
      name[len + 1] = 0;
    }
  }
  fclose(src);
  fclose(dst);
  return 0;
}
