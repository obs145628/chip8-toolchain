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

typedef enum {
  OC8_IS_TYPE_SYS,         // 0NNN
  OC8_IS_TYPE_CLS,         // 00E0
  OC8_IS_TYPE_RET,         // 00EE
  OC8_IS_TYPE_JMP,         // 1NNN
  OC8_IS_TYPE_CALL,        // 2NNN
  OC8_IS_TYPE_SKIP_E_IMM,  // 3XNN
  OC8_IS_TYPE_SKIP_NE_IMM, // 4XNN
  OC8_IS_TYPE_SKIP_E_REG,  // 5XY0
  OC8_IS_TYPE_LD_IMM,      // 6XNN
  OC8_IS_TYPE_ADD_IMM,     // 7XNN
  OC8_IS_TYPE_LD_REG,      // 8XY0
  OC8_IS_TYPE_OR,          // 8XY1
  OC8_IS_TYPE_AND,         // 8XY2
  OC8_IS_TYPE_XOR,         // 8XY3
  OC8_IS_TYPE_ADD_REG,     // 8XY4
  OC8_IS_TYPE_SUB,         // 8XY5
  OC8_IS_TYPE_SHR,         // 8XY6
  OC8_IS_TYPE_SUBN,        // 8XY7
  OC8_IS_TYPE_SHL,         // 8XYE
  OC8_IS_TYPE_SKIP_NE_REG, // 9XYE
  OC8_IS_TYPE_ST_I,        // ANNN
  OC8_IS_TYPE_JMP_V0,      // BNNN
  OC8_IS_TYPE_RAND,        // CXNN
  OC8_IS_TYPE_DRAW,        // DXYN
  OC8_IS_TYPE_SKIP_KP,     // EX9E
  OC8_IS_TYPE_SKIP_KNP,    // EXA1
  OC8_IS_TYPE_LD_DT,       // FX07
  OC8_IS_TYPE_LD_K,        // FX0A
  OC8_IS_TYPE_ST_DT,       // FX15
  OC8_IS_TYPE_ST_ST,       // FX18
  OC8_IS_TYPE_ADD_I,       // FX1E
  OC8_IS_TYPE_LD_F,        // FX29
  OC8_IS_TYPE_LD_B,        // FX33
  OC8_IS_TYPE_ST_REGS,     // FX55
  OC8_IS_TYPE_LD_REGS,     // FX65
} oc8_is_type_t;

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
/// @returnds != 0 il failed to encode instruction
int oc8_is_encode_ins(const oc8_is_ins_t *ins, char *buf);

#ifdef __cplusplus
}
#endif

#endif // !OC8_IS_INS_H_
