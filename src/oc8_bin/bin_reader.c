#include "oc8_bin/bin_reader.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "oc8_bin/format.h"
#include "oc8_defs/debug.h"

static void check_magic(const oc8_bin_raw_header_t *header) {
  for (size_t i = 0; i < sizeof(header->magic); ++i)
    if (header->magic[i] != g_oc8_bin_raw_magic_value[i]) {
      fprintf(
          stderr,
          "oc8_bin_read_file_raw: Not a bin file (invalid magic number)\n.");
      PANIC();
    }
}

static void io_error(const char *path) {
  fprintf(stderr, "oc8_bin_read_from_file: Failed to read file `%s'.\n", path);
  PANIC();
}

void oc8_bin_read_file_raw(oc8_bin_file_t *f, const void *in_buf,
                           size_t buf_len) {
  oc8_bin_file_init(f);
  if (buf_len < sizeof(oc8_bin_raw_header_t)) {
    fprintf(stderr, "oc8_bin_read_file_raw: Not a bin file (invalid header)\n");
    PANIC();
  }

  const oc8_bin_raw_header_t *header = (const oc8_bin_raw_header_t *)in_buf;
  check_magic(header);
  oc8_bin_file_set_version(f, header->version);
  oc8_bin_file_set_type(
      f, header->type == 1
             ? OC8_BIN_FILE_TYPE_OBJ
             : (header->type == 2 ? OC8_BIN_FILE_TYPE_BIN : 0xFF));
  size_t nb_defs = header->nb_syms_defs;
  size_t nb_refs = header->nb_syms_refs;
  size_t rom_size = header->rom_size;
  size_t len = sizeof(oc8_bin_raw_header_t) +
               nb_defs * sizeof(oc8_bin_raw_sym_def_t) +
               nb_refs * sizeof(oc8_bin_raw_sym_ref_t) + rom_size;
  if (len != buf_len) {
    fprintf(stderr,
            "oc8_bin_read_file_raw: Invalid data. According to header, raw "
            "data should be %u bytes, but it's only %u bytes.\n",
            (unsigned)len, (unsigned)buf_len);
    PANIC();
  }

  const oc8_bin_raw_sym_def_t *defs = (const oc8_bin_raw_sym_def_t *)&header[1];
  const oc8_bin_raw_sym_ref_t *refs =
      (const oc8_bin_raw_sym_ref_t *)&defs[nb_defs];
  const void *rom_data = (const void *)&refs[nb_refs];

  for (size_t i = 0; i < nb_defs; ++i) {
    const oc8_bin_raw_sym_def_t *raw_def = &defs[i];
    oc8_bin_sym_type_t raw_type;
    switch (raw_def->type) {
    case 0:
      raw_type = OC8_BIN_SYM_TYPE_NO;
      break;
    case 1:
      raw_type = OC8_BIN_SYM_TYPE_FUN;
      break;
    case 2:
      raw_type = OC8_BIN_SYM_TYPE_OBJ;
      break;
    default:
      raw_type = 0xFF;
    };

    oc8_bin_file_add_def(f, raw_def->name, raw_def->is_global, raw_type,
                         raw_def->addr);
  }

  for (size_t i = 0; i < nb_refs; ++i) {
    const oc8_bin_raw_sym_ref_t *raw_ref = &refs[i];
    oc8_bin_file_add_ref(f, raw_ref->ins_addr, raw_ref->sym_id);
  }

  oc8_bin_file_init_rom(f, rom_size);
  memcpy(f->rom, rom_data, rom_size);
}

void oc8_bin_read_from_file(oc8_bin_file_t *f, const char *path) {
  FILE *is = fopen(path, "rb");
  if (is == NULL)
    io_error(path);
  fseek(is, 0, SEEK_END);
  size_t size = ftell(is);
  fseek(is, 0, SEEK_SET);
  if (!size)
    io_error(path);

  char *buf = malloc(size);
  if (fread(buf, 1, size, is) != size)
    io_error(path);
  oc8_bin_read_file_raw(f, buf, size);
  free(buf);
  fclose(is);
}
