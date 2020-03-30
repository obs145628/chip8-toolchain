#ifndef OC8_AS_SFILE_H_
#define OC8_AS_SFILE_H_

//===--oc8_as/sfile.h - SFile struct definition -------------------*- C -*-===//
//
// oc8_as library
// Author: Steven Lariau
//
//===----------------------------------------------------------------------===//
///
/// \file
/// SFile struct definition
///
//===----------------------------------------------------------------------===//

#include <stddef.h>
#include <stdint.h>

#include "oc8_smap/oc8_smap.h"

#define OC8_AS_MAX_SYMS (256)

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  OC8_AS_DATA_ITEM_TYPE_INS,
  OC8_AS_DATA_ITEM_TYPE_ALIGN,
  OC8_AS_DATA_ITEM_TYPE_BYTE,
  OC8_AS_DATA_ITEM_TYPE_WORD,
  OC8_AS_DATA_ITEM_TYPE_ZERO,
} oc8_as_data_item_type_t;

typedef struct {
  oc8_as_data_item_type_t type;
  uint16_t sym_idx; // != 0 if the data use a symbol (eg: call to fun symbol)
  uint16_t pos;
  union {
    uint16_t ins_opcode;
    uint16_t align_nbytes;
    uint8_t byte_val;
    uint16_t word_val;
    uint16_t zero_n;
  };
} oc8_as_data_item_t;

typedef enum {
  OC8_AS_DATA_SYM_TYPE_NO,
  OC8_AS_DATA_SYM_TYPE_FUN,
  OC8_AS_DATA_SYM_TYPE_OBJ,
} oc8_as_sym_type_t;

typedef struct {
  const char *name; // pointer to the str stored in smap
  uint16_t pos;
  uint16_t size;
  oc8_as_sym_type_t type;
  int is_global;
} oc8_as_sym_def_t;

/// This struct is used to create an assembly file
/// To create a file, you make a sequence of calls to add instructions, use
/// directives, and put labels
/// Then it can be exported to text format (.c8s files), or directly assembled
/// to binary format
///
/// The struct contains:
/// - a sequence of data item. Each item is either
///   + an instruction
///   + an align directive (.align <nbytes>)
///   + a byte (.byte <uint>)
///   + a word (.word <uint>)
///   + a sequence of 0s (.zero <n>)
/// - a set of all defined symbols with some infos:
///   + their position (offset in bytes from begining of data)
///   + their length if defined
///   + their type if defined
///   + weither it's global or not
/// - a set of all symbols (defined, and unknown). Each have an associacted
/// symbol index (from 1 to 256 max)
/// - a set of all constants, associated with their value. This is only used
///    during construction, and is unused afterwards (only the const vals are
///    used, not the key they came from)
typedef struct {
  oc8_as_data_item_t *items_arr; // array grows by realloc
  size_t items_size;
  size_t items_cap;
  uint16_t curr_addr; // position in bytes where next data is inserted

  oc8_as_sym_def_t syms_defs_arr[OC8_AS_MAX_SYMS];
  size_t syms_defs_size;
  oc8_smap_t syms_defs_map; // value is an index in sym_defs_arr

  oc8_smap_t syms_map;   // value is the symbol index
  uint16_t next_sym_idx; // index of next symbol

  oc8_smap_t equ_map; // map for .equ directive
} oc8_as_sfile_t;

/// Allocate and initialize a sfile_t struct
/// @returns the pointer to the initialized struct
oc8_as_sfile_t *oc8_as_sfile_new();

/// Free all memory used by the struct (and the struct itself)
void oc8_as_sfile_free(oc8_as_sfile_t *as);

/// Make sure the sfile is valid
/// If it's not, the program abort
/// Possible errors are:
/// - empty file
/// - trying to set properties (type, size, globl) of extern symbol
void oc8_as_sfile_check(oc8_as_sfile_t *as);

/// Add a symbol at the current position in the code
void oc8_as_sfile_add_sym(oc8_as_sfile_t *as, const char *sym);

/// Returns the index associated with a symbol
/// Create one if doesn't exist yet
uint16_t oc8_as_sfile_get_sym_idx(oc8_as_sfile_t *as, const char *sym);

/// Get the value of a constant (added with .equ directive)
/// Returns a pointer to the value, and null if not found
size_t *oc8_as_sfile_get_equ(oc8_as_sfile_t *as, const char *key);

// ## Add instructions functions ##

void oc8_as_sfile_ins_add_imm(oc8_as_sfile_t *as, uint8_t i_src, uint8_t r_dst);
void oc8_as_sfile_sins_add_imm(oc8_as_sfile_t *as, const char *s_src,
                               uint8_t r_dst);
void oc8_as_sfile_ins_add(oc8_as_sfile_t *as, uint8_t r_src, uint8_t r_dst);
void oc8_as_sfile_ins_add_i(oc8_as_sfile_t *as, uint8_t r_src);

void oc8_as_sfile_ins_and(oc8_as_sfile_t *as, uint8_t r_src, uint8_t r_dst);

void oc8_as_sfile_ins_bcd(oc8_as_sfile_t *as, uint8_t r_src);

void oc8_as_sfile_ins_call(oc8_as_sfile_t *as, uint16_t i_addr);
void oc8_as_sfile_sins_call(oc8_as_sfile_t *as, const char *s_addr);

void oc8_as_sfile_ins_cls(oc8_as_sfile_t *as);

