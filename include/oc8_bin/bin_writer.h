#ifndef OC8_BIN_BIN_WRITER_H_
#define OC8_BIN_BIN_WRITER_H_

//===--oc8_bin/bin_writer.h - binaray writer for file_t -----------*- C -*-===//
//
// oc8_bin library
// Author: Steven Lariau
//
//===----------------------------------------------------------------------===//
///
/// \file
/// file_t Binary Writer of oc8_bin_file_t
/// Save file_t to to raw binary form in memory buffer
///
//===----------------------------------------------------------------------===//

#include "file.h"

#ifdef __cplusplus
extern "C" {
#endif

/// Write `f` in raw binaray format to `out_buf`
/// Works correctly only if `oc8_bin_file_check(f, 0)` was successfull
/// Returns the number of bytes written
/// If `out_buf` is NULL, directly returns with the size (O(1))
size_t oc8_bin_write_file_raw(oc8_bin_file_t *f, void *out_buf);

/// Wrapper around `oc8_bin_write_file_raw` to write data directly to a file
void oc8_bin_write_to_file(oc8_bin_file_t *f, const char *path);

#ifdef __cplusplus
}
#endif

#endif // !OC8_BIN_BIN_WRITER_H_
