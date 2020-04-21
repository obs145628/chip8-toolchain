#define _GNU_SOURCE

#include "emu_odb/vm_api.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "oc8_bin/printer.h"
#include "oc8_defs/consts.h"
#include "oc8_emu/cpu.h"
#include "oc8_emu/debug.h"
#include "oc8_emu/mem.h"

// Registers mapping:
//  - V0 - VF: 0 - 15 (1 byte)
//  - PC: 16 (2 bytes)
//  - I: 17 (2 bytes)
//  - SP: 18 (1 byte)
//  - DT: 19 (1 byte) (Delay Timer)
//  - ST: 20 (1 byte) (Sound Timer)
//
// Memory: 0000 - 1000
// System: 0000 - 0200
// Reading system memory gives 0
// Writin system memory does nothing

#define REG_PC (16)
#define REG_I (17)
#define REG_SP (18)
#define REG_DT (19)
#define REG_ST (20)
#define NB_REGS (21)

static const odb_vm_reg_t regs_general[] = {
    0,  1,  2,  3,  4,  5,  6,     7,      8,     9,
    10, 11, 12, 13, 14, 15, REG_I, REG_DT, REG_ST};
static const odb_vm_reg_t regs_program_counter[] = {REG_PC};
static const odb_vm_reg_t regs_stack_pointer[] = {REG_SP};

static const char *regs_names[] = {"v0", "v1", "v2", "v3", "v4", "v5", "v6",
                                   "v7", "v8", "v9", "va", "vb", "vc", "vd",
                                   "ve", "vf", "pc", "i",  "sp", "dt", "st"};

typedef struct {
  uint16_t old_pc;
  uint8_t old_sp;
  uint16_t *sorted_defs; // array of def ids, sorted by addr order
  oc8_bin_printer_t printer;
} api_data_t;

static int cmp_fn(const void *a, const void *b, void *arg) {
  uint16_t ia = *((const uint16_t *)a);
  uint16_t ib = *((const uint16_t *)b);
  oc8_bin_file_t *bf = arg;

  uint16_t pa = bf->syms_defs[ia].addr;
  uint16_t pb = bf->syms_defs[ib].addr;
  return pa < pb ? -1 : pa != pb;
}

static void get_vm_infos(odb_vm_api_data_t data, odb_vm_api_error_t *err,
                         odb_vm_infos_t *out_infos) {
  (void)err;
  api_data_t *ad = (api_data_t *)data;
  ad->old_pc = 0;
  oc8_emu_cpu_t *cpu = &g_oc8_emu_cpu;
  ad->old_sp = cpu->reg_sp;
  oc8_bin_file_t *bf = &g_oc8_emu_bin_file;

  out_infos->name = "oc8_emu";
  out_infos->regs_general = regs_general;
  out_infos->regs_program_counter = regs_program_counter;
  out_infos->regs_stack_pointer = regs_stack_pointer;
  out_infos->regs_base_pointer = NULL;
  out_infos->regs_flags = NULL;
  out_infos->regs_general_size = 19;
  out_infos->regs_program_counter_size = 1;
  out_infos->regs_stack_pointer_size = 1;
  out_infos->regs_base_pointer_size = 0;
  out_infos->regs_flags_size = 0;
  out_infos->regs_count = NB_REGS;
  out_infos->memory_size = OC8_MEMORY_SIZE;
  out_infos->symbols_count = bf->syms_defs_size;
  out_infos->pointer_size = 2;
  out_infos->integer_size = 1;
  out_infos->use_opcode = 1;
}

static void get_update_infos(odb_vm_api_data_t data, odb_vm_api_error_t *err,
                             odb_vm_api_update_infos_t *out_udp) {
  (void)err;
  api_data_t *ad = (api_data_t *)data;
  oc8_emu_cpu_t *cpu = &g_oc8_emu_cpu;

  uint16_t old_pc = ad->old_pc;
  uint8_t old_sp = ad->old_sp;
  ad->old_pc = cpu->reg_pc;
  ad->old_sp = cpu->reg_sp;
  out_udp->act_addr = cpu->reg_pc;

  if (old_pc == 0) {
    out_udp->state = ODB_VM_API_UPDATE_STATE_OK;
  }

  else if (cpu->reg_sp > old_sp) {
    out_udp->state = ODB_VM_API_UPDATE_STATE_CALL_SUB;
  }

  else if (cpu->reg_sp < old_sp) {
    out_udp->state = ODB_VM_API_UPDATE_STATE_RET_SUB;
  }

  else {
    out_udp->state = ODB_VM_API_UPDATE_STATE_OK;
  }
}

static void get_reg_infos(odb_vm_api_data_t data, odb_vm_api_error_t *err,
                          odb_vm_reg_t idx, odb_reg_infos_t *out_reg) {
  (void)data;
  if (idx >= NB_REGS) {
    strcpy(err->msg, "Invalid register identifier");
    return;
  }

  strcpy(out_reg->name, regs_names[idx]);
  out_reg->idx = idx;
  out_reg->size = (idx == REG_PC || idx == REG_I) ? 2 : 1;
  if (idx == REG_PC)
    out_reg->kind = ODB_REG_KIND_PROGRAM_COUNTER;
  else if (idx == REG_SP)
    out_reg->kind = ODB_REG_KIND_STACK_POINTER;
  else
    out_reg->kind = ODB_REG_KIND_GENERAL;
}

