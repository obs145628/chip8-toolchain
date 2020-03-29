#ifndef OC8_DEFS_DEBUG_H_
#define OC8_DEFS_DEBUG_H_

//===--oc8_defs/debug.h - Debug utils -----------------------------*- C -*-===//
//
// oc8 toolchain
// Author: Steven Lariau
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Misc defs for debug purposes
///
//===----------------------------------------------------------------------===//

#include <assert.h>
#include <stdlib.h>

#ifdef NDEBUG
#define PANIC() (exit(1))
#else
#define PANIC() (assert(0))
#endif

#endif // !OC8_DEFS_DEBUG_H_
