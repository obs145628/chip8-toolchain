#include "oc8_as/sfile.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "oc8_is/ins.h"

#define BASE_ITEMS_LENGTH 256
#define POS_UNDEF ((uint16_t)-1)

static void add_item(oc8_as_sfile_t *as, oc8_as_data_item_t item) {
  if (as->items_size == as->items_cap) {
    as->items_cap *= 2;
    as->items_arr =
        realloc(as->items_arr, as->items_cap * sizeof(oc8_as_data_item_t));
  }

  as->items_arr[as->items_size++] = item;
}

static void add_ins(oc8_as_sfile_t *as, oc8_is_ins_t *ins, oc8_is_type_t type) {
  ins->opcode = 0;
  ins->type = type;
  oc8_is_encode_ins(ins, NULL);

  oc8_as_data_item_t item;
  item.type = OC8_AS_DATA_ITEM_TYPE_INS;
  item.ins_opcode = ins->opcode;
  add_item(as, item);
}

static oc8_as_sym_def_t *add_sym_def(oc8_as_sfile_t *as, const char *sym) {
  // Check if already exists
  oc8_smap_node_t *node = oc8_smap_find(&as->syms_defs_map, sym);
  if (node)
    return &as->syms_defs_arr[node->val];

  // Insert in syms_defs_map
  size_t idx = as->syms_defs_size++;
  oc8_smap_insert(&as->syms_defs_map, sym, idx);
  node = oc8_smap_find(&as->syms_defs_map, sym);
  assert(node);

  // Insert in syms_defs_list
  oc8_as_sym_def_t *def = &as->syms_defs_arr[idx];
  def->name = node->key;
  def->pos = POS_UNDEF;
  def->size = 0;
  def->type = OC8_AS_DATA_SYM_TYPE_NO;
  def->is_global = 0;

  // generate entry in syms_map if none already
  oc8_as_sfile_get_sym_addr(as, sym);
  return def;
}

oc8_as_sfile_t *oc8_as_sfile_new() {
  oc8_as_sfile_t *as = (oc8_as_sfile_t *)malloc(sizeof(oc8_as_sfile_t));
  as->items_arr = malloc(BASE_ITEMS_LENGTH * sizeof(oc8_as_data_item_t));
  as->items_size = 0;
  as->items_cap = BASE_ITEMS_LENGTH;
  as->curr_addr = 0;

  as->syms_defs_size = 0;
  oc8_smap_init(&as->syms_defs_map);

  oc8_smap_init(&as->syms_map);
  as->next_sym_addr = 1;

  oc8_smap_init(&as->equ_map);
  return as;
}

void oc8_as_sfile_free(oc8_as_sfile_t *as) {
  free(as->items_arr);
  oc8_smap_free(&as->syms_defs_map);
  oc8_smap_free(&as->syms_map);
  oc8_smap_free(&as->equ_map);
  free(as);
}

void oc8_as_sfile_add_sym(oc8_as_sfile_t *as, const char *sym) {
  // Check for label redefinition
  oc8_as_sym_def_t *def = add_sym_def(as, sym);
  if (def->pos != POS_UNDEF) {
    fprintf(stderr,
            "oc8_as_sfile_add_sym: Defining symbol `%s`, but there is already "
            "one with the same name",
            sym);
    exit(1);
  }

  // Update position
  def->pos = as->curr_addr;
}

uint16_t oc8_as_sfile_get_sym_addr(oc8_as_sfile_t *as, const char *sym) {
  oc8_smap_node_t *node = oc8_smap_find(&as->syms_map, sym);
  if (node)
    return (uint16_t)node->val;

  uint16_t addr = as->next_sym_addr++;
  oc8_smap_insert(&as->syms_map, sym, addr);
  return addr;
}

size_t *oc8_as_sfile_get_equ(oc8_as_sfile_t *as, const char *key) {
  oc8_smap_node_t *node = oc8_smap_find(&as->equ_map, key);
  return node ? &(node->val) : NULL;
}

