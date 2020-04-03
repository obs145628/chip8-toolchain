#include "oc8_ld/linker.h"
#include "oc8_defs/debug.h"
#include "oc8_is/ins.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define UNITS_ALLOC_BASE (8)

void oc8_ld_linker_init(oc8_ld_linker_t *ld, int use_start_sym) {
  ld->units_size = 0;
  ld->units_cap = UNITS_ALLOC_BASE;
  ld->units_arr = malloc(ld->units_cap * sizeof(oc8_ld_unit_t *));
  ld->use_start_bf = 0;

  if (use_start_sym) {
    oc8_bin_file_t *bf = &ld->start_bf;
    ld->use_start_bf = 1;
    oc8_bin_file_init(bf);
    oc8_bin_file_set_version(bf, 10);
    oc8_bin_file_set_type(bf, OC8_BIN_FILE_TYPE_BIN);
    oc8_bin_file_set_defs_count(bf, 2);
    oc8_bin_file_add_def(bf, "_rom_begin", 0, OC8_BIN_SYM_TYPE_NO,
                         OC8_ROM_START);
    oc8_bin_file_add_def(bf, "_start", 1, OC8_BIN_SYM_TYPE_NO, 0);
    oc8_bin_file_add_ref(bf, OC8_ROM_START, 1); // bind jmp to _start
    oc8_bin_file_init_rom(bf, 2);

    oc8_is_ins_t jmp_ins;
    jmp_ins.opcode = 0;
    jmp_ins.type = OC8_IS_TYPE_1NNN;
    jmp_ins.operands[0] = 0;
    oc8_is_encode_ins(&jmp_ins, (char *)bf->rom);

    oc8_ld_linker_add(ld, bf);
  }
}

void oc8_ld_linker_free(oc8_ld_linker_t *ld) {
  for (size_t i = 0; i < ld->units_size; ++i) {
    oc8_ld_unit_t *unit = ld->units_arr[i];
    free(unit->syms_map);
    free(unit);
  }

  if (ld->use_start_bf)
    oc8_bin_file_free(&ld->start_bf);

  free(ld->units_arr);
}

void oc8_ld_linker_add(oc8_ld_linker_t *ld, oc8_bin_file_t *bf) {
  if (ld->units_size == ld->units_cap) {
    ld->units_cap *= 2;
    ld->units_arr =
        realloc(ld->units_arr, ld->units_cap * sizeof(oc8_ld_unit_t *));
  }

  oc8_ld_unit_t *unit = malloc(sizeof(oc8_ld_unit_t));
  unit->bf = bf;
  unit->syms_map = NULL;

  ld->units_arr[ld->units_size++] = unit;
}

static inline uint16_t check_val(uint16_t val, uint16_t max,
                                 uint16_t ins_addr) {
  if (val > max) {
    fprintf(stderr,
            "Linker cannot resolve value when fixing instruction at %x\n: "
            "value is %u, but max is %u.\n",
            (unsigned)ins_addr, (unsigned)val, (unsigned)max);
    PANIC();
  }
  return val;
}

// Change the opcode of the instruction at `ins_addr`
// `val` if the value of the label
static void fix_opcode(uint8_t *rom, uint16_t ins_addr, uint16_t val) {
  char *ins_ptr = (char *)&rom[ins_addr - OC8_ROM_START];
  oc8_is_ins_t ins;
  if (oc8_is_decode_ins(&ins, ins_ptr) != 0) {
    fprintf(stderr, "Linker failed to decode insstruction at %x.\n",
            (unsigned)ins_addr);
    PANIC();
  }
  ins.opcode = 0;

  switch (ins.type) {
  case OC8_IS_TYPE_2NNN:
  case OC8_IS_TYPE_1NNN:
  case OC8_IS_TYPE_BNNN:
  case OC8_IS_TYPE_ANNN:
  case OC8_IS_TYPE_0NNN:
    ins.operands[0] = check_val(val, 0xFFF, ins_addr);
    break;

  case OC8_IS_TYPE_7XNN:
  case OC8_IS_TYPE_6XNN:
  case OC8_IS_TYPE_CXNN:
  case OC8_IS_TYPE_3XNN:
  case OC8_IS_TYPE_4XNN:
    ins.operands[1] = check_val(val, 0xFF, ins_addr);
    break;

  case OC8_IS_TYPE_DXYN:
    ins.operands[2] = check_val(val, 0xF, ins_addr);
    break;

  default:
    fprintf(stderr,
            "Linker received an instruction to fix without immediate at %x.\n",
            (unsigned)ins_addr);
    PANIC();
  }

  ins.opcode = 0;
  if (oc8_is_encode_ins(&ins, ins_ptr) != 0) {
    fprintf(stderr, "Linker failed to encode instruction at %x.\n",
            (unsigned)ins_addr);
    PANIC();
  }
}

