#ifndef OC8_BIN_FILE_H_
#define OC8_BIN_FILE_H_

//===--oc8_bin/file.h - bin_file struct definition ----------------*- C -*-===//
//
// oc8_bin library
// Author: Steven Lariau
//
//===----------------------------------------------------------------------===//
///
/// \file
/// file_t struct definition
/// Struct that contains a binary file content (either c8o or c8bin)
///
//===----------------------------------------------------------------------===//

#include <stddef.h>
#include <stdint.h>

#include "oc8_defs/oc8_defs.h"
#include "oc8_smap/oc8_smap.h"

#ifdef __cplusplus
extern "C" {
#endif

// bin struct:

// - header (oc8_bin_header_t)
//   + version: uint16_t, must be 10
//   + type: object file (.c8o) or binary file (.c8bin)

// - list of symbols defs, with some metadata: (oc8_bin_sym_def_t)
//   + id: unique identifier, uint16_t. Position in the list
//   + name: original symbol name, not unique (but 2 globals cannot have same
//   name)
//   + global: true/false
//   + type (function, object, or no)
//     used to know how to print data at specific addr.
//     If no or unknown, print as binary data
//   + addr in binary data (remember: starts at 0x200)
//     if 0, symbol is unkown

// - list of all symbols references (oc8_bin_sym_ref_t)
//   These are the references in the ROM content to symbols.
//   Each ref is a pair (ins_addr, sym_id) (ins start at 0x200)
//   There is no specific oredering
//   They are used when printing code to display the symbol name
//   And when linking, to replace whatever value in the opcode with the true sym
//   addr

// - actual binary ROM content. (remember: starts at 0x200)
//   This is the true content. Can simply copy/paste this to get chip-8 ROM.
//
//
// Rules To be valid:
// - Header:
//   + version must be supported
//   + type must be valid
// - Symbol def:
//   + All symbols must have a unique id
//   + name must be 0-terminated, and a valid @id ASCII string
//   + 2 global symbols cannot have the same name
//   + global must be valid
//   + type must be valid
//   + addr must be in the range of the ROM content (0x200 to ROM end), or 0
//     (late check)
// - Symbol ref:
//   + ins_addr must be in the valid range of the ROM content (0x200 to ROM end)
//     (late check)
//   + sym_id must refer to an existing def (late check)
// - Rom Content: Cannot go beyond addr 0xFFF (starts at 0x200)
//
//
// More rules to be runnable:
// - Header:
//   + type must be binary file
// - Symbol def:
//   + addr must not be 0

typedef enum {
  OC8_BIN_FILE_TYPE_OBJ,
  OC8_BIN_FILE_TYPE_BIN,
} oc8_bin_file_type_t;

typedef enum {
  OC8_BIN_SYM_TYPE_FUN,
  OC8_BIN_SYM_TYPE_OBJ,
  OC8_BIN_SYM_TYPE_NO,
} oc8_bin_sym_type_t;

typedef struct {
  uint16_t version;
  oc8_bin_file_type_t type;
} oc8_bin_header_t;

typedef struct {
  uint16_t id;
  char name[OC8_MAX_SYM_SIZE + 1];
  int is_global;
  oc8_bin_sym_type_t type;
  uint16_t addr;
} oc8_bin_sym_def_t;

typedef struct {
  uint16_t ins_addr;
  uint16_t sym_id;
} oc8_bin_sym_ref_t;

/// Binary file (either object or full binary ROM)
/// You must never write field structs directly, and use functions
/// They perform arguments checking
/// Some more complex checks are run with file_check() once the struct is
/// complete
/// The only exception is for ROM, once ts'is setup (function init_room), you
/// can directly write anything to the buffer
typedef struct {
  oc8_bin_header_t header;

  // pointer to 1st item of allocated array. Alloc once with known final size
  oc8_bin_sym_def_t *syms_defs;
  size_t syms_defs_size;
  size_t syms_defs_cap;

  // pointer to 1st item of allocated array, grows (realloc) when adding refs
  oc8_bin_sym_ref_t *syms_refs;
  size_t syms_refs_size;
  size_t syms_refs_cap;
  oc8_smap_t globals; // globals defs, val is symbol id

  // pointer to allocated ROM data, struct responsible for its lifetime
  uint8_t *rom;
  size_t rom_size;
} oc8_bin_file_t;

/// Initialize `bf` to empty, unprepared file, and allocate all needed memory
void oc8_bin_file_init(oc8_bin_file_t *bf);

/// Free all memory allocated by `bf`
void oc8_bin_file_free(oc8_bin_file_t *bf);

/// Check if the file is valid, after it's complete
/// If `is_bin`, do more tests to be sure it can be runned
/// Panics if there is an error
void oc8_bin_file_check(oc8_bin_file_t *bf, int is_bin);

void oc8_bin_file_set_version(oc8_bin_file_t *bf, uint16_t version);

void oc8_bin_file_set_type(oc8_bin_file_t *bf, oc8_bin_file_type_t type);

/// Can only be called once to set the max number of definitions
/// Cannot be changed later
void oc8_bin_file_set_defs_count(oc8_bin_file_t *bf, size_t len);

/// @returns id of the new symbol
uint16_t oc8_bin_file_add_def(oc8_bin_file_t *bf, const char *name,
                              int is_global, oc8_bin_sym_type_t type,
                              uint16_t addr);

/// Can add a ref before it's added in the defs
void oc8_bin_file_add_ref(oc8_bin_file_t *bf, uint16_t ins_addr,
                          uint16_t sym_id);

/// Allocate memory for the ROM, can only be called once,
/// Cannot be resized later
void oc8_bin_file_init_rom(oc8_bin_file_t *bf, size_t rom_size);

#ifdef __cplusplus
}
#endif

#endif // !OC8_BIN_FILE_H_
