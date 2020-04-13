#include "oc8_bin/printer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "oc8_is/ins.h"

#define SYM_NONE ((uint16_t)-1)
#define SYM_MULT ((uint16_t)-1)

#define OUT_BUF_SIZE 32

// Fill infos table, trying to guess type
static void init_infos(oc8_bin_printer_t *p) {
  oc8_bin_file_t *bf = p->bf;
  size_t rom_size = bf->rom_size;

  // Go through all syms_ref to fill
  for (size_t i = 0; i < bf->syms_refs_size; ++i) {
    oc8_bin_sym_ref_t *ref = &bf->syms_refs[i];
    oc8_bin_printer_infos_t *infos = &p->infos[ref->ins_addr - OC8_ROM_START];
    infos->sym_ref = ref->sym_id;
    infos->ty = OC8_BIN_PRINTER_DATA_OPCODE;
  }

  // Go through all syms_defs to fill infos and try to infer data type
  for (size_t i = 0; i < bf->syms_defs_size; ++i) {
    oc8_bin_sym_def_t *def = &bf->syms_defs[i];
    if (!def->addr)
      continue;

    oc8_bin_printer_infos_t *infos = &p->infos[def->addr - OC8_ROM_START];
    oc8_bin_printer_data_t ty = OC8_BIN_PRINTER_DATA_OPCODE;
    if (def->type == OC8_BIN_SYM_TYPE_FUN)
      ty = OC8_BIN_PRINTER_DATA_OPCODE;
    else if (def->type == OC8_BIN_SYM_TYPE_OBJ)
      ty = OC8_BIN_PRINTER_DATA_WORD;
    else if (infos->ty != OC8_BIN_PRINTER_DATA_UNKOWN)
      ty = infos->ty;

    infos->sym_def = def->id;
    infos->ty = ty;
  }

  // Go through all even address, and fill type with previously seen type
  oc8_bin_printer_data_t ty = OC8_BIN_PRINTER_DATA_OPCODE;
  for (uint16_t addr = 0; addr < rom_size; addr += 2) {
    oc8_bin_printer_infos_t *infos = &p->infos[addr];
    if (infos->ty == OC8_BIN_PRINTER_DATA_UNKOWN)
      infos->ty = ty;
    else
      ty = infos->ty;
  }

  // Go through all odd addresses, and fill type with byte if not filled
  // before
  for (uint16_t addr = 1; addr < rom_size; addr += 2) {
    oc8_bin_printer_infos_t *infos = &p->infos[addr];
    if (infos->ty == OC8_BIN_PRINTER_DATA_UNKOWN)
      infos->ty = OC8_BIN_PRINTER_DATA_BYTE;
  }
}

static void write_str(oc8_bin_printer_t *p, const char *str) {
  size_t len = strlen(str);
  if (p->out_buf_size + len > p->out_buf_cap) {
    p->out_buf_cap *= 2;
    p->out_buf = realloc(p->out_buf, p->out_buf_cap);
  }

  memcpy(p->out_buf + p->out_buf_size - 1, str, len);
  p->out_buf_size += len;
  p->out_buf[p->out_buf_size - 1] = '\0';
}

static void dump_byte(oc8_bin_printer_t *p, uint16_t addr) {
  char dir_str[16];
  uint8_t *ptr = (uint8_t *)&p->bf->rom[addr - OC8_ROM_START];
  sprintf(dir_str, ".byte 0x%02X", (unsigned)*ptr);
  write_str(p, dir_str);
}

static void dump_word(oc8_bin_printer_t *p, uint16_t addr) {
  char dir_str[16];
  uint16_t *ptr = (uint16_t *)&p->bf->rom[addr - OC8_ROM_START];
  sprintf(dir_str, ".word 0x%04X", (unsigned)*ptr);
  write_str(p, dir_str);
}