void oc8_as_sfile_ins_add_imm(oc8_as_sfile_t *as, uint8_t i_src,
                              uint8_t r_dst) {
  oc8_is_ins_t ins;
  ins.operands[0] = r_dst;
  ins.operands[1] = i_src;
  add_ins(as, &ins, OC8_IS_TYPE_7XNN);
}

void oc8_as_sfile_ins_add(oc8_as_sfile_t *as, uint8_t r_src, uint8_t r_dst) {
  oc8_is_ins_t ins;
  ins.operands[0] = r_dst;
  ins.operands[1] = r_src;
  add_ins(as, &ins, OC8_IS_TYPE_8XY4);
}

void oc8_as_sfile_ins_add_i(oc8_as_sfile_t *as, uint8_t r_src) {
  oc8_is_ins_t ins;
  ins.operands[0] = r_src;
  add_ins(as, &ins, OC8_IS_TYPE_FX1E);
}

void oc8_as_sfile_ins_and(oc8_as_sfile_t *as, uint8_t r_src, uint8_t r_dst) {
  oc8_is_ins_t ins;
  ins.operands[0] = r_dst;
  ins.operands[1] = r_src;
  add_ins(as, &ins, OC8_IS_TYPE_8XY2);
}

void oc8_as_sfile_ins_bcd(oc8_as_sfile_t *as, uint8_t r_src) {
  oc8_is_ins_t ins;
  ins.operands[0] = r_src;
  add_ins(as, &ins, OC8_IS_TYPE_FX33);
}

void oc8_as_sfile_ins_call(oc8_as_sfile_t *as, uint16_t i_addr) {
  oc8_is_ins_t ins;
  ins.operands[0] = i_addr;
  add_ins(as, &ins, OC8_IS_TYPE_2NNN);
}

void oc8_as_sfile_ins_cls(oc8_as_sfile_t *as) {
  oc8_is_ins_t ins;
  add_ins(as, &ins, OC8_IS_TYPE_00E0);
}

void oc8_as_sfile_ins_draw(oc8_as_sfile_t *as, uint8_t r_x, uint8_t r_y,
                           uint8_t i_h) {
  oc8_is_ins_t ins;
  ins.operands[0] = r_x;
  ins.operands[1] = r_y;
  ins.operands[2] = i_h;
  add_ins(as, &ins, OC8_IS_TYPE_DXYN);
}

void oc8_as_sfile_ins_fspr(oc8_as_sfile_t *as, uint8_t r_src) {
  oc8_is_ins_t ins;
  ins.operands[0] = r_src;
  add_ins(as, &ins, OC8_IS_TYPE_FX29);
}

void oc8_as_sfile_ins_jmp(oc8_as_sfile_t *as, uint16_t i_addr) {
  oc8_is_ins_t ins;
  ins.operands[0] = i_addr;
  add_ins(as, &ins, OC8_IS_TYPE_1NNN);
}

void oc8_as_sfile_ins_jmp_v0(oc8_as_sfile_t *as, uint16_t i_addr) {
  oc8_is_ins_t ins;
  ins.operands[0] = i_addr;
  add_ins(as, &ins, OC8_IS_TYPE_BNNN);
}

void oc8_as_sfile_ins_mov_imm(oc8_as_sfile_t *as, uint8_t i_src,
                              uint8_t r_dst) {
  oc8_is_ins_t ins;
  ins.operands[0] = r_dst;
  ins.operands[1] = i_src;
  add_ins(as, &ins, OC8_IS_TYPE_6XNN);
}

void oc8_as_sfile_ins_mov(oc8_as_sfile_t *as, uint8_t r_src, uint8_t r_dst) {
  oc8_is_ins_t ins;
  ins.operands[0] = r_dst;
  ins.operands[1] = r_src;
  add_ins(as, &ins, OC8_IS_TYPE_8XY0);
}

void oc8_as_sfile_ins_mov_i(oc8_as_sfile_t *as, uint16_t i_addr) {
  oc8_is_ins_t ins;
  ins.operands[0] = i_addr;
  add_ins(as, &ins, OC8_IS_TYPE_ANNN);
}

