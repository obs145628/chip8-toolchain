#include "oc8_as/sfile.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "oc8_is/ins.h"

#define BASE_ITEMS_LENGTH 256
#define POS_UNDEF ((uint16_t)-1)
#define OPCODE_SIZE (2)

static void add_item(oc8_as_sfile_t *as, oc8_as_data_item_t item) {
  item.pos = as->curr_addr;
  if (as->items_size == as->items_cap) {
    as->items_cap *= 2;
    as->items_arr =
        realloc(as->items_arr, as->items_cap * sizeof(oc8_as_data_item_t));
  }

  as->items_arr[as->items_size++] = item;
}

/// Add an instruction at the current position in the file
/// operands must already be filled
/// op_sym is NULL usually, or the sym if one of the operands is a symbol
/// No need to precise which, there is no ins with 2 immediate operands
static void add_ins(oc8_as_sfile_t *as, oc8_is_ins_t *ins, oc8_is_type_t type,
                    const char *op_sym) {
  ins->opcode = 0;
  ins->type = type;
  oc8_is_encode_ins(ins, NULL);

  oc8_as_data_item_t item;
  item.sym_idx = op_sym ? oc8_as_sfile_get_sym_idx(as, op_sym) : 0;
  item.type = OC8_AS_DATA_ITEM_TYPE_INS;
  item.ins_opcode = ins->opcode;
  add_item(as, item);
  as->curr_addr += OPCODE_SIZE;
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
  oc8_as_sfile_get_sym_idx(as, sym);
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
  as->next_sym_idx = 1;

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

/// Make sure the sfile is valid
/// If it's not, the program abort
/// Possible errors are:
/// - empty file
/// - trying to set properties (type, size, globl) of extern symbol
void oc8_as_sfile_check(oc8_as_sfile_t *as) {
  // - empty file
  if (as->curr_addr == 0) {
    fprintf(stderr, "oc8_as_sfile_check: File is empty");
    assert(0); //@TODO panic
  }

  // - trying to set properties (type, size, globl) of extern symbol
  for (size_t i = 0; i < as->syms_defs_size; ++i) {
    oc8_as_sym_def_t *def = &as->syms_defs_arr[i];
    if (def->pos == POS_UNDEF) {
      fprintf(stderr,
              "oc8_as_sfile_check: cannot set property of extern symbol `%s`",
              def->name);
      assert(0); // @TODO panic
    }
  }
}

void oc8_as_sfile_add_sym(oc8_as_sfile_t *as, const char *sym) {
  // Check for label redefinition
  oc8_as_sym_def_t *def = add_sym_def(as, sym);
  if (def->pos != POS_UNDEF) {
    fprintf(stderr,
            "oc8_as_sfile_add_sym: Defining symbol `%s`, but there is already "
            "one with the same name",
            sym);
    assert(0); //@TODO panic
  }

  // Update position
  def->pos = as->curr_addr;
}

uint16_t oc8_as_sfile_get_sym_idx(oc8_as_sfile_t *as, const char *sym) {
  oc8_smap_node_t *node = oc8_smap_find(&as->syms_map, sym);
  if (node)
    return (uint16_t)node->val;

  uint16_t id = as->next_sym_idx++;
  oc8_smap_insert(&as->syms_map, sym, id);
  return id;
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
  add_ins(as, &ins, OC8_IS_TYPE_7XNN, NULL);
}

void oc8_as_sfile_sins_add_imm(oc8_as_sfile_t *as, const char *s_src,
                               uint8_t r_dst) {
  oc8_is_ins_t ins;
  ins.operands[0] = r_dst;
  ins.operands[1] = 0;
  add_ins(as, &ins, OC8_IS_TYPE_7XNN, s_src);
}

void oc8_as_sfile_ins_add(oc8_as_sfile_t *as, uint8_t r_src, uint8_t r_dst) {
  oc8_is_ins_t ins;
  ins.operands[0] = r_dst;
  ins.operands[1] = r_src;
  add_ins(as, &ins, OC8_IS_TYPE_8XY4, NULL);
}

void oc8_as_sfile_ins_add_i(oc8_as_sfile_t *as, uint8_t r_src) {
  oc8_is_ins_t ins;
  ins.operands[0] = r_src;
  add_ins(as, &ins, OC8_IS_TYPE_FX1E, NULL);
}

void oc8_as_sfile_ins_and(oc8_as_sfile_t *as, uint8_t r_src, uint8_t r_dst) {
  oc8_is_ins_t ins;
  ins.operands[0] = r_dst;
  ins.operands[1] = r_src;
  add_ins(as, &ins, OC8_IS_TYPE_8XY2, NULL);
}

void oc8_as_sfile_ins_bcd(oc8_as_sfile_t *as, uint8_t r_src) {
  oc8_is_ins_t ins;
  ins.operands[0] = r_src;
  add_ins(as, &ins, OC8_IS_TYPE_FX33, NULL);
}

void oc8_as_sfile_ins_call(oc8_as_sfile_t *as, uint16_t i_addr) {
  oc8_is_ins_t ins;
  ins.operands[0] = i_addr;
  add_ins(as, &ins, OC8_IS_TYPE_2NNN, NULL);
}

void oc8_as_sfile_sins_call(oc8_as_sfile_t *as, const char *s_addr) {
  oc8_is_ins_t ins;
  ins.operands[0] = 0;
  add_ins(as, &ins, OC8_IS_TYPE_2NNN, s_addr);
}

void oc8_as_sfile_ins_cls(oc8_as_sfile_t *as) {
  oc8_is_ins_t ins;
  add_ins(as, &ins, OC8_IS_TYPE_00E0, NULL);
}

void oc8_as_sfile_ins_draw(oc8_as_sfile_t *as, uint8_t r_x, uint8_t r_y,
                           uint8_t i_h) {
  oc8_is_ins_t ins;
  ins.operands[0] = r_x;
  ins.operands[1] = r_y;
  ins.operands[2] = i_h;
  add_ins(as, &ins, OC8_IS_TYPE_DXYN, NULL);
}

void oc8_as_sfile_sins_draw(oc8_as_sfile_t *as, uint8_t r_x, uint8_t r_y,
                            const char *s_h) {
  oc8_is_ins_t ins;
  ins.operands[0] = r_x;
  ins.operands[1] = r_y;
  ins.operands[2] = 0;
  add_ins(as, &ins, OC8_IS_TYPE_DXYN, s_h);
}

void oc8_as_sfile_ins_fspr(oc8_as_sfile_t *as, uint8_t r_src) {
  oc8_is_ins_t ins;
  ins.operands[0] = r_src;
  add_ins(as, &ins, OC8_IS_TYPE_FX29, NULL);
}

void oc8_as_sfile_ins_jmp(oc8_as_sfile_t *as, uint16_t i_addr) {
  oc8_is_ins_t ins;
  ins.operands[0] = i_addr;
  add_ins(as, &ins, OC8_IS_TYPE_1NNN, NULL);
}

void oc8_as_sfile_sins_jmp(oc8_as_sfile_t *as, const char *s_addr) {
  oc8_is_ins_t ins;
  ins.operands[0] = 0;
  add_ins(as, &ins, OC8_IS_TYPE_1NNN, s_addr);
}

void oc8_as_sfile_ins_jmp_v0(oc8_as_sfile_t *as, uint16_t i_addr) {
  oc8_is_ins_t ins;
  ins.operands[0] = i_addr;
  add_ins(as, &ins, OC8_IS_TYPE_BNNN, NULL);
}

void oc8_as_sfile_sins_jmp_v0(oc8_as_sfile_t *as, const char *s_addr) {
  oc8_is_ins_t ins;
  ins.operands[0] = 0;
  add_ins(as, &ins, OC8_IS_TYPE_BNNN, s_addr);
}

void oc8_as_sfile_ins_mov_imm(oc8_as_sfile_t *as, uint8_t i_src,
                              uint8_t r_dst) {
  oc8_is_ins_t ins;
  ins.operands[0] = r_dst;
  ins.operands[1] = i_src;
  add_ins(as, &ins, OC8_IS_TYPE_6XNN, NULL);
}

void oc8_as_sfile_sins_mov_imm(oc8_as_sfile_t *as, const char *s_src,
                               uint8_t r_dst) {
  oc8_is_ins_t ins;
  ins.operands[0] = r_dst;
  ins.operands[1] = 0;
  add_ins(as, &ins, OC8_IS_TYPE_6XNN, s_src);
}

void oc8_as_sfile_ins_mov(oc8_as_sfile_t *as, uint8_t r_src, uint8_t r_dst) {
  oc8_is_ins_t ins;
  ins.operands[0] = r_dst;
  ins.operands[1] = r_src;
  add_ins(as, &ins, OC8_IS_TYPE_8XY0, NULL);
}

void oc8_as_sfile_ins_mov_i(oc8_as_sfile_t *as, uint16_t i_addr) {
  oc8_is_ins_t ins;
  ins.operands[0] = i_addr;
  add_ins(as, &ins, OC8_IS_TYPE_ANNN, NULL);
}

void oc8_as_sfile_sins_mov_i(oc8_as_sfile_t *as, const char *s_addr) {
  oc8_is_ins_t ins;
  ins.operands[0] = 0;
  add_ins(as, &ins, OC8_IS_TYPE_ANNN, s_addr);
}

void oc8_as_sfile_ins_mov_fdt(oc8_as_sfile_t *as, uint8_t r_dst) {
  oc8_is_ins_t ins;
  ins.operands[0] = r_dst;
  add_ins(as, &ins, OC8_IS_TYPE_FX07, NULL);
}

void oc8_as_sfile_ins_mov_dt(oc8_as_sfile_t *as, uint8_t r_src) {
  oc8_is_ins_t ins;
  ins.operands[0] = r_src;
  add_ins(as, &ins, OC8_IS_TYPE_FX15, NULL);
}

void oc8_as_sfile_ins_mov_st(oc8_as_sfile_t *as, uint8_t r_src) {
  oc8_is_ins_t ins;
  ins.operands[0] = r_src;
  add_ins(as, &ins, OC8_IS_TYPE_FX18, NULL);
}

void oc8_as_sfile_ins_movm_st(oc8_as_sfile_t *as, uint8_t r_src) {
  oc8_is_ins_t ins;
  ins.operands[0] = r_src;
  add_ins(as, &ins, OC8_IS_TYPE_FX55, NULL);
}

void oc8_as_sfile_ins_movm_ld(oc8_as_sfile_t *as, uint8_t r_dst) {
  oc8_is_ins_t ins;
  ins.operands[0] = r_dst;
  add_ins(as, &ins, OC8_IS_TYPE_FX65, NULL);
}

void oc8_as_sfile_ins_or(oc8_as_sfile_t *as, uint8_t r_src, uint8_t r_dst) {
  oc8_is_ins_t ins;
  ins.operands[0] = r_dst;
  ins.operands[1] = r_src;
  add_ins(as, &ins, OC8_IS_TYPE_8XY1, NULL);
}

void oc8_as_sfile_ins_rand(oc8_as_sfile_t *as, uint8_t i_mask, uint8_t r_dst) {
  oc8_is_ins_t ins;
  ins.operands[0] = r_dst;
  ins.operands[1] = i_mask;
  add_ins(as, &ins, OC8_IS_TYPE_CXNN, NULL);
}

void oc8_as_sfile_sins_rand(oc8_as_sfile_t *as, const char *s_mask,
                            uint8_t r_dst) {
  oc8_is_ins_t ins;
  ins.operands[0] = r_dst;
  ins.operands[1] = 0;
  add_ins(as, &ins, OC8_IS_TYPE_CXNN, s_mask);
}

void oc8_as_sfile_ins_ret(oc8_as_sfile_t *as) {
  oc8_is_ins_t ins;
  add_ins(as, &ins, OC8_IS_TYPE_00EE, NULL);
}

void oc8_as_sfile_ins_shl(oc8_as_sfile_t *as, uint8_t r_src, uint8_t r_dst) {
  oc8_is_ins_t ins;
  ins.operands[0] = r_dst;
  ins.operands[1] = r_src;
  add_ins(as, &ins, OC8_IS_TYPE_8XYE, NULL);
}

void oc8_as_sfile_ins_shr(oc8_as_sfile_t *as, uint8_t r_src, uint8_t r_dst) {
  oc8_is_ins_t ins;
  ins.operands[0] = r_dst;
  ins.operands[1] = r_src;
  add_ins(as, &ins, OC8_IS_TYPE_8XY6, NULL);
}

void oc8_as_sfile_ins_skpe_imm(oc8_as_sfile_t *as, uint8_t i_y, uint8_t r_x) {
  oc8_is_ins_t ins;
  ins.operands[0] = r_x;
  ins.operands[1] = i_y;
  add_ins(as, &ins, OC8_IS_TYPE_3XNN, NULL);
}

void oc8_as_sfile_sins_skpe_imm(oc8_as_sfile_t *as, const char *s_y,
                                uint8_t r_x) {
  oc8_is_ins_t ins;
  ins.operands[0] = r_x;
  ins.operands[1] = 0;
  add_ins(as, &ins, OC8_IS_TYPE_3XNN, s_y);
}

void oc8_as_sfile_ins_skpe(oc8_as_sfile_t *as, uint8_t r_y, uint8_t r_x) {
  oc8_is_ins_t ins;
  ins.operands[0] = r_x;
  ins.operands[1] = r_y;
  add_ins(as, &ins, OC8_IS_TYPE_5XY0, NULL);
}

void oc8_as_sfile_ins_skpn_imm(oc8_as_sfile_t *as, uint8_t i_y, uint8_t r_x) {
  oc8_is_ins_t ins;
  ins.operands[0] = r_x;
  ins.operands[1] = i_y;
  add_ins(as, &ins, OC8_IS_TYPE_4XNN, NULL);
}

void oc8_as_sfile_sins_skpn_imm(oc8_as_sfile_t *as, const char *s_y,
                                uint8_t r_x) {
  oc8_is_ins_t ins;
  ins.operands[0] = r_x;
  ins.operands[1] = 0;
  add_ins(as, &ins, OC8_IS_TYPE_4XNN, s_y);
}

void oc8_as_sfile_ins_skpn(oc8_as_sfile_t *as, uint8_t r_y, uint8_t r_x) {
  oc8_is_ins_t ins;
  ins.operands[0] = r_x;
  ins.operands[1] = r_y;
  add_ins(as, &ins, OC8_IS_TYPE_9XY0, NULL);
}

void oc8_as_sfile_ins_skpkp(oc8_as_sfile_t *as, uint8_t r_src) {
  oc8_is_ins_t ins;
  ins.operands[0] = r_src;
  add_ins(as, &ins, OC8_IS_TYPE_EX9E, NULL);
}

void oc8_as_sfile_ins_skpkn(oc8_as_sfile_t *as, uint8_t r_src) {
  oc8_is_ins_t ins;
  ins.operands[0] = r_src;
  add_ins(as, &ins, OC8_IS_TYPE_EXA1, NULL);
}

void oc8_as_sfile_ins_sub(oc8_as_sfile_t *as, uint8_t r_src, uint8_t r_dst) {
  oc8_is_ins_t ins;
  ins.operands[0] = r_dst;
  ins.operands[1] = r_src;
  add_ins(as, &ins, OC8_IS_TYPE_8XY5, NULL);
}

void oc8_as_sfile_ins_subn(oc8_as_sfile_t *as, uint8_t r_src, uint8_t r_dst) {
  oc8_is_ins_t ins;
  ins.operands[0] = r_dst;
  ins.operands[1] = r_src;
  add_ins(as, &ins, OC8_IS_TYPE_8XY7, NULL);
}

void oc8_as_sfile_ins_sys(oc8_as_sfile_t *as, uint16_t i_addr) {
  oc8_is_ins_t ins;
  ins.operands[0] = i_addr;
  add_ins(as, &ins, OC8_IS_TYPE_0NNN, NULL);
}

void oc8_as_sfile_sins_sys(oc8_as_sfile_t *as, const char *s_addr) {
  oc8_is_ins_t ins;
  ins.operands[0] = 0;
  add_ins(as, &ins, OC8_IS_TYPE_0NNN, s_addr);
}

void oc8_as_sfile_ins_waitk(oc8_as_sfile_t *as, uint8_t r_dst) {
  oc8_is_ins_t ins;
  ins.operands[0] = r_dst;
  add_ins(as, &ins, OC8_IS_TYPE_FX0A, NULL);
}

void oc8_as_sfile_ins_xor(oc8_as_sfile_t *as, uint8_t r_src, uint8_t r_dst) {
  oc8_is_ins_t ins;
  ins.operands[0] = r_dst;
  ins.operands[1] = r_src;
  add_ins(as, &ins, OC8_IS_TYPE_8XY3, NULL);
}

void oc8_as_sfile_dir_align(oc8_as_sfile_t *as, uint16_t nbytes) {
  oc8_as_data_item_t item;
  item.sym_idx = 0;
  item.type = OC8_AS_DATA_ITEM_TYPE_ALIGN;
  item.align_nbytes = nbytes;
  add_item(as, item);

  uint16_t addr = as->curr_addr;
  uint16_t rem = addr % nbytes;
  if (!rem)
    return;
  addr += nbytes - rem;
  as->curr_addr = addr;
}

void oc8_as_sfile_dir_byte(oc8_as_sfile_t *as, uint8_t val) {
  oc8_as_data_item_t item;
  item.sym_idx = 0;
  item.type = OC8_AS_DATA_ITEM_TYPE_BYTE;
  item.byte_val = val;
  add_item(as, item);
  as->curr_addr += 1;
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
  item.sym_idx = 0;
  item.type = OC8_AS_DATA_ITEM_TYPE_WORD;
  item.word_val = val;
  add_item(as, item);
  as->curr_addr += 2;
}

void oc8_as_sfile_dir_zero(oc8_as_sfile_t *as, uint16_t n) {
  oc8_as_data_item_t item;
  item.sym_idx = 0;
  item.type = OC8_AS_DATA_ITEM_TYPE_ZERO;
  item.zero_n = n;
  add_item(as, item);
  as->curr_addr += n;
}