void oc8_as_sfile_ins_draw(oc8_as_sfile_t *as, uint8_t r_x, uint8_t r_y,
                           uint8_t i_h);
void oc8_as_sfile_sins_draw(oc8_as_sfile_t *as, uint8_t r_x, uint8_t r_y,
                            const char *s_h);

void oc8_as_sfile_ins_fspr(oc8_as_sfile_t *as, uint8_t r_src);

void oc8_as_sfile_ins_jmp(oc8_as_sfile_t *as, uint16_t i_addr);
void oc8_as_sfile_sins_jmp(oc8_as_sfile_t *as, const char *s_addr);
void oc8_as_sfile_ins_jmp_v0(oc8_as_sfile_t *as, uint16_t i_addr);
void oc8_as_sfile_sins_jmp_v0(oc8_as_sfile_t *as, const char *s_addr);

void oc8_as_sfile_ins_mov_imm(oc8_as_sfile_t *as, uint8_t i_src, uint8_t r_dst);
void oc8_as_sfile_sins_mov_imm(oc8_as_sfile_t *as, const char *s_src,
                               uint8_t r_dst);
void oc8_as_sfile_ins_mov(oc8_as_sfile_t *as, uint8_t r_src, uint8_t r_dst);
void oc8_as_sfile_ins_mov_i(oc8_as_sfile_t *as, uint16_t i_addr);
void oc8_as_sfile_sins_mov_i(oc8_as_sfile_t *as, const char *s_addr);
void oc8_as_sfile_ins_mov_fdt(oc8_as_sfile_t *as, uint8_t r_dst);
void oc8_as_sfile_ins_mov_dt(oc8_as_sfile_t *as, uint8_t r_src);
void oc8_as_sfile_ins_mov_st(oc8_as_sfile_t *as, uint8_t r_src);

void oc8_as_sfile_ins_movm_st(oc8_as_sfile_t *as, uint8_t r_src);
void oc8_as_sfile_ins_movm_ld(oc8_as_sfile_t *as, uint8_t r_dst);

void oc8_as_sfile_ins_or(oc8_as_sfile_t *as, uint8_t r_src, uint8_t r_dst);

void oc8_as_sfile_ins_rand(oc8_as_sfile_t *as, uint8_t i_mask, uint8_t r_dst);
void oc8_as_sfile_sins_rand(oc8_as_sfile_t *as, const char *s_mask,
                            uint8_t r_dst);

void oc8_as_sfile_ins_ret(oc8_as_sfile_t *as);

void oc8_as_sfile_ins_shl(oc8_as_sfile_t *as, uint8_t r_src, uint8_t r_dst);

void oc8_as_sfile_ins_shr(oc8_as_sfile_t *as, uint8_t r_src, uint8_t r_dst);

void oc8_as_sfile_ins_skpe_imm(oc8_as_sfile_t *as, uint8_t i_y, uint8_t r_x);
void oc8_as_sfile_sins_skpe_imm(oc8_as_sfile_t *as, const char *s_y,
                                uint8_t r_x);
void oc8_as_sfile_ins_skpe(oc8_as_sfile_t *as, uint8_t r_y, uint8_t r_x);
void oc8_as_sfile_ins_skpn_imm(oc8_as_sfile_t *as, uint8_t i_y, uint8_t r_x);
void oc8_as_sfile_sins_skpn_imm(oc8_as_sfile_t *as, const char *s_y,
                                uint8_t r_x);
void oc8_as_sfile_ins_skpn(oc8_as_sfile_t *as, uint8_t r_y, uint8_t r_x);

void oc8_as_sfile_ins_skpkp(oc8_as_sfile_t *as, uint8_t r_src);
void oc8_as_sfile_ins_skpkn(oc8_as_sfile_t *as, uint8_t r_src);

void oc8_as_sfile_ins_sub(oc8_as_sfile_t *as, uint8_t r_src, uint8_t r_dst);

void oc8_as_sfile_ins_subn(oc8_as_sfile_t *as, uint8_t r_src, uint8_t r_dst);

void oc8_as_sfile_ins_sys(oc8_as_sfile_t *as, uint16_t i_addr);
void oc8_as_sfile_sins_sys(oc8_as_sfile_t *as, const char *s_addr);

void oc8_as_sfile_ins_waitk(oc8_as_sfile_t *as, uint8_t r_dst);

void oc8_as_sfile_ins_xor(oc8_as_sfile_t *as, uint8_t r_src, uint8_t r_dst);

// ## Directive Functions ##

void oc8_as_sfile_dir_align(oc8_as_sfile_t *as, uint16_t nbytes);

void oc8_as_sfile_dir_byte(oc8_as_sfile_t *as, uint8_t val);

void oc8_as_sfile_dir_equ(oc8_as_sfile_t *as, const char *key, uint16_t val);

void oc8_as_sfile_dir_globl(oc8_as_sfile_t *as, const char *sym);

void oc8_as_sfile_dir_size(oc8_as_sfile_t *as, const char *sym, uint16_t size);

void oc8_as_sfile_dir_type(oc8_as_sfile_t *as, const char *sym,
                           oc8_as_sym_type_t type);

void oc8_as_sfile_dir_word(oc8_as_sfile_t *as, uint16_t val);

void oc8_as_sfile_dir_zero(oc8_as_sfile_t *as, uint16_t n);

#ifdef __cplusplus
}
#endif

#endif // !OC8_AS_SFILE_H_
