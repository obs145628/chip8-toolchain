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

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

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
///   + their position
///   + their length if defined
///   + their type if defined
///   + weither it's global or not
/// - a set of all symbols (defined, and unknown). Each have a fake address
///    associated, used in the opcode to refer to these symbols. All these
///    adresses are < 512, because they are not supposed to be used anyway.
/// - a set of all constants, associated with their value. This is only used
///    during construction, and is unused afterwards (only the const vals are
///    used, not the key they came from)
typedef struct {
  void *todo;
  //@TODO
} oc8_as_sfile_t;

/// Initialize and allocate memory needed by the struct
void oc8_as_sfile_init(oc8_as_sfile_t *as);

/// Free all memory used by the struct
void oc8_as_sfile_free(oc8_as_sfile_t *as);

/// Add a symbol at the current position in the code
void oc8_as_sfile_add_sym(oc8_as_sfile_t *as, const char *sym);

/// Returns the fake address associacted with a symbol
/// Create one if doesn't exist yet
uint16_t oc8_as_sfile_get_sym_addr(oc8_as_sfile_t *as, const char *sym);

/// Get the value of a constant (added with .equ directive)
/// Returns a pointer to the value, and null if not found
uint16_t *oc8_as_sfile_get_equ(oc8_as_sfile_t *as, const char *path);

// ## Add instructions functions ##

void oc8_as_sfile_ins_add_imm(oc8_as_sfile_t *as, uint8_t i_src, uint8_t r_dst);
void oc8_as_sfile_ins_add(oc8_as_sfile_t *as, uint8_t r_src, uint8_t r_dst);
void oc8_as_sfile_ins_add_i(oc8_as_sfile_t *as, uint8_t r_src);

void oc8_as_sfile_ins_and(oc8_as_sfile_t *as, uint8_t r_src, uint8_t r_dst);

void oc8_as_sfile_ins_bcd(oc8_as_sfile_t *as, uint8_t r_src);

void oc8_as_sfile_ins_call(oc8_as_sfile_t *as, uint16_t i_addr);

void oc8_as_sfile_ins_cls(oc8_as_sfile_t *as);

void oc8_as_sfile_ins_draw(oc8_as_sfile_t *as, uint8_t r_x, uint8_t r_y,
                           uint8_t i_h);

void oc8_as_sfile_ins_fspr(oc8_as_sfile_t *as, uint8_t r_src);

void oc8_as_sfile_ins_jmp(oc8_as_sfile_t *as, uint16_t i_addr);
void oc8_as_sfile_ins_jmp_v0(oc8_as_sfile_t *as, uint16_t i_addr);

void oc8_as_sfile_ins_mov_imm(oc8_as_sfile_t *as, uint8_t i_src, uint8_t r_dst);
void oc8_as_sfile_ins_mov(oc8_as_sfile_t *as, uint8_t r_src, uint8_t r_dst);
void oc8_as_sfile_ins_mov_i(oc8_as_sfile_t *as, uint16_t r_addr);
void oc8_as_sfile_ins_mov_fdt(oc8_as_sfile_t *as, uint8_t r_dst);
void oc8_as_sfile_ins_mov_dt(oc8_as_sfile_t *as, uint8_t r_src);
void oc8_as_sfile_ins_mov_st(oc8_as_sfile_t *as, uint8_t r_src);

void oc8_as_sfile_ins_movm_st(oc8_as_sfile_t *as, uint8_t r_src);
void oc8_as_sfile_ins_movm_ld(oc8_as_sfile_t *as, uint8_t r_dst);

void oc8_as_sfile_ins_or(oc8_as_sfile_t *as, uint8_t r_src, uint8_t r_dst);

void oc8_as_sfile_ins_rand(oc8_as_sfile_t *as, uint8_t i_mask, uint8_t r_dst);

void oc8_as_sfile_ins_ret(oc8_as_sfile_t *as);

void oc8_as_sfile_ins_shl(oc8_as_sfile_t *as, uint8_t r_src, uint8_t r_dst);

void oc8_as_sfile_ins_shr(oc8_as_sfile_t *as, uint8_t r_src, uint8_t r_dst);

void oc8_as_sfile_ins_skpe_imm(oc8_as_sfile_t *as, uint8_t i_y, uint8_t r_x);
void oc8_as_sfile_ins_skpe(oc8_as_sfile_t *as, uint8_t r_y, uint8_t r_x);
void oc8_as_sfile_ins_skpn_imm(oc8_as_sfile_t *as, uint8_t i_y, uint8_t r_x);
void oc8_as_sfile_ins_skpn(oc8_as_sfile_t *as, uint8_t r_y, uint8_t r_x);

void oc8_as_sfile_ins_skpkp(oc8_as_sfile_t *as, uint8_t r_src);
void oc8_as_sfile_ins_skpkn(oc8_as_sfile_t *as, uint8_t r_src);

void oc8_as_sfile_ins_sub(oc8_as_sfile_t *as, uint8_t r_src, uint8_t r_dst);

void oc8_as_sfile_ins_subn(oc8_as_sfile_t *as, uint8_t r_src, uint8_t r_dst);

void oc8_as_sfile_ins_sys(oc8_as_sfile_t *as, uint16_t addr);

void oc8_as_sfile_ins_waitk(oc8_as_sfile_t *as, uint8_t r_dst);

void oc8_as_sfile_ins_xor(oc8_as_sfile_t *as, uint8_t r_src, uint8_t r_dst);

// ## Directive Functions ##

void oc8_as_sfile_dir_align(oc8_as_sfile_t *as, uint16_t nbytes);

void oc8_as_sfile_dir_byte(oc8_as_sfile_t *as, uint8_t val);

void oc8_as_sfile_dir_globl(oc8_as_sfile_t *as, const char *sym);

void oc8_as_sfile_dir_equ(oc8_as_sfile_t *as, const char *key, uint16_t val);

void oc8_as_sfile_dir_size(oc8_as_sfile_t *as, const char *sym, uint16_t size);

// @TODO type
void oc8_as_sfile_dir_type(oc8_as_sfile_t *as, const char *sym, int type);

void oc8_as_sfile_dir_word(oc8_as_sfile_t *as, uint16_t val);

void oc8_as_sfile_dir_zero(oc8_as_sfile_t *as, uint16_t n);

#ifdef __cplusplus
}
#endif

#endif // !OC8_AS_SFILE_H_
