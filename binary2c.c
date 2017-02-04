#include <stdio.h>

int main(int argc, char *argv[]) {
  FILE *src;
  FILE *dst;
  int i, total;

  if (argc != 4) {
    fprintf(stderr, "Usage: %s <name> <src> <dst>\n", argv[0]);
    return 1;
  }
  src = fopen(argv[2], "rb");
  if (!src) {
    fprintf(stderr, "Cannot open: %s\n", argv[2]);
    return 1;
  }
  dst = fopen(argv[3], "w");
  if (!dst) {
    fprintf(stderr, "Cannot open: %s\n", argv[3]);
    return 1;
  }

  fprintf(dst, "#include <stdint.h>\n\n");
  fprintf(dst, "uint8_t %s[] = {\n", argv[1]);
  i = 0;
  total = 0;
  for (;;) {
    int ch = fgetc(src);
    if (ch < 0) { break; }
    fprintf(dst, " %3d,", ch);
    ++i;
    ++total;
    if (i >= 16) {
      fprintf(dst, "\n");
      i = 0;
    }
  }
  fprintf(dst, "\n");
  fprintf(dst, "};\n\n");
  fprintf(dst, "uintptr_t %s_size = %d;\n", argv[1], total);
  fclose(dst);
  fclose(src);
  return 0;
}