static void get_reg_val(odb_vm_api_data_t data, odb_vm_api_error_t *err,
                        odb_vm_reg_t idx, void *out_buf) {
  (void)data;
  if (idx >= NB_REGS) {
    strcpy(err->msg, "Invalid register identifier");
    return;
  }
  oc8_emu_cpu_t *cpu = &g_oc8_emu_cpu;

  if (idx <= 0xF)
    memcpy(out_buf, &cpu->regs_data[idx], 1);
  else if (idx == REG_PC)
    memcpy(out_buf, &cpu->reg_pc, 2);
  else if (idx == REG_I)
    memcpy(out_buf, &cpu->reg_i, 2);
  else if (idx == REG_SP)
    memcpy(out_buf, &cpu->reg_sp, 1);
  else if (idx == REG_DT)
    memcpy(out_buf, &cpu->reg_dt, 1);
  else if (idx == REG_ST)
    memcpy(out_buf, &cpu->reg_st, 1);
}

static void set_reg(odb_vm_api_data_t data, odb_vm_api_error_t *err,
                    odb_vm_reg_t idx, const void *buf) {
  (void)data;
  if (idx >= NB_REGS) {
    strcpy(err->msg, "Invalid register identifier");
    return;
  }
  oc8_emu_cpu_t *cpu = &g_oc8_emu_cpu;

  if (idx <= 0xF)
    memcpy(&cpu->regs_data[idx], buf, 1);
  else if (idx == REG_PC)
    memcpy(&cpu->reg_pc, buf, 2);
  else if (idx == REG_I)
    memcpy(&cpu->reg_i, buf, 2);
  else if (idx == REG_SP)
    memcpy(&cpu->reg_sp, buf, 1);
  else if (idx == REG_DT)
    memcpy(&cpu->reg_dt, buf, 1);
  else if (idx == REG_ST)
    memcpy(&cpu->reg_st, buf, 1);
}

static odb_vm_reg_t find_reg_id(odb_vm_api_data_t data, odb_vm_api_error_t *err,
                                const char *name) {
  (void)data;
  for (size_t i = 0; i < NB_REGS; ++i)
    if (strcmp(name, regs_names[i]) == 0)
      return i;

  strcpy(err->msg, "Invalid register name");
  return 0;
}

static void read_mem(odb_vm_api_data_t data, odb_vm_api_error_t *err,
                     odb_vm_ptr_t addr, odb_vm_size_t size, void *out_buf) {
  (void)data;
  if (addr >= OC8_MEMORY_SIZE || addr + size > OC8_MEMORY_SIZE) {
    strcpy(err->msg, "Memory address out of bounds");
    return;
  }
  char *buf = (char *)out_buf;
  oc8_emu_mem_t *mem = &g_oc8_emu_mem;

  // Reading 0's from system memory
  if (addr < OC8_ROM_START) {
    odb_vm_ptr_t nzer = OC8_ROM_START - addr;
    if (nzer > size)
      nzer = size;
    memset(buf, 0, nzer);
    addr += nzer;
    size -= nzer;
    buf += nzer;
  }
  if (size == 0)
    return;

  memcpy(buf, &mem->ram[addr], size);
}

static void write_mem(odb_vm_api_data_t data, odb_vm_api_error_t *err,
                      odb_vm_ptr_t addr, odb_vm_size_t size, const void *buf) {
  (void)data;
  if (addr >= OC8_MEMORY_SIZE || addr + size > OC8_MEMORY_SIZE) {
    strcpy(err->msg, "Memory address out of bounds");
    return;
  }
  const char *cbuf = (const char *)buf;
  oc8_emu_mem_t *mem = &g_oc8_emu_mem;

  // Ignore system memory
  if (addr < OC8_ROM_START) {
    odb_vm_ptr_t nzer = OC8_ROM_START - addr;
    if (nzer > size)
      nzer = size;
    addr += nzer;
    size -= nzer;
    cbuf += nzer;
  }
  if (size == 0)
    return;

  memcpy(&mem->ram[addr], cbuf, size);
}

static void get_symbols(odb_vm_api_data_t data, odb_vm_api_error_t *err,
                        odb_vm_ptr_t addr, odb_vm_size_t size,
                        odb_vm_sym_t *out_syms, size_t *out_nb_syms,
                        odb_vm_size_t *out_act_size) {
  if (addr >= OC8_MEMORY_SIZE || addr + size > OC8_MEMORY_SIZE) {
    strcpy(err->msg, "Memory address out of bounds");
    return;
  }
  api_data_t *ad = (api_data_t *)data;
  oc8_bin_file_t *bf = &g_oc8_emu_bin_file;
  size_t count = 0;
  odb_vm_size_t act_size = 0;

  uint16_t vbeg = addr;
  uint16_t vend = addr + size;

  // Go through all addresses in sorted order
  size_t i;
  for (i = 0; i < bf->syms_defs_size; ++i) {
    oc8_bin_sym_def_t *def = &bf->syms_defs[ad->sorted_defs[i]];
    act_size = (odb_vm_ptr_t)def->addr - addr + 1;
    if (def->addr < vbeg)
      continue;
    if (def->addr >= vend) {
      act_size = size;
      break;
    }

    out_syms[count++] = def->id;
    if (count == ODB_VM_API_SYMS_LIST_CAP)
      break;
  }

  if (i == bf->syms_defs_size)
    act_size = size;

  *out_nb_syms = count;
  *out_act_size = act_size;
}

