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
static int32_t offset = 0;
static int32_t user_offset = 0;
static int32_t last_op = -1;
static FILE* src;
static FILE* dst;

static uint8_t read8() {
  ++offset;
  return fgetc(src);
}

static uint16_t read16() {
  uint16_t a, b;
  a = read8();
  b = read8();
  return a | (b << 8);
}

static uint32_t read32() {
  uint32_t a, b;
  a = read16();
  b = read16();
  return a | (b << 16);
}

static void read_string(char *name, int32_t *len) {
  int i;
  *len = read16() - 1;
  if (*len < 0 || *len > MAX_NAME * 10) {
    fprintf(stderr, "Bad name length at: %d, of: %d\n", offset, *len);
    exit(1);
  }
  for (i = 0; i < 1 + *len; ++i) {
    *name = read8();
    ++name;
  }
}

static void add_word(uint8_t op) {
  char name[MAX_NAME * 10];
  int32_t len;

  if (op == OP_DEFINE || op == OP_DEFUSER) {
    memset(&names[names_size], 0, sizeof(names[0]));
    read_string(names[names_size].name, &len);
    if (op == OP_DEFUSER) {
      names[names_size].addr = user_offset++;
    } else {
      names[names_size].addr = offset;
    }
    names[names_size].is_var = op == OP_DEFUSER;
    ++names_size;
  } else if (op == OP_STRING || op == OP_COMMENT) {
    read_string(name, &len);
  } else if (op == OP_DEC || op == OP_HEX) {
    read32();
  } else if (op == OP_CALL || op == OP_TICK || op == OP_USER) {
    read16();
  }
}

static void word(uint8_t op) {
  char name[MAX_NAME * 10];
  int32_t len;
  int32_t operand;
  int i;

  fflush(dst);
  if (op == OP_DEC) {
    fprintf(dst, " %d", (int32_t) read32());
    last_op = op;
    return;
  }
  if (op == OP_HEX) {
    fprintf(dst, " $%x", (uint32_t) read32());
    last_op = op;
    return;
  }
  if (op == OP_CALL || op == OP_TICK) {
    operand = (int16_t) read16();
    for (i = 0; i < names_size; ++i) {
      if (names[i].addr ==
          (int32_t) offset + (int32_t) operand && !names[i].is_var) {
        if (op == OP_TICK) {
          fprintf(dst, " '");
        }
        fprintf(dst, " %s", names[i].name);
        last_op = op;
        return;
      }
    }
    fprintf(dst, " call:%d", operand);
    last_op = op;
    return;
  }
  if (op == OP_USER) {
    operand = (uint16_t) read16();
    for (i = 0; i < names_size; ++i) {
      if (names[i].addr == operand && names[i].is_var) {
        fprintf(dst, " %s", names[i].name);
        last_op = op;
        return;
      }
    }
    fprintf(dst, " user:%d", operand);
    last_op = op;
    return;
  }
  if (op == OP_STRING) {
    fprintf(dst, " s\" ");
    read_string(name, &len);
    fwrite(name, 1, len, dst);
    fprintf(dst, "\"");
    last_op = op;
    return;
  }
  if (op == OP_DASH) {
    fprintf(dst, "\n  --");
    last_op = op;
    return;
  }
  if (op == OP_DEFINE) {
    read_string(name, &len);
    fprintf(dst, "\n: %s", name);
    last_op = op;
    return;
  } else if (op == OP_DEFUSER) {
    read_string(name, &len);
    fprintf(dst, "\nuser %s", name);
    ++user_offset;
    last_op = op;
    return;
  } else if (op == OP_COMMENT) {
    if(last_op == OP_DEFINE) {
      fprintf(dst, " ( ");
    } else {
      if (last_op != -1) {
        fprintf(dst, "\n\n");
      }
      fprintf(dst, "( ");
    }
    read_string(name, &len);
    fprintf(dst, "%s)", name);
    last_op = op;
    return;
  }
#define V(num, _, wname, code) \
  if (op == num) { \
    fprintf(dst, " %s", wname); \
    last_op = op; \
    return; \
  }
  INSTRUCTIONS(V)
#undef V
  fprintf(stderr, "Bad opcode: %d at: %d\n", op, offset);
  exit(1);
}

int main(int argc, char *argv[]) {
  char name[MAX_NAME];
  uint8_t op;

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
  offset = 0;
  user_offset = 0;
  last_op = -1;
  for (;;) {
    if (fread(&op, 1, 1, src) != 1) {
      break;
    }
    add_word(op);
    offset++;
  }
  fseek(src, 0, SEEK_SET);
  offset = 0;
  user_offset = 0;
  last_op = -1;
  for (;;) {
    if (fread(&op, 1, 1, src) != 1) {
      break;
    }
    word(op);
    offset++;
  }
  fprintf(dst, "\n");
  fclose(src);
  fclose(dst);
  return 0;
}