void oc8_as_sfile_ins_mov_fdt(oc8_as_sfile_t *as, uint8_t r_dst) {
  oc8_is_ins_t ins;
  ins.operands[0] = r_dst;
  add_ins(as, &ins, OC8_IS_TYPE_FX07);
}

void oc8_as_sfile_ins_mov_dt(oc8_as_sfile_t *as, uint8_t r_src) {
  oc8_is_ins_t ins;
  ins.operands[0] = r_src;
  add_ins(as, &ins, OC8_IS_TYPE_FX15);
}

void oc8_as_sfile_ins_mov_st(oc8_as_sfile_t *as, uint8_t r_src) {
  oc8_is_ins_t ins;
  ins.operands[0] = r_src;
  add_ins(as, &ins, OC8_IS_TYPE_FX18);
}

void oc8_as_sfile_ins_movm_st(oc8_as_sfile_t *as, uint8_t r_src) {
  oc8_is_ins_t ins;
  ins.operands[0] = r_src;
  add_ins(as, &ins, OC8_IS_TYPE_FX55);
}

void oc8_as_sfile_ins_movm_ld(oc8_as_sfile_t *as, uint8_t r_dst) {
  oc8_is_ins_t ins;
  ins.operands[0] = r_dst;
  add_ins(as, &ins, OC8_IS_TYPE_FX65);
}

void oc8_as_sfile_ins_or(oc8_as_sfile_t *as, uint8_t r_src, uint8_t r_dst) {
  oc8_is_ins_t ins;
  ins.operands[0] = r_dst;
  ins.operands[1] = r_src;
  add_ins(as, &ins, OC8_IS_TYPE_8XY1);
}

void oc8_as_sfile_ins_rand(oc8_as_sfile_t *as, uint8_t i_mask, uint8_t r_dst) {
  oc8_is_ins_t ins;
  ins.operands[0] = i_mask;
  ins.operands[1] = r_dst;
  add_ins(as, &ins, OC8_IS_TYPE_CXNN);
}

void oc8_as_sfile_ins_ret(oc8_as_sfile_t *as) {
  oc8_is_ins_t ins;
  add_ins(as, &ins, OC8_IS_TYPE_00EE);
}

void oc8_as_sfile_ins_shl(oc8_as_sfile_t *as, uint8_t r_src, uint8_t r_dst) {
  oc8_is_ins_t ins;
  ins.operands[0] = r_dst;
  ins.operands[1] = r_src;
  add_ins(as, &ins, OC8_IS_TYPE_8XYE);
}

void oc8_as_sfile_ins_shr(oc8_as_sfile_t *as, uint8_t r_src, uint8_t r_dst) {
  oc8_is_ins_t ins;
  ins.operands[0] = r_dst;
  ins.operands[1] = r_src;
  add_ins(as, &ins, OC8_IS_TYPE_8XY6);
}

void oc8_as_sfile_ins_skpe_imm(oc8_as_sfile_t *as, uint8_t i_y, uint8_t r_x) {
  oc8_is_ins_t ins;
  ins.operands[0] = r_x;
  ins.operands[1] = i_y;
  add_ins(as, &ins, OC8_IS_TYPE_3XNN);
}

void oc8_as_sfile_ins_skpe(oc8_as_sfile_t *as, uint8_t r_y, uint8_t r_x) {
  oc8_is_ins_t ins;
  ins.operands[0] = r_x;
  ins.operands[1] = r_y;
  add_ins(as, &ins, OC8_IS_TYPE_5XY0);
}

void oc8_as_sfile_ins_skpn_imm(oc8_as_sfile_t *as, uint8_t i_y, uint8_t r_x) {
  oc8_is_ins_t ins;
  ins.operands[0] = r_x;
  ins.operands[1] = i_y;
  add_ins(as, &ins, OC8_IS_TYPE_4XNN);
}

void oc8_as_sfile_ins_skpn(oc8_as_sfile_t *as, uint8_t r_y, uint8_t r_x) {
  oc8_is_ins_t ins;
  ins.operands[0] = r_x;
  ins.operands[1] = r_y;
  add_ins(as, &ins, OC8_IS_TYPE_9XY0);
}

