#ifndef OC8_IS_INS_H_
#define OC8_IS_INS_H_

//===--oc8_is/ins.h - ins_t struct def ----------------------------*- C -*-===//
//
// oc8_is library
// Author: Steven Lariau
//
//===----------------------------------------------------------------------===//
///
/// \file
/// ins_t struct that represesent a CHIP-8 instruction
///
//===----------------------------------------------------------------------===//

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// The 35 types of instructions
// Described by the docs
typedef enum {
  OC8_IS_TYPE_0NNN,
  OC8_IS_TYPE_00E0,
  OC8_IS_TYPE_00EE,
  OC8_IS_TYPE_1NNN,
  OC8_IS_TYPE_2NNN,
  OC8_IS_TYPE_3XNN,
  OC8_IS_TYPE_4XNN,
  OC8_IS_TYPE_5XY0,
  OC8_IS_TYPE_6XNN,
  OC8_IS_TYPE_7XNN,
  OC8_IS_TYPE_8XY0,
  OC8_IS_TYPE_8XY1,
  OC8_IS_TYPE_8XY2,
  OC8_IS_TYPE_8XY3,
  OC8_IS_TYPE_8XY4,
  OC8_IS_TYPE_8XY5,
  OC8_IS_TYPE_8XY6,
  OC8_IS_TYPE_8XY7,
  OC8_IS_TYPE_8XYE,
  OC8_IS_TYPE_9XY0,
  OC8_IS_TYPE_ANNN,
  OC8_IS_TYPE_BNNN,
  OC8_IS_TYPE_CXNN,
  OC8_IS_TYPE_DXYN,
  OC8_IS_TYPE_EX9E,
  OC8_IS_TYPE_EXA1,
  OC8_IS_TYPE_FX07,
  OC8_IS_TYPE_FX0A,
  OC8_IS_TYPE_FX15,
  OC8_IS_TYPE_FX18,
  OC8_IS_TYPE_FX1E,
  OC8_IS_TYPE_FX29,
  OC8_IS_TYPE_FX33,
  OC8_IS_TYPE_FX55,
  OC8_IS_TYPE_FX65,
} oc8_is_type_t;

/// Represent an instruction
/// Used to decode from / encode to opcode
typedef struct {
  // ins opcode, optional, 0 if not defined
  // set to 0 when filling other fields manually, then call encode_ins that
  // fills it
  uint16_t opcode;

  // between [0, 35[. Type as defined by the instructions ref order
  oc8_is_type_t type;

  // Instruction operands (may have 1, 2 or three)
  // Same order than in the opcode
  unsigned operands[3];
} oc8_is_ins_t;

/// Decode the instruction stored in the 2 bytes of `buf`, and
/// Write result in `ins`
/// @returns != 0 if failed to decode instruction
int oc8_is_decode_ins(oc8_is_ins_t *ins, const char *buf);

/// Encode the instruction `ins`
/// Write the opcode in 2 bytes of `buf`
/// Also write it in opcode field of `ins`
/// `buf` may be NULL: only write to opcode field
/// @returns != 0 il failed to encode instruction
int oc8_is_encode_ins(oc8_is_ins_t *ins, char *buf);

#ifdef __cplusplus
}
#endif

#endif // !OC8_IS_INS_H_
