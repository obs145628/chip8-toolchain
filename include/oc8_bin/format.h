#ifndef OC8_BIN_FORMAT_H_
#define OC8_BIN_FORMAT_H_

//===--oc8_bin/format.h - binaray format for file_t ---------------*- C -*-===//
//
// oc8_bin library
// Author: Steven Lariau
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Contains structs to read / write oc8_bin_file_t in binary format
///
//===----------------------------------------------------------------------===//

#include "../oc8_defs/oc8_defs.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Binary format:
// - 00-07: magic number: 0x14 0x40 0x4F 0x43 0x38 0x00 0x00 0x00
// - 08-09: version: uint16_t (little endian)
// - 0a-0b: type: uint16_t (little endian). 1 = obj, 2 = bin
// - 0c-0d: number of symbol defs: uint16_t (little endian)
// - 0e-0f: number of symbol refs: uint16_t (little endian)
// - 10-12: ron size in bytes: uint16_t (little endian)
// - 13-??: list of symbol defs
// - ??-??: list of symbol refs
// - ??-??: rom content
//
// Each symbol def is:
// - 00-40: symbol name (65 bytes, ascii, 0-terminated)
// - 41-41: global (1 byte, 0/1)
// - 42-42: type (1 byte, 0 = no, 1 = function, 2 = object)
// - 43-44: addr: uint16_t (little endian)
//
// Each symbol ref is:
// - 00-01: ins_addr: uint16_t (little endian)
// - 02-03: sym_id: uint16_t (little endian)

extern const uint8_t g_oc8_bin_raw_magic_value[8];

typedef struct __attribute__((__packed__)) {
  char magic[8];
  uint16_t version;
  uint16_t type;
  uint16_t nb_syms_defs;
  uint16_t nb_syms_refs;
  uint16_t rom_size;
} oc8_bin_raw_header_t;

typedef struct __attribute__((__packed__)) {
  char name[OC8_MAX_SYM_SIZE + 1];
  uint8_t is_global;
  uint8_t type;
  uint16_t addr;
} oc8_bin_raw_sym_def_t;

typedef struct __attribute__((__packed__)) {
  uint16_t ins_addr;
  uint16_t sym_id;
} oc8_bin_raw_sym_ref_t;

#ifdef __cplusplus
}
#endif

#endif // !OC8_BIN_FORMAT_H_
