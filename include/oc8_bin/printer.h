#ifndef OC8_BIN_PRINTER_H_
#define OC8_BIN_PRINTER_H_

//===--oc8_bin/printer.h - human-readable printer for file_t ------*- C -*-===//
//
// oc8_bin library
// Author: Steven Lariau
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Define struct oc8_bin_printer_t, used to print obj code in a human-readable
/// form
///
//===----------------------------------------------------------------------===//

#include "../oc8_defs/oc8_defs.h"
#include "file.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  OC8_BIN_PRINTER_DATA_OPCODE,
  OC8_BIN_PRINTER_DATA_BYTE,
  OC8_BIN_PRINTER_DATA_WORD,
  OC8_BIN_PRINTER_DATA_UNKOWN,
} oc8_bin_printer_data_t;

/// Contain all infos needed to print data at a specific addr
/// Computed only once at initialization
/// @extra multiple labels at same location not handled yet
/// Doesn't bug, it's just only one of the labels get printed
typedef struct {
  uint16_t sym_def;
  uint16_t sym_ref;
  oc8_bin_printer_data_t ty;
} oc8_bin_printer_infos_t;

/// Print data (opcodes, raw data) in a human-readable form
/// Really close to assembly format (same syntax and instructions)
/// But cannot be parser by oc8-parser
/// Must do a lot of guessing to infer if what is at a specific address is
/// opcode or just data
typedef struct {
  oc8_bin_file_t *bf;
  oc8_bin_printer_infos_t *infos; // array indexed by rom addr
  char *out_buf;                  // where code is actually printed
  size_t out_buf_size;            // nb bytes in out_buf ('\0' is included)
  size_t out_buf_cap;             // `out_buf` capacity
} oc8_bin_printer_t;

/// Allocate and init `p`
/// `bf` pointer must be valid during the whole use of `p`
void oc8_bin_printer_init(oc8_bin_printer_t *p, oc8_bin_file_t *bf);

/// Deallocate all resources used by `p`
void oc8_bin_printer_free(oc8_bin_printer_t *p);

/// Print human readable-data at addr `addr`
/// Bytes written as 0-terminated at `p->out_buf`
/// @param print_sym_defs - If true, print all symbols defs before (eg `foo:\n`)
/// @param print_sym_refs - If true, print all symbol refs in instruction by
/// name, and not by value
/// @param print_sym_ids - If true, symbol refs are printed using the index, and
/// not the name. This parameter is to output odb compatible text.
/// @param data_hint - Tell what kind of data is to be printed. If unkown, it's
/// guessed by the printer
/// @param buf_size - Will contain the number of bytes written (without final
/// \0)
/// @param inc - will contain the number of bytes analyzed from the rom (1 for
/// byte, 2 for opcode, etc)
/// @return 0 if success, 1 otherwhise
int oc8_bin_printer_print_at(oc8_bin_printer_t *p, uint16_t addr,
                             int print_sym_defs, int print_sym_refs,
                             int print_sym_ids,
                             oc8_bin_printer_data_t data_hint, size_t *buf_size,
                             uint16_t *inc);

#ifdef __cplusplus
}
#endif

#endif // !OC8_BIN_PRINTER_H_
