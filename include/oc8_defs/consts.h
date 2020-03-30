#ifndef OC8_DEFS_CONSTS_H_
#define OC8_DEFS_CONSTS_H_

//===--oc8_defs/consts.h - General constants ----------------------*- C -*-===//
//
// oc8 toolchain
// Author: Steven Lariau
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Define many constants usefull accros the whole toolchain
///
//===----------------------------------------------------------------------===//

// Maximum size of a symbol (unique identifier of binary data symbol)
#define OC8_MAX_SYM_SIZE (64)

// Beginning of the ROM in memory (before is system reserved data)
#define OC8_ROM_START (0x200)

// Total amount of memory in the CHIP-8
#define OC8_MEMORY_SIZE (0x1000)

#endif // !OC8_DEFS_CONSTS_H_
