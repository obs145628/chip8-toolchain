#define _GNU_SOURCE

#include "oc8_as/printer.h"
#include "oc8_defs/oc8_defs.h"
#include "oc8_is/ins.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SYMS (512)
#define INS_WS "    "

typedef struct {

  // Arguments
  oc8_as_sfile_t *sf;
  char *out_buf;
  size_t out_buf_len;
  void (*buf_cb)(size_t, void *);
  void *cb_arg;

  // Curently writen number of bytes in buffer
  size_t nb_bytes;
  // Total number of bytes writen to out_buf
  size_t total_bytes;

  // Get symbol from index
  const char *inv_sym[MAX_SYMS];

  // Index to symbol defs, sorted based on position
  size_t sorted_defs[OC8_AS_MAX_SYMS];

  // next symbol to be printed
  size_t next_def_idx;

} printer_t;

static int sort_defs_fn(size_t *idx_a, size_t *idx_b, printer_t *printer) {
  oc8_as_sym_def_t *def_a = &printer->sf->syms_defs_arr[*idx_a];
  oc8_as_sym_def_t *def_b = &printer->sf->syms_defs_arr[*idx_b];
  assert(def_a->pos < 0x1000);
  assert(def_b->pos < 0x1000);
  return (int)def_a->pos - (int)def_b->pos;
}

/// Copy `len` bytes from `buf` to the buffer in `printer`
/// Always call this function to write to out_buf
/// Update number of bytes, and call CB if necessary
static void write_bytes(printer_t *printer, const char *buf, size_t len) {
  // count without writing
  if (printer->out_buf == NULL) {
    printer->nb_bytes += len;
    return;
  }
  assert(printer->nb_bytes < printer->out_buf_len);

  // Compute what to write now in buffer, and after
  size_t len_after = 0;
  size_t len_rest = printer->out_buf_len - printer->nb_bytes;
  if (len > len_rest) {
    len_after = len - len_rest;
    len = len_rest;
  }

  // Perform writing and update count
  memcpy(printer->out_buf + printer->nb_bytes, buf, len);
  printer->nb_bytes += len;
  printer->total_bytes += len;

  // Check if buffer full
  if (printer->nb_bytes == printer->out_buf_len && printer->buf_cb) {
    printer->buf_cb(printer->nb_bytes, printer->cb_arg);
    printer->nb_bytes = 0;
  }

  if (len_after) {
    write_bytes(printer, buf + len, len_after);
  }
}

// write a symbol definition (<name> ':') with all it's properties
// @param idx index if syms_defs_arr
static void write_symbol_def(printer_t *printer) {

  char buf[OC8_MAX_SYM_SIZE + 32];

  oc8_as_sym_def_t *def = &printer->sf->syms_defs_arr[printer->next_def_idx];

  if (def->size != 0) { //.size <name>, <size>
    sprintf(buf, ".size %s, %u\n", def->name, (unsigned)def->size);
    write_bytes(printer, buf, strlen(buf));
  }
  if (def->type == OC8_AS_DATA_SYM_TYPE_FUN) { //.type <name>, @function
    sprintf(buf, ".type %s, @function\n", def->name);
    write_bytes(printer, buf, strlen(buf));
  }
  if (def->type == OC8_AS_DATA_SYM_TYPE_OBJ) { //.type <name>, @object
    sprintf(buf, ".type %s, @object\n", def->name);
    write_bytes(printer, buf, strlen(buf));
  }
  if (def->is_global) { //.globl <name>
    sprintf(buf, ".globl %s\n", def->name);
    write_bytes(printer, buf, strlen(buf));
  }

  sprintf(buf, "%s:\n", def->name);
  write_bytes(printer, buf, strlen(buf));
}