void oc8_ld_linker_link(oc8_ld_linker_t *ld, oc8_bin_file_t *out_bf) {

  // Initialize output bin file
  oc8_bin_file_init(out_bf);
  oc8_bin_file_set_version(out_bf, 10);
  oc8_bin_file_set_type(out_bf, OC8_BIN_FILE_TYPE_BIN);

  // Step 1)
  size_t out_rom_off = OC8_ROM_START;
  for (size_t i = 0; i < ld->units_size; ++i) {
    oc8_ld_unit_t *unit = ld->units_arr[i];
    unit->rom_addr = out_rom_off;
    out_rom_off += unit->bf->rom_size;
  }
  size_t out_rom_size = out_rom_off - OC8_ROM_START;
  if (out_rom_off > OC8_MEMORY_SIZE) {
    fprintf(stderr,
            "Output ROM too big; max address is %u, but %u is reached.\n",
            (unsigned)OC8_MEMORY_SIZE, (unsigned)out_rom_off);
    PANIC();
  }

  // Step 2)
  // Count number of defs first
  size_t nb_defs = 0;
  for (size_t i = 0; i < ld->units_size; ++i) {
    oc8_ld_unit_t *unit = ld->units_arr[i];
    for (size_t j = 0; j < unit->bf->syms_defs_size; ++j) {
      oc8_bin_sym_def_t *def = &unit->bf->syms_defs[j];
      nb_defs += def->addr != 0;
    }
  }
  oc8_bin_file_set_defs_count(out_bf, nb_defs);

  for (size_t i = 0; i < ld->units_size; ++i) {
    oc8_ld_unit_t *unit = ld->units_arr[i];
    unit->syms_map = malloc(unit->bf->syms_defs_size * sizeof(uint16_t));

    for (size_t j = 0; j < unit->bf->syms_defs_size; ++j) {
      oc8_bin_sym_def_t *def = &unit->bf->syms_defs[j];
      if (def->addr == 0)
        continue;

      const char *new_def_name = def->name;
      int new_def_global = def->is_global;
      oc8_bin_sym_type_t new_def_type = def->type;
      uint16_t new_def_addr = def->addr - OC8_ROM_START + unit->rom_addr;

      if (new_def_global &&
          oc8_smap_find(&out_bf->globals, new_def_name) != NULL) {
        fprintf(stderr, "Linker error: multiple definitions of `%s'.\n",
                new_def_name);
        PANIC();
      }

      uint16_t new_sym_id = oc8_bin_file_add_def(
          out_bf, new_def_name, new_def_global, new_def_type, new_def_addr);
      unit->syms_map[def->id] = new_sym_id;
    }
  }

  // Step 3)
  for (size_t i = 0; i < ld->units_size; ++i) {
    oc8_ld_unit_t *unit = ld->units_arr[i];
    for (size_t j = 0; j < unit->bf->syms_defs_size; ++j) {
      oc8_bin_sym_def_t *def = &unit->bf->syms_defs[j];
      if (def->addr != 0)
        continue;

      oc8_smap_node_t *node = oc8_smap_find(&out_bf->globals, def->name);
      if (node == NULL) {
        fprintf(stderr, "Linker error: undefined reference to `%s'.\n",
                def->name);
        PANIC();
      }
      unit->syms_map[def->id] = (uint16_t)node->val;
    }
  }

  // Step 4)
  for (size_t i = 0; i < ld->units_size; ++i) {
    oc8_ld_unit_t *unit = ld->units_arr[i];
    for (size_t j = 0; j < unit->bf->syms_refs_size; ++j) {
      oc8_bin_sym_ref_t *ref = &unit->bf->syms_refs[j];
      uint16_t new_ins_addr = ref->ins_addr - OC8_ROM_START + unit->rom_addr;
      uint16_t new_sym_id = unit->syms_map[ref->sym_id];
      assert(new_sym_id < nb_defs);

      oc8_bin_file_add_ref(out_bf, new_ins_addr, new_sym_id);
    }
  }

  // Step 5)
  oc8_bin_file_init_rom(out_bf, out_rom_size);
  for (size_t i = 0; i < ld->units_size; ++i) {
    oc8_ld_unit_t *unit = ld->units_arr[i];
    memcpy(out_bf->rom + unit->rom_addr - OC8_ROM_START, unit->bf->rom,
           unit->bf->rom_size);
  }

  // Step 6)
  for (size_t i = 0; i < out_bf->syms_refs_size; ++i) {
    oc8_bin_sym_ref_t *ref = &out_bf->syms_refs[i];
    uint16_t val = out_bf->syms_defs[ref->sym_id].addr;
    fix_opcode(out_bf->rom, ref->ins_addr, val);
  }
}
