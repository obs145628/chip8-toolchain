#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "oc8_is/ins.h"

static uint8_t *read_bytes(const char *path) {
  FILE *is;
  assert((is = fopen(path, "rb")) != NULL);
  assert(fseek(is, 0, SEEK_END) == 0);
  unsigned long size = ftell(is);
  assert(fseek(is, 0, SEEK_SET) == 0);
  uint8_t *buf = (uint8_t *)malloc(size);
  assert(fread(buf, 1, size, is) == size);
  fclose(is);
  return buf;
}

int main(int argc, char **argv) {
  if (argc < 2) {
    fprintf(stderr, "Usage: ./oc8-emu <file>\n");
    return 1;
  }

  uint8_t *data = read_bytes(argv[1]);

  uint16_t b0 = *((uint16_t *)data);
  uint8_t *p8 = (uint8_t *)&b0;

  printf("val data: %x%x\n", (unsigned)data[0], (unsigned)data[1]);
  printf("val b0: %x\n", (unsigned)b0);
  printf("val p8: %x%x\n", (unsigned)p8[0], (unsigned)p8[1]);
  printf("val b0 0|1: %x%x\n", (unsigned)((b0 >> 8) & 0xFF),
         (unsigned)(b0 & 0xFF));

  oc8_is_ins_t ins;
  oc8_is_decode_ins(&ins, (const char *)data);

  free(data);
  return 0;
}
