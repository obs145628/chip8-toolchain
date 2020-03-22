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

typedef struct {
  uint16_t opcode;
} oc8_is_ins_t;

/// Decode the instruction stored in the 2 bytes of `buf`, and
/// Write result in `ins`
/// @returns != 0 if failed to decode instruction
int oc8_is_decode_ins(oc8_is_ins_t *ins, const char *buf);

/// Encode the instruction `ins`
/// Write the opcode in 2 bytes of `buf`
/// Also write it in opcode field of `ins`
/// @returnds != 0 il failed to encode instruction
int oc8_is_encode_ins(const oc8_is_ins_t *ins, char *buf);

#ifdef __cplusplus
}
#endif

#endif // !OC8_IS_INS_H_
