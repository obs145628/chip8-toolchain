#include "oc8_bin/file.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include "oc8_defs/oc8_defs.h"

#define REFS_BASE_CAP (16)

void oc8_bin_file_init(oc8_bin_file_t *bf) {
  bf->syms_defs = NULL;
  bf->syms_defs_size = 0;
  bf->syms_defs_cap = 0;
  oc8_smap_init(&bf->globals);

  bf->syms_refs = malloc(REFS_BASE_CAP * sizeof(oc8_bin_sym_ref_t));
  bf->syms_refs_size = 0;
  bf->syms_refs_cap = REFS_BASE_CAP;

  bf->rom = NULL;
  bf->rom_size = 0;
}

void oc8_bin_file_free(oc8_bin_file_t *bf) {
  free(bf->syms_defs);
  free(bf->syms_refs);
  free(bf->rom);
  oc8_smap_free(&bf->globals);
}

void oc8_bin_file_check(oc8_bin_file_t *bf, int is_bin) {

  uint16_t max_addr = bf->rom_size + OC8_ROM_START;

  for (size_t i = 0; i < bf->syms_defs_size; ++i) {
    oc8_bin_sym_def_t *def = &bf->syms_defs[i];
    if (def->addr && (def->addr < OC8_ROM_START || def->addr >= max_addr)) {
      fprintf(stderr,
              "bin_file_check fail for symdef `%s': addr %u not in valid range "
              "[%u, %u[\n",
              def->name, (unsigned)def->addr, (unsigned)OC8_ROM_START,
              (unsigned)max_addr);
      PANIC();
    }

    if (is_bin && def->addr == 0) {
      fprintf(stderr, "bin_file_check fail for symdef `%s': not defined\n",
              def->name);
      PANIC();
    }
  }

  for (size_t i = 0; i < bf->syms_refs_size; ++i) {
    oc8_bin_sym_ref_t *ref = &bf->syms_refs[i];
    if (ref->ins_addr < OC8_ROM_START || ref->ins_addr >= max_addr) {
      fprintf(stderr,
              "bin_file_check fail for symref: addr %u not in valid range "
              "[%u, %u[\n",
              (unsigned)ref->ins_addr, (unsigned)OC8_ROM_START,
              (unsigned)max_addr);
      PANIC();
    }

    if (ref->sym_id >= bf->syms_defs_size) {
      fprintf(stderr,
              "bin_file_check fail for symref: reference def id %u, but there "
              "are only %u defs\n",
              (unsigned)ref->sym_id, (unsigned)bf->syms_defs_size);
      PANIC();
    }
  }

  if (is_bin && bf->header.type == OC8_BIN_FILE_TYPE_OBJ) {
    fprintf(stderr,
            "bin_file_check fail: file type is object and not binary\n");
    PANIC();
  }
}

void oc8_bin_file_set_version(oc8_bin_file_t *bf, uint16_t version) {
  if (version != 10) {
    fprintf(stderr,
            "bin_file_check fail: invalid version %u, Only 10 is supported.\n",
            (unsigned)version);
    PANIC();
  }
  bf->header.version = version;
}

void oc8_bin_file_set_type(oc8_bin_file_t *bf, oc8_bin_file_type_t type) {
  if (type != OC8_BIN_FILE_TYPE_OBJ && type != OC8_BIN_FILE_TYPE_OBJ) {
    fprintf(stderr, "bin_file_check fail: invalid type %u.\n", (unsigned)type);
    PANIC();
  }
  bf->header.type = type;
}

void oc8_bin_file_set_defs_count(oc8_bin_file_t *bf, size_t len) {
  if (bf->syms_defs) {
    fprintf(stderr, "bin_file_check: set_defs_count already called\n");
    PANIC();
  }

  bf->syms_defs = malloc(len * sizeof(oc8_bin_sym_def_t));
  bf->syms_defs_size = 0;
  bf->syms_defs_cap = len;
}

uint16_t oc8_bin_file_add_def(oc8_bin_file_t *bf, const char *name,
                              int is_global, oc8_bin_sym_type_t type,
                              uint16_t addr) {
  if (!bf->syms_defs || bf->syms_defs_size == bf->syms_defs_cap) {
    fprintf(stderr,
            "bin_file_check: add_def: defs not setup, or reached max size\n");
    PANIC();
  }

  uint16_t id = bf->syms_defs_size++;
  oc8_bin_sym_def_t *def = &bf->syms_defs[id];

  int i = 0;
  while (i < OC8_MAX_SYM_SIZE && name[i]) {
    char c = name[i];
    if (!isalnum(c) && c != '_') {
      fprintf(stderr, "bin_file_check: add_def: invalid symbol name\n");
      PANIC();
    }
    def->name[i++] = c;
  }
  if (i >= OC8_MAX_SYM_SIZE) {
    fprintf(stderr, "bin_file_check: add_def: symbol name too long\n");
    PANIC();
  }
  def->name[i] = '\0';
  def->is_global = !!is_global;

  if (type < OC8_BIN_SYM_TYPE_FUN || type > OC8_BIN_SYM_TYPE_NO) {
    fprintf(stderr, "bin_file_check: add_def: invalid type: %u.\n",
            (unsigned)type);
    PANIC();
  }
  def->type = type;

  def->addr = addr;

  if (is_global && !oc8_smap_insert(&bf->globals, def->name, id)) {
    fprintf(stderr,
            "bin_file_check: add_ref: there is already a global symbol `%s'.\n",
            def->name);
    PANIC();
  }

  return id;
}

void oc8_bin_file_add_ref(oc8_bin_file_t *bf, uint16_t ins_addr,
                          uint16_t sym_id) {
  if (bf->syms_refs_size == bf->syms_refs_cap) {
    bf->syms_refs_cap *= 2;
    bf->syms_refs =
        realloc(bf->syms_refs, bf->syms_refs_cap * sizeof(oc8_bin_sym_ref_t));
  }

  oc8_bin_sym_ref_t *ref = &bf->syms_refs[bf->syms_refs_size++];
  ref->ins_addr = ins_addr;
  ref->sym_id = sym_id;
}

/// Allocate memory for the ROM, can only be called once,
/// Cannot be resized later
void oc8_bin_file_init_rom(oc8_bin_file_t *bf, size_t rom_size) {
  if (bf->rom) {
    fprintf(stderr, "bin_file_check: init_rom cannot be called twice.\n");
    PANIC();
  }

  size_t max_size = OC8_MEMORY_SIZE - OC8_ROM_START;

  if (rom_size > max_size) {
    fprintf(
        stderr,
        "bin_fil_check: init_rom: ROM too big, want %u, maximum size is %u.\n",
        (unsigned)rom_size, (unsigned)max_size);
    PANIC();
  }

  bf->rom = malloc(rom_size);
  bf->rom_size = rom_size;
}
