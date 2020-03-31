#ifndef OC8_BIN_BIN_READER_H_
#define OC8_BIN_BIN_READER_H_

//===--oc8_bin/bin_reader.h - binaray reader for file_t -----------*- C -*-===//
//
// oc8_bin library
// Author: Steven Lariau
//
//===----------------------------------------------------------------------===//
///
/// \file
/// file_t Binary Reader of oc8_bin_file_t
/// Create file_t from raw data in memory buffer
///
//===----------------------------------------------------------------------===//

#include "file.h"

#ifdef __cplusplus
extern "C" {
#endif

/// Build file `f` using raw binaray data in `in_buf`
/// `f` must be unitialized.
/// This function doesn't call `oc8_bin_file_check`
/// `buf_len` is used to make sure the raw data has the right size
void oc8_bin_read_file_raw(oc8_bin_file_t *f, const void *in_buf,
                           size_t buf_len);

/// Wrapper around `oc8_bin_read_file_raw` to read data directly from a file
void oc8_bin_read_from_file(oc8_bin_file_t *f, const char *path);

#ifdef __cplusplus
}
#endif

#endif // !OC8_BIN_BIN_READER_H_