static void get_symb_infos(odb_vm_api_data_t data, odb_vm_api_error_t *err,
                           odb_vm_sym_t idx, odb_symbol_infos_t *out_sym) {
  (void)data;
  oc8_bin_file_t *bf = &g_oc8_emu_bin_file;
  if ((size_t)idx >= bf->syms_defs_size) {
    strcpy(err->msg, "Invalid symbol index");
    return;
  }

  oc8_bin_sym_def_t *def = &bf->syms_defs[idx];
  // no need to check size: max symbol size in oc8 smaller than buffer size
  strcpy(out_sym->name, def->name);
  out_sym->addr = def->addr;
  out_sym->idx = def->id;
}

static odb_vm_sym_t find_sym_id(odb_vm_api_data_t data, odb_vm_api_error_t *err,
                                const char *name) {
  (void)data;
  oc8_bin_file_t *bf = &g_oc8_emu_bin_file;
  oc8_bin_sym_def_t *res = NULL;

  for (size_t i = 0; i < bf->syms_defs_size; ++i) {
    oc8_bin_sym_def_t *def = &bf->syms_defs[i];
    if (strcmp(def->name, name) == 0) {
      res = def;
      break;
    }
  }

  if (res == NULL) {
    strcpy(err->msg, "Invalid symbol name");
    return 0;
  } else
    return res->id;
}

static void get_code_text(odb_vm_api_data_t data, odb_vm_api_error_t *err,
                          odb_vm_ptr_t addr, char *out_text,
                          odb_vm_size_t *out_addr_dist) {
  api_data_t *ad = (api_data_t *)data;
  oc8_bin_file_t *bf = &g_oc8_emu_bin_file;
  if (addr >= OC8_MEMORY_SIZE) {
    strcpy(err->msg, "Memory address out of bounds");
    return;
  }
  if (addr < OC8_ROM_START || addr >= OC8_ROM_START + bf->rom_size) {
    out_text[0] = 0;
    return;
  }

  // Use printer to get str code
  size_t buf_size;
  uint16_t inc;
  int res = oc8_bin_printer_print_at(&ad->printer, addr,
                                     /*print_sym_defs=*/0, /*print_sym_refs=*/1,
                                     /*print_sym_defs=*/1, /*print_opcode=*/0,
                                     /*data_hint=*/OC8_BIN_PRINTER_DATA_UNKOWN,
                                     &buf_size, &inc);
  if (buf_size == 0 || res != 0) {
    strcpy(err->msg, "Failed to get code text");
    return;
  }

  // Write str code to buffer
  if (buf_size > ODB_VM_API_TEXT_INS_CAP)
    buf_size = ODB_VM_API_TEXT_INS_CAP;
  memcpy(out_text, ad->printer.out_buf, buf_size);
  out_text[buf_size] = '\0';

  *out_addr_dist = (odb_vm_size_t)inc;
}

static void cleanup(odb_vm_api_data_t data) {
  api_data_t *ad = (api_data_t *)data;
  oc8_bin_printer_free(&ad->printer);
  free(ad->sorted_defs);
  free(ad);
}

odb_vm_api_data_t oc8_odb_vm_make_data() {
  oc8_emu_gen_debug_bin_file();
  oc8_bin_file_t *bf = &g_oc8_emu_bin_file;
  api_data_t *ad = malloc(sizeof(api_data_t));
  ad->sorted_defs = malloc(bf->syms_defs_size * sizeof(uint16_t));
  for (size_t i = 0; i < bf->syms_defs_size; ++i)
    ad->sorted_defs[i] = i;

  qsort_r(ad->sorted_defs, bf->syms_defs_size, sizeof(uint16_t), cmp_fn, bf);

  oc8_bin_printer_init(&ad->printer, bf);

  return (odb_vm_api_data_t)ad;
}

odb_vm_api_vtable_t g_oc8_odb_vm_api_table = {
    .get_vm_infos = get_vm_infos,
    .get_update_infos = get_update_infos,
    .get_reg_infos = get_reg_infos,
    .get_reg_val = get_reg_val,
    .set_reg = set_reg,
    .find_reg_id = find_reg_id,
    .read_mem = read_mem,
    .write_mem = write_mem,
    .get_symbols = get_symbols,
    .get_symb_infos = get_symb_infos,
    .find_sym_id = find_sym_id,
    .get_code_text = get_code_text,
    .cleanup = cleanup,
};