static void dump_ins(oc8_bin_printer_t *p, oc8_is_ins_t *ins, const char *sym) {
  char buf[OC8_MAX_SYM_SIZE + 32];

  switch (ins->type) {
  case OC8_IS_TYPE_7XNN:
    if (sym)
      sprintf(buf, "add <%s>, %%v%x", sym, (unsigned)ins->operands[0]);
    else
      sprintf(buf, "add 0x%X, %%v%x", (unsigned)ins->operands[1],
              (unsigned)ins->operands[0]);
    break;

  case OC8_IS_TYPE_8XY4:
    sprintf(buf, "add %%v%x, %%v%x", (unsigned)ins->operands[1],
            (unsigned)ins->operands[0]);
    break;

  case OC8_IS_TYPE_FX1E:
    sprintf(buf, "add %%v%x, %%i", (unsigned)ins->operands[0]);
    break;

  case OC8_IS_TYPE_8XY2:
    sprintf(buf, "and %%v%x, %%v%x", (unsigned)ins->operands[1],
            (unsigned)ins->operands[0]);
    break;

  case OC8_IS_TYPE_FX33:
    sprintf(buf, "bcd %%v%x", (unsigned)ins->operands[0]);
    break;

  case OC8_IS_TYPE_2NNN:
    if (sym)
      sprintf(buf, "call <%s>", sym);
    else
      sprintf(buf, "call 0x%X", (unsigned)ins->operands[0]);
    break;

  case OC8_IS_TYPE_00E0:
    sprintf(buf, "cls");
    break;

  case OC8_IS_TYPE_DXYN:
    if (sym)
      sprintf(buf, "draw %%v%x, %%v%x, <%s>", (unsigned)ins->operands[0],
              (unsigned)ins->operands[1], sym);
    else
      sprintf(buf, "draw %%v%x, %%v%x, 0x%X", (unsigned)ins->operands[0],
              (unsigned)ins->operands[1], (unsigned)ins->operands[2]);
    break;

  case OC8_IS_TYPE_FX29:
    sprintf(buf, "fspr %%v%x", (unsigned)ins->operands[0]);
    break;

  case OC8_IS_TYPE_1NNN:
    if (sym)
      sprintf(buf, "jmp <%s>", sym);
    else
      sprintf(buf, "jmp 0x%X", (unsigned)ins->operands[0]);
    break;

  case OC8_IS_TYPE_BNNN:
    if (sym)
      sprintf(buf, "jmp <%s>(%%v0)", sym);
    else
      sprintf(buf, "jmp 0x%X(%%v0)", (unsigned)ins->operands[0]);
    break;

  case OC8_IS_TYPE_6XNN:
    if (sym)
      sprintf(buf, "mov <%s>, %%v%x", sym, (unsigned)ins->operands[0]);
    else
      sprintf(buf, "mov 0x%X, %%v%x", (unsigned)ins->operands[1],
              (unsigned)ins->operands[0]);
    break;

  case OC8_IS_TYPE_8XY0:
    sprintf(buf, "mov %%v%x, %%v%x", (unsigned)ins->operands[1],
            (unsigned)ins->operands[0]);
    break;

  case OC8_IS_TYPE_ANNN:
    if (sym)
      sprintf(buf, "mov <%s>, %%i", sym);
    else
      sprintf(buf, "mov 0x%X, %%i", (unsigned)ins->operands[0]);
    break;

  case OC8_IS_TYPE_FX07:
    sprintf(buf, "mov %%dt, %%v%x", (unsigned)ins->operands[0]);
    break;

  case OC8_IS_TYPE_FX15:
    sprintf(buf, "mov %%v%x, %%dt", (unsigned)ins->operands[0]);
    break;

  case OC8_IS_TYPE_FX18:
    sprintf(buf, "mov %%v%x, %%st", (unsigned)ins->operands[0]);
    break;

  case OC8_IS_TYPE_FX55:
    sprintf(buf, "movm %%v%x, %%i", (unsigned)ins->operands[0]);
    break;

  case OC8_IS_TYPE_FX65:
    sprintf(buf, "movm %%i, %%v%x", (unsigned)ins->operands[0]);
    break;

  case OC8_IS_TYPE_8XY1:
    sprintf(buf, "or %%v%x, %%v%x", (unsigned)ins->operands[1],
            (unsigned)ins->operands[0]);
    break;

  case OC8_IS_TYPE_CXNN:
    if (sym)
      sprintf(buf, "rand <%s>, %%v%x", sym, (unsigned)ins->operands[0]);
    else
      sprintf(buf, "rand 0x%X, %%v%x", (unsigned)ins->operands[1],
              (unsigned)ins->operands[0]);
    break;

  case OC8_IS_TYPE_00EE:
    sprintf(buf, "ret");
    break;

  case OC8_IS_TYPE_8XYE:
    sprintf(buf, "shl %%v%x, %%v%x", (unsigned)ins->operands[1],
            (unsigned)ins->operands[0]);
    break;

  case OC8_IS_TYPE_8XY6:
    sprintf(buf, "shr %%v%x, %%v%x", (unsigned)ins->operands[1],
            (unsigned)ins->operands[0]);
    break;

  case OC8_IS_TYPE_3XNN:
    if (sym)
      sprintf(buf, "skpe <%s>, %%v%x", sym, (unsigned)ins->operands[0]);
    else
      sprintf(buf, "skpe 0x%X, %%v%x", (unsigned)ins->operands[1],
              (unsigned)ins->operands[0]);
    break;

  case OC8_IS_TYPE_5XY0:
    sprintf(buf, "skpe %%v%x, %%v%x", (unsigned)ins->operands[1],
            (unsigned)ins->operands[0]);
    break;

  case OC8_IS_TYPE_4XNN:
    if (sym)
      sprintf(buf, "skpn <%s>, %%v%x", sym, (unsigned)ins->operands[0]);
    else
      sprintf(buf, "skpn 0x%X, %%v%x", (unsigned)ins->operands[1],
              (unsigned)ins->operands[0]);
    break;

  case OC8_IS_TYPE_9XY0:
    sprintf(buf, "skpn %%v%x, %%v%x", (unsigned)ins->operands[1],
            (unsigned)ins->operands[0]);
    break;

  case OC8_IS_TYPE_EX9E:
    sprintf(buf, "skpkp %%v%x", (unsigned)ins->operands[0]);
    break;

  case OC8_IS_TYPE_EXA1:
    sprintf(buf, "skpkn %%v%x", (unsigned)ins->operands[0]);
    break;

  case OC8_IS_TYPE_8XY5:
    sprintf(buf, "sub %%v%x, %%v%x", (unsigned)ins->operands[1],
            (unsigned)ins->operands[0]);
    break;

  case OC8_IS_TYPE_8XY7:
    sprintf(buf, "subn %%v%x, %%v%x", (unsigned)ins->operands[1],
            (unsigned)ins->operands[0]);
    break;

  case OC8_IS_TYPE_0NNN:
    if (sym)
      sprintf(buf, "sys <%s>", sym);
    else
      sprintf(buf, "sys 0x%X", (unsigned)ins->operands[0]);
    break;

  case OC8_IS_TYPE_FX0A:
    sprintf(buf, "waitk %%v%x", (unsigned)ins->operands[0]);
    break;

  case OC8_IS_TYPE_8XY3:
    sprintf(buf, "xor %%v%x, %%v%x", (unsigned)ins->operands[1],
            (unsigned)ins->operands[0]);
    break;

  default:
    PANIC();
  }

  write_str(p, buf);
}