static void write_ins(printer_t *printer, oc8_as_data_item_t item) {
  char buf[OC8_MAX_SYM_SIZE + 32];
  oc8_is_ins_t ins;
  uint16_t opcode = item.ins_opcode;
  const char *sym = item.sym_idx ? printer->inv_sym[item.sym_idx] : NULL;
  oc8_is_decode_ins(&ins, (const char *)&opcode);

  switch (ins.type) {
  case OC8_IS_TYPE_7XNN:

    if (sym)
      sprintf(buf, INS_WS "add %s, %%v%x\n", sym, (unsigned)ins.operands[0]);
    else
      sprintf(buf, INS_WS "add 0x%X, %%v%x\n", (unsigned)ins.operands[1],
              (unsigned)ins.operands[0]);

    write_bytes(printer, buf, strlen(buf));
    break;

  case OC8_IS_TYPE_8XY4:
    sprintf(buf, INS_WS "add %%v%x, %%v%x\n", (unsigned)ins.operands[1],
            (unsigned)ins.operands[0]);
    write_bytes(printer, buf, strlen(buf));
    break;

  case OC8_IS_TYPE_FX1E:
    sprintf(buf, INS_WS "add %%v%x, %%i\n", (unsigned)ins.operands[0]);
    write_bytes(printer, buf, strlen(buf));
    break;

  case OC8_IS_TYPE_8XY2:
    sprintf(buf, INS_WS "and %%v%x, %%v%x\n", (unsigned)ins.operands[1],
            (unsigned)ins.operands[0]);
    write_bytes(printer, buf, strlen(buf));
    break;

  case OC8_IS_TYPE_FX33:
    sprintf(buf, INS_WS "bcd %%v%x\n", (unsigned)ins.operands[0]);
    write_bytes(printer, buf, strlen(buf));
    break;

  case OC8_IS_TYPE_2NNN:
    if (sym)
      sprintf(buf, INS_WS "call %s\n", sym);
    else
      sprintf(buf, INS_WS "call 0x%X\n", (unsigned)ins.operands[0]);
    write_bytes(printer, buf, strlen(buf));
    break;

  case OC8_IS_TYPE_00E0:
    sprintf(buf, INS_WS "cls\n");
    write_bytes(printer, buf, strlen(buf));
    break;

  case OC8_IS_TYPE_DXYN:
    if (sym)
      sprintf(buf, INS_WS "draw %%v%x, %%v%x, %s\n", (unsigned)ins.operands[0],
              (unsigned)ins.operands[1], sym);
    else
      sprintf(buf, INS_WS "draw %%v%x, %%v%x, 0x%X\n",
              (unsigned)ins.operands[0], (unsigned)ins.operands[1],
              (unsigned)ins.operands[2]);
    write_bytes(printer, buf, strlen(buf));
    break;

  case OC8_IS_TYPE_FX29:
    sprintf(buf, INS_WS "fspr %%v%x\n", (unsigned)ins.operands[0]);
    write_bytes(printer, buf, strlen(buf));
    break;

  case OC8_IS_TYPE_1NNN:
    if (sym)
      sprintf(buf, INS_WS "jmp %s\n", sym);
    else
      sprintf(buf, INS_WS "jmp 0x%X\n", (unsigned)ins.operands[0]);
    write_bytes(printer, buf, strlen(buf));
    break;

  case OC8_IS_TYPE_BNNN:
    if (sym)
      sprintf(buf, INS_WS "jmp %s(%%v0)\n", sym);
    else
      sprintf(buf, INS_WS "jmp 0x%X(%%v0)\n", (unsigned)ins.operands[0]);
    write_bytes(printer, buf, strlen(buf));
    break;

  case OC8_IS_TYPE_6XNN:
    if (sym)
      sprintf(buf, INS_WS "mov %s, %%v%x\n", sym, (unsigned)ins.operands[0]);
    else
      sprintf(buf, INS_WS "mov 0x%X, %%v%x\n", (unsigned)ins.operands[1],
              (unsigned)ins.operands[0]);
    write_bytes(printer, buf, strlen(buf));
    break;

  case OC8_IS_TYPE_8XY0:
    sprintf(buf, INS_WS "mov %%v%x, %%v%x\n", (unsigned)ins.operands[1],
            (unsigned)ins.operands[0]);
    write_bytes(printer, buf, strlen(buf));
    break;

  case OC8_IS_TYPE_ANNN:
    if (sym)
      sprintf(buf, INS_WS "mov %s, %%i\n", sym);
    else
      sprintf(buf, INS_WS "mov 0x%X, %%i\n", (unsigned)ins.operands[0]);
    write_bytes(printer, buf, strlen(buf));
    break;

  case OC8_IS_TYPE_FX07:
    sprintf(buf, INS_WS "mov %%dt, %%v%x\n", (unsigned)ins.operands[0]);
    write_bytes(printer, buf, strlen(buf));
    break;

  case OC8_IS_TYPE_FX15:
    sprintf(buf, INS_WS "mov %%v%x, %%dt\n", (unsigned)ins.operands[0]);
    write_bytes(printer, buf, strlen(buf));
    break;

  case OC8_IS_TYPE_FX18:
    sprintf(buf, INS_WS "mov %%v%x, %%st\n", (unsigned)ins.operands[0]);
    write_bytes(printer, buf, strlen(buf));
    break;

  case OC8_IS_TYPE_FX55:
    sprintf(buf, INS_WS "movm %%v%x, %%i\n", (unsigned)ins.operands[0]);
    write_bytes(printer, buf, strlen(buf));
    break;

  case OC8_IS_TYPE_FX65:
    sprintf(buf, INS_WS "movm %%i, %%v%x\n", (unsigned)ins.operands[0]);
    write_bytes(printer, buf, strlen(buf));
    break;

  case OC8_IS_TYPE_8XY1:
    sprintf(buf, INS_WS "or %%v%x, %%v%x\n", (unsigned)ins.operands[1],
            (unsigned)ins.operands[0]);
    write_bytes(printer, buf, strlen(buf));
    break;

  case OC8_IS_TYPE_CXNN:
    if (sym)
      sprintf(buf, INS_WS "rand %s, %%v%x\n", sym, (unsigned)ins.operands[0]);
    else
      sprintf(buf, INS_WS "rand 0x%X, %%v%x\n", (unsigned)ins.operands[1],
              (unsigned)ins.operands[0]);
    write_bytes(printer, buf, strlen(buf));
    break;

  case OC8_IS_TYPE_00EE:
    sprintf(buf, INS_WS "ret\n");
    write_bytes(printer, buf, strlen(buf));
    break;

  case OC8_IS_TYPE_8XYE:
    sprintf(buf, INS_WS "shl %%v%x, %%v%x\n", (unsigned)ins.operands[1],
            (unsigned)ins.operands[0]);
    write_bytes(printer, buf, strlen(buf));
    break;

  case OC8_IS_TYPE_8XY6:
    sprintf(buf, INS_WS "shr %%v%x, %%v%x\n", (unsigned)ins.operands[1],
            (unsigned)ins.operands[0]);
    write_bytes(printer, buf, strlen(buf));
    break;

  case OC8_IS_TYPE_3XNN:
    if (sym)
      sprintf(buf, INS_WS "skpe %s, %%v%x\n", sym, (unsigned)ins.operands[0]);
    else
      sprintf(buf, INS_WS "skpe 0x%X, %%v%x\n", (unsigned)ins.operands[1],
              (unsigned)ins.operands[0]);
    write_bytes(printer, buf, strlen(buf));
    break;

  case OC8_IS_TYPE_5XY0:
    sprintf(buf, INS_WS "skpe %%v%x, %%v%x\n", (unsigned)ins.operands[1],
            (unsigned)ins.operands[0]);
    write_bytes(printer, buf, strlen(buf));
    break;

  case OC8_IS_TYPE_4XNN:
    if (sym)
      sprintf(buf, INS_WS "skpn %s, %%v%x\n", sym, (unsigned)ins.operands[0]);
    else
      sprintf(buf, INS_WS "skpn 0x%X, %%v%x\n", (unsigned)ins.operands[1],
              (unsigned)ins.operands[0]);
    write_bytes(printer, buf, strlen(buf));
    break;

  case OC8_IS_TYPE_9XY0:
    sprintf(buf, INS_WS "skpn %%v%x, %%v%x\n", (unsigned)ins.operands[1],
            (unsigned)ins.operands[0]);
    write_bytes(printer, buf, strlen(buf));
    break;

  case OC8_IS_TYPE_EX9E:
    sprintf(buf, INS_WS "skpkp %%v%x\n", (unsigned)ins.operands[0]);
    write_bytes(printer, buf, strlen(buf));
    break;

  case OC8_IS_TYPE_EXA1:
    sprintf(buf, INS_WS "skpkn %%v%x\n", (unsigned)ins.operands[0]);
    write_bytes(printer, buf, strlen(buf));
    break;

  case OC8_IS_TYPE_8XY5:
    sprintf(buf, INS_WS "sub %%v%x, %%v%x\n", (unsigned)ins.operands[1],
            (unsigned)ins.operands[0]);
    write_bytes(printer, buf, strlen(buf));
    break;

  case OC8_IS_TYPE_8XY7:
    sprintf(buf, INS_WS "subn %%v%x, %%v%x\n", (unsigned)ins.operands[1],
            (unsigned)ins.operands[0]);
    write_bytes(printer, buf, strlen(buf));
    break;

  case OC8_IS_TYPE_0NNN:
    if (sym)
      sprintf(buf, INS_WS "sys %s\n", sym);
    else
      sprintf(buf, INS_WS "sys 0x%X\n", (unsigned)ins.operands[0]);
    write_bytes(printer, buf, strlen(buf));
    break;

  case OC8_IS_TYPE_FX0A:
    sprintf(buf, INS_WS "waitk %%v%x\n", (unsigned)ins.operands[0]);
    write_bytes(printer, buf, strlen(buf));
    break;

  case OC8_IS_TYPE_8XY3:
    sprintf(buf, INS_WS "xor %%v%x, %%v%x\n", (unsigned)ins.operands[1],
            (unsigned)ins.operands[0]);
    write_bytes(printer, buf, strlen(buf));
    break;

  default:
    // Unreachable
    assert(0);
  };
}

