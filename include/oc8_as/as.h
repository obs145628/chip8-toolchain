#ifndef OC8_AS_AS_H_
#define OC8_AS_AS_H_

//===--oc8_as/as.h - SFile Assembler ------------------------------*- C -*-===//
//
// oc8_as library
// Author: Steven Lariau
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Assembler: build oc8_fin_file_t from oc8_as_sfile_t
///
//===----------------------------------------------------------------------===//

#include <stddef.h>
#include <stdint.h>

#include "../oc8_bin/file.h"
#include "sfile.h"

#ifdef __cplusplus
extern "C" {
#endif

/// Build and fill `bf` from `sf`
/// Works correctly only if `oc8_as_sfile_check(sf)` was successfull
/// `oc8_bin_file_check` isn't called at the end
/// `bf` must not be initialized
void oc8_as_compile_sfile(oc8_as_sfile_t *sf, oc8_bin_file_t *bf);

#ifdef __cplusplus
}
#endif

#endif // !OC8_AS_AS_H_
