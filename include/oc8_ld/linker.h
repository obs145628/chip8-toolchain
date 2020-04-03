#ifndef OC8_LD_LINKER_H_
#define OC8_LD_LINKER_H_

//===--oc8_ld/linker.h - linker struct definition ----------------*- C -*-===//
//
// oc8_ld library
// Author: Steven Lariau
//
//===----------------------------------------------------------------------===//
///
/// \file
/// linker_t struct definition
/// Take many bin file objects and combine them into one binary file
///
//===----------------------------------------------------------------------===//

#include <stddef.h>
#include <stdint.h>

#include "oc8_bin/file.h"
#include "oc8_defs/oc8_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

// Linking process
//
// 1) Compute the start address for every obj in the final ROM file
//    All obj code is located one after the other, starting at 0x200
//    Check if the output ROM size isn't too big (max 4K)
//
// 2) Go through all the local symbol defs (addr != 0) for each obj file
//    Add every def to the output symbol defs
//    Fix the addr using the start addr of the obj
//    All other fields stay the same
//    If is_globl=1, check if the symbol is already in the defs list. If it is,
//    it's a multiple definiton error
//    Set the mapping from obj sym_id to output sym_id
//
// 3) Go through all the extern symbol defs (addr == 0) for each obj file
//    Try to find it in the output sumbol defs using the symbol name
//    If not found, it's a undefined reference error
//    Set the mapping from obj sym_id to output sym_id
//
// 4) Go through all the symbol refs for each obj file
//    Fix and add every ref to the output symbol refs list
//    Fix ins_addr: use the start addr of the obj
//    Fix sym_id: use the mapping from obj sym_id to output sym_id
//
// 5) Concat binary data of all obj files into one for the output bin file.
//
// 6) Go through all the symbol refs of the output bin file
//    For each one, fix the opcode in the RAM by using the known addr from the
//    symbols def as the constant value
//
// A synbol '_start' must be defined, that's the program entry point.
// Actually the entry point is still at 0x200, but the linker add this code at
// the beginning:
// _rom_begin:
//  jmp _start

// Contains all infos needed during linking for one obj file `bf`
typedef struct {
  oc8_bin_file_t *bf;
  uint16_t *syms_map; // mapping from obj sym_id to output sym_id
  uint16_t rom_addr;  // ROM addr offset in the output binary
} oc8_ld_unit_t;

typedef struct {
  oc8_ld_unit_t **units_arr; // allocated array of units, grows by realloc
  size_t units_size;
  size_t units_cap;
  oc8_bin_file_t start_bf;
  int use_start_bf;
} oc8_ld_linker_t;

/// Initialize the linker `ld` with no input files
/// If use_start_sym is true, the linker use the start entry point, as explained
/// above
void oc8_ld_linker_init(oc8_ld_linker_t *ld, int use_start_sym);

/// Free all ressources alocated by the linker struct `ld`
void oc8_ld_linker_free(oc8_ld_linker_t *ld);

/// Add an input bin object file `bf` (.c8o) to the linker unit `ld`
/// Doesn't perform linking yet, just add `bf` to the list of objects
/// `bf` must not be of type object and not bin
/// Doesn't call `oc8_bin_file_check` on `bf`
/// `bf` pointer must still be valid when calling `oc8_ld_linker_link`
void oc8_ld_linker_add(oc8_ld_linker_t *ld, oc8_bin_file_t *bf);

/// Link all object files specified with `oc8_ld_linker_add` into one header
/// file Output binary file written to `out_bf` `out_bf` must be initialized
/// Doesn't call `oc8_bin_file_check` at the end
void oc8_ld_linker_link(oc8_ld_linker_t *ld, oc8_bin_file_t *out_bf);

#ifdef __cplusplus
}
#endif

#endif // !OC8_LD_LINKER_H_