static void write_item(printer_t *printer, oc8_as_data_item_t item) {

  char buf[32];

  switch (item.type) {
  case OC8_AS_DATA_ITEM_TYPE_INS:
    write_ins(printer, item);
    break;

  case OC8_AS_DATA_ITEM_TYPE_ALIGN: //.align <nbytes>
    sprintf(buf, ".align %u\n", (unsigned)item.align_nbytes);
    write_bytes(printer, buf, strlen(buf));
    break;

  case OC8_AS_DATA_ITEM_TYPE_BYTE: //.byte <val>
    sprintf(buf, ".byte 0x%X\n", (unsigned)item.byte_val);
    write_bytes(printer, buf, strlen(buf));
    break;

  case OC8_AS_DATA_ITEM_TYPE_WORD: //.word <val>
    sprintf(buf, ".word 0x%X\n", (unsigned)item.word_val);
    write_bytes(printer, buf, strlen(buf));
    break;

  case OC8_AS_DATA_ITEM_TYPE_ZERO: //.zero <n>
    sprintf(buf, ".zero %u\n", (unsigned)item.zero_n);
    write_bytes(printer, buf, strlen(buf));
    break;

  default:
    // Unreachable
    assert(0);
  }
}

size_t oc8_as_print_sfile(oc8_as_sfile_t *sf, char *out_buf, size_t out_buf_len,
                          void (*buf_cb)(size_t, void *), void *arg) {

  printer_t printer;
  printer.sf = sf;
  printer.out_buf = out_buf;
  printer.out_buf_len = out_buf_len;
  printer.buf_cb = buf_cb;
  printer.cb_arg = arg;
  printer.nb_bytes = 0;
  printer.total_bytes = 0;

  // Build inversed symbols table
  memset(printer.inv_sym, 0, sizeof(printer.inv_sym));
  oc8_smap_it_t it = oc8_smap_get_it(&sf->syms_map);
  while (oc8_smap_it_get(&it)) {
    oc8_smap_node_t *node = oc8_smap_it_get(&it);
    printer.inv_sym[node->val] = node->key;
    oc8_smap_it_next(&it);
  }

  // Sort definitions
  for (size_t i = 0; i < sf->syms_defs_size; ++i)
    printer.sorted_defs[i] = i;
  printer.next_def_idx = 0;
  qsort_r(printer.sorted_defs, sf->syms_defs_size, sizeof(size_t),
          (int (*)(const void *, const void *, void *))sort_defs_fn, &printer);
  uint16_t next_sym_pos = printer.next_def_idx < sf->syms_defs_size
                              ? sf->syms_defs_arr[printer.next_def_idx].pos
                              : 0xFFFF;

  // Print through all items
  for (size_t i = 0; i < sf->items_size; ++i) {
    oc8_as_data_item_t item = sf->items_arr[i];

    // print optional symbol defs (<name> ':'), maybe + than 1
    assert(next_sym_pos >= item.pos);
    while (item.pos == next_sym_pos) {
      write_symbol_def(&printer);
      ++printer.next_def_idx;
      next_sym_pos = printer.next_def_idx < sf->syms_defs_size
                         ? sf->syms_defs_arr[printer.next_def_idx].pos
                         : 0xFFFF;
    }

    write_item(&printer, item);
  }

  // Finish
  if (printer.nb_bytes && buf_cb) {
    buf_cb(printer.nb_bytes, arg);
  }
  size_t nbytes = printer.total_bytes;

  return nbytes;
}