void oc8_bin_printer_init(oc8_bin_printer_t *p, oc8_bin_file_t *bf) {
  // Initialization
  size_t rom_size = bf->rom_size;
  p->bf = bf;
  p->out_buf = malloc(OUT_BUF_SIZE);
  p->out_buf[0] = '\0';
  p->out_buf_size = 1;
  p->out_buf_cap = OUT_BUF_SIZE;
  p->infos = malloc(rom_size * sizeof(oc8_bin_printer_infos_t));
  for (size_t i = 0; i < rom_size; ++i) {
    p->infos[i].sym_def = SYM_NONE;
    p->infos[i].sym_ref = SYM_NONE;
    p->infos[i].ty = OC8_BIN_PRINTER_DATA_UNKOWN;
  }
  init_infos(p);
}

void oc8_bin_printer_free(oc8_bin_printer_t *p) {
  free(p->out_buf);
  free(p->infos);
}

int oc8_bin_printer_print_at(oc8_bin_printer_t *p, uint16_t addr,
                             int print_sym_defs, int print_sym_refs,
                             int print_sym_ids, int print_opcode,
                             oc8_bin_printer_data_t data_hint, size_t *buf_size,
                             uint16_t *inc) {
  oc8_bin_file_t *bf = p->bf;
  size_t addr_idx = addr - OC8_ROM_START;
  assert(addr_idx < bf->rom_size);
  oc8_bin_printer_infos_t *infos = &p->infos[addr_idx];

  oc8_bin_sym_def_t *def = (print_sym_defs && infos->sym_def != SYM_NONE)
                               ? &bf->syms_defs[infos->sym_def]
                               : NULL;
  oc8_bin_sym_def_t *ref = (print_sym_refs && infos->sym_ref != SYM_NONE)
                               ? &bf->syms_defs[infos->sym_ref]
                               : NULL;

  oc8_bin_printer_data_t ty = infos->ty;
  if (data_hint != OC8_BIN_PRINTER_DATA_UNKOWN)
    ty = data_hint;
  p->out_buf[0] = '\0';
  p->out_buf_size = 1;

  if (def) {
    write_str(p, "<");
    write_str(p, def->name);
    write_str(p, ">:\n");
  }

  const char *sym = ref ? ref->name : NULL;
  oc8_is_ins_t ins;
  size_t rem_bytes = bf->rom_size - addr_idx;
  const uint8_t *code_ptr = &p->bf->rom[addr_idx];
  if (rem_bytes < 2)
    ty = OC8_BIN_PRINTER_DATA_BYTE;

  if (ty == OC8_BIN_PRINTER_DATA_OPCODE) {
    const char *op_ptr = (const char *)code_ptr;
    if (oc8_is_decode_ins(&ins, op_ptr) != 0)
      ty = OC8_BIN_PRINTER_DATA_WORD;
  }

  if (ty == OC8_BIN_PRINTER_DATA_OPCODE) {
    if (print_opcode) {
      char op_buf[32];
      sprintf(op_buf, "      %04X        ", (unsigned)*((uint16_t *)code_ptr));
      write_str(p, op_buf);
    }

    char sym_id_buff[7];
    if (sym && print_sym_ids) {
      sprintf(sym_id_buff, "{%d}", (int)ref->id);
      sym = sym_id_buff;
    }

    dump_ins(p, &ins, sym);
    *inc = 2;
  }

  else if (ty == OC8_BIN_PRINTER_DATA_BYTE) {
    if (print_opcode) {
      char op_buf[32];
      sprintf(op_buf, "      %02X          ", (unsigned)*((uint8_t *)code_ptr));
      write_str(p, op_buf);
    }
    dump_byte(p, addr);
    *inc = 1;
  }

  else if (ty == OC8_BIN_PRINTER_DATA_WORD) {
    if (print_opcode) {
      char op_buf[32];
      sprintf(op_buf, "      %04X        ", (unsigned)*((uint16_t *)code_ptr));
      write_str(p, op_buf);
    }
    dump_word(p, addr);
    *inc = 2;
  }

  else {
    PANIC();
  }

  *buf_size = p->out_buf_size - 1;
  return 0;
}