void oc8_as_sfile_ins_skpkp(oc8_as_sfile_t *as, uint8_t r_src) {
  oc8_is_ins_t ins;
  ins.operands[0] = r_src;
  add_ins(as, &ins, OC8_IS_TYPE_EX9E);
}

void oc8_as_sfile_ins_skpkn(oc8_as_sfile_t *as, uint8_t r_src) {
  oc8_is_ins_t ins;
  ins.operands[0] = r_src;
  add_ins(as, &ins, OC8_IS_TYPE_EXA1);
}

void oc8_as_sfile_ins_sub(oc8_as_sfile_t *as, uint8_t r_src, uint8_t r_dst) {
  oc8_is_ins_t ins;
  ins.operands[0] = r_dst;
  ins.operands[1] = r_src;
  add_ins(as, &ins, OC8_IS_TYPE_8XY5);
}

void oc8_as_sfile_ins_subn(oc8_as_sfile_t *as, uint8_t r_src, uint8_t r_dst) {
  oc8_is_ins_t ins;
  ins.operands[0] = r_dst;
  ins.operands[1] = r_src;
  add_ins(as, &ins, OC8_IS_TYPE_8XY7);
}

void oc8_as_sfile_ins_sys(oc8_as_sfile_t *as, uint16_t i_addr) {
  oc8_is_ins_t ins;
  ins.operands[0] = i_addr;
  add_ins(as, &ins, OC8_IS_TYPE_0NNN);
}

void oc8_as_sfile_ins_waitk(oc8_as_sfile_t *as, uint8_t r_dst) {
  oc8_is_ins_t ins;
  ins.operands[0] = r_dst;
  add_ins(as, &ins, OC8_IS_TYPE_FX0A);
}

void oc8_as_sfile_ins_xor(oc8_as_sfile_t *as, uint8_t r_src, uint8_t r_dst) {
  oc8_is_ins_t ins;
  ins.operands[0] = r_dst;
  ins.operands[1] = r_src;
  add_ins(as, &ins, OC8_IS_TYPE_8XY3);
}

void oc8_as_sfile_dir_align(oc8_as_sfile_t *as, uint16_t nbytes) {
  oc8_as_data_item_t item;
  item.type = OC8_AS_DATA_ITEM_TYPE_ALIGN;
  item.align_nbytes = nbytes;
  add_item(as, item);
}

void oc8_as_sfile_dir_byte(oc8_as_sfile_t *as, uint8_t val) {
  oc8_as_data_item_t item;
  item.type = OC8_AS_DATA_ITEM_TYPE_BYTE;
  item.byte_val = val;
  add_item(as, item);
}

void oc8_as_sfile_dir_globl(oc8_as_sfile_t *as, const char *sym) {
  oc8_as_sym_def_t *def = add_sym_def(as, sym);
  def->is_global = 1;
}

void oc8_as_sfile_dir_equ(oc8_as_sfile_t *as, const char *key, uint16_t val) {
  if (oc8_smap_insert(&as->equ_map, key, val) == 0) {
    fprintf(stderr, "oc8_as_file_dir_equ: Redefinition of cast %s\n", key);
    exit(1);
  }
}

void oc8_as_sfile_dir_size(oc8_as_sfile_t *as, const char *sym, uint16_t size) {
  oc8_as_sym_def_t *def = add_sym_def(as, sym);
  def->size = size;
}

void oc8_as_sfile_dir_type(oc8_as_sfile_t *as, const char *sym,
                           oc8_as_sym_type_t type) {
  oc8_as_sym_def_t *def = add_sym_def(as, sym);
  def->type = type;
}

void oc8_as_sfile_dir_word(oc8_as_sfile_t *as, uint16_t val) {
  oc8_as_data_item_t item;
  item.type = OC8_AS_DATA_ITEM_TYPE_WORD;
  item.word_val = val;
  add_item(as, item);
}

void oc8_as_sfile_dir_zero(oc8_as_sfile_t *as, uint16_t n) {
  oc8_as_data_item_t item;
  item.type = OC8_AS_DATA_ITEM_TYPE_ZERO;
  item.zero_n = n;
  add_item(as, item);
}
