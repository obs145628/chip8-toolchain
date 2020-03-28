#ifndef OC8_AS_PRINTER_H_
#define OC8_AS_PRINTER_H_

//===--oc8_as/printer.h - SFile Printer ---------------------------*- C -*-===//
//
// oc8_as library
// Author: Steven Lariau
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Write SFile to output buffer, as string, that can be parser again
///
//===----------------------------------------------------------------------===//

#include <stddef.h>
#include <stdint.h>

#include "sfile.h"

#ifdef __cplusplus
extern "C" {
#endif

/// Write `as` as text_form that can be parsed again
/// Bytes are output to `out_buf`. No \0 is written at the end
/// If `out_buf` is NULL, nothing is written
/// @param buf_cb Function called every time the buffer is full, and at the end
/// for the last bytes
/// If NULL, never called, but can overflow `out_buf`
///  - arg 1: number of bytes writen to `out_buf`
///  - arg 2: The `arg` argument
/// @param arg
/// @returns number of bytes written (or would have if `out_buf` NULL) (size of
/// text format)
size_t oc8_as_print_sfile(oc8_as_sfile_t *sf, char *out_buf, size_t out_buf_len,
                          void (*buf_cb)(size_t, void *), void *arg);

#ifdef __cplusplus
}
#endif

#endif // !OC8_AS_PRINTER_H_
