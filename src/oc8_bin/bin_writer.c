#include "oc8_bin/bin_writer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "oc8_bin/format.h"
#include "oc8_defs/debug.h"

size_t oc8_bin_write_file_raw(oc8_bin_file_t *f, void *out_buf) {
  char *cbuf = (char *)out_buf;
  size_t nb_defs = f->syms_defs_size;
  size_t nb_refs = f->syms_refs_size;
  size_t rom_size = f->rom_size;
  size_t len = sizeof(oc8_bin_raw_header_t) +
               nb_defs * sizeof(oc8_bin_raw_sym_def_t) +
               nb_refs * sizeof(oc8_bin_raw_sym_ref_t) + rom_size;
  if (out_buf == NULL)
    return len;

  oc8_bin_raw_header_t *header = (oc8_bin_raw_header_t *)&cbuf[0];
  oc8_bin_raw_sym_def_t *defs = (oc8_bin_raw_sym_def_t *)&header[1];
  oc8_bin_raw_sym_ref_t *refs = (oc8_bin_raw_sym_ref_t *)&defs[nb_defs];
  void *rom_data = (void *)&refs[nb_refs];

  memcpy(&header->magic[0], g_oc8_bin_raw_magic_value, sizeof(header->magic));
  header->version = 10;
  header->type = f->header.type == OC8_BIN_FILE_TYPE_OBJ ? 1 : 2;
  header->nb_syms_defs = nb_defs;
  header->nb_syms_refs = nb_refs;
  header->rom_size = rom_size;

  for (size_t i = 0; i < nb_defs; ++i) {
    oc8_bin_raw_sym_def_t *raw_def = &defs[i];
    oc8_bin_sym_def_t *def = &f->syms_defs[i];
    memcpy(raw_def->name, def->name, sizeof(raw_def->name));
    raw_def->is_global = def->is_global ? 1 : 0;
    raw_def->type = def->type == OC8_BIN_SYM_TYPE_FUN
                        ? 1
                        : (def->type == OC8_BIN_SYM_TYPE_OBJ ? 2 : 0);
    raw_def->addr = def->addr;
  }

  for (size_t i = 0; i < nb_refs; ++i) {
    oc8_bin_raw_sym_ref_t *raw_ref = &refs[i];
    oc8_bin_sym_ref_t *ref = &f->syms_refs[i];
    raw_ref->ins_addr = ref->ins_addr;
    raw_ref->sym_id = ref->sym_id;
  }

  memcpy(rom_data, f->rom, rom_size);

  return len;
}

void oc8_bin_write_to_file(oc8_bin_file_t *f, const char *path) {
  FILE *os = fopen(path, "wb");
  if (os == NULL) {
    fprintf(stderr, "oc8_bin_write_to_file: Failed to open output file `%s'.\n",
            path);
    PANIC();
  }

  size_t len = oc8_bin_write_file_raw(f, NULL);
  void *buf = malloc(len);
  oc8_bin_write_file_raw(f, buf);
  if (fwrite(buf, 1, len, os) != len) {
    fprintf(stderr,
            "oc8_bin_write_to_file: Failed to write to output file `%s'.\n",
            path);
    PANIC();
  }

  free(buf);
  fclose(os);
}
