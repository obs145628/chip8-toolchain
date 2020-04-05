#ifndef OC8_EMU_DEBUG_H_
#define OC8_EMU_DEBUG_H_

//===--oc8_emu/debug.h - Debug tools ------------------------------*- C -*-===//
//
// oc8_emu library
// Author: Steven Lariau
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Debug tools for oc8_emu
///
//===----------------------------------------------------------------------===//

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "../oc8_bin/file.h"

/// bin file for the ROM being executed
/// This is optional, and not directly used by the emulator
/// Null if version is 0
extern oc8_bin_file_t g_oc8_emu_bin_file;

/// Returns true if `g_oc8_emu_bin_file` isn't empty
static inline int g_oc8_emu_bin_file_loaded() {
  return g_oc8_emu_bin_file.header.version != 0;
}

/// Calling this function assure `g_oc8_emu_bin_file` isn't empty
/// If bin already loaded, does nothing
/// Otherwhise it generate an empty bin file from the actual RAM
void oc8_emu_gen_debug_bin_file();

#ifdef __cplusplus
}
#endif

/// Clear if needed and reset the bin file
void oc8_emu_init_debug();

#endif // !OC8_EMU_DEBUG_H_
