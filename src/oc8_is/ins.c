#include "oc8_is/ins.h"

#include <stdio.h>

// swap byte 1 and byte 2
#define OPCODE_SWAP(X) ((X << 8) | (X >> 8))

// HX, X from left to right
// get Hexa value (4 bits)
#define OPCODE_H0(X) (((X) >> 12) & 0xF)
#define OPCODE_H1(X) (((X) >> 8) & 0xF)
#define OPCODE_H2(X) (((X) >> 4) & 0xF)
#define OPCODE_H3(X) (((X) >> 0) & 0xF)
#define OPCODE_H123(X) ((X)&0xFFF)
#define OPCODE_H23(X) ((X)&0xFF)

/// Decode the instruction stored in the 2 bytes of `buf`, and
/// Write result in `ins`
/// @returns != 0 if failed to decode instruction
int oc8_is_decode_ins(oc8_is_ins_t *ins, const char *buf) {
  uint16_t opcode = *((uint16_t *)buf);

  // instructions opcodes stored in big endian, must be swapped for LE systems.
  // @TODO add conditions to not swap for BE systems
  opcode = OPCODE_SWAP(opcode);

  // opcodes starting by 0
  if (opcode == 0x00E0) {
    ins->type = OC8_IS_TYPE_CLS;
  }

  else if (opcode == 0x00EE) {
    ins->type = OC8_IS_TYPE_RET;
  }

  else if (OPCODE_H0(opcode) == 0x0) {
    ins->type = OC8_IS_TYPE_SYS;
    ins->operands[0] = OPCODE_H123(opcode);
  }

  // opcodes starting by 1
  else if (OPCODE_H0(opcode) == 1) {
    ins->type = OC8_IS_TYPE_JMP;
    ins->operands[0] = OPCODE_H123(opcode);
  }

  // opcodes starting by 2
  else if (OPCODE_H0(opcode) == 2) {
    ins->type = OC8_IS_TYPE_CALL;
    ins->operands[0] = OPCODE_H123(opcode);
  }

  // opcodes starting by 3
  else if (OPCODE_H0(opcode) == 3) {
    ins->type = OC8_IS_TYPE_SKIP_E_IMM;
    ins->operands[0] = OPCODE_H1(opcode);
    ins->operands[1] = OPCODE_H23(opcode);
  }

  // opcodes starting by 4
  else if (OPCODE_H0(opcode) == 4) {
    ins->type = OC8_IS_TYPE_SKIP_NE_IMM;
    ins->operands[0] = OPCODE_H1(opcode);
    ins->operands[1] = OPCODE_H23(opcode);
  }

  // opcodes starting by 5
  else if (OPCODE_H0(opcode) == 5 && OPCODE_H3(opcode) == 0) {
    ins->type = OC8_IS_TYPE_SKIP_E_REG;
    ins->operands[0] = OPCODE_H1(opcode);
    ins->operands[1] = OPCODE_H2(opcode);
  }

  // opcodes starting by 6
  else if (OPCODE_H0(opcode) == 6) {
    ins->type = OC8_IS_TYPE_LD_IMM;
    ins->operands[0] = OPCODE_H1(opcode);
    ins->operands[1] = OPCODE_H23(opcode);
  }

  // opcodes starting by 7
  else if (OPCODE_H0(opcode) == 7) {
    ins->type = OC8_IS_TYPE_ADD_IMM;
    ins->operands[0] = OPCODE_H1(opcode);
    ins->operands[1] = OPCODE_H23(opcode);
  }

  // opcodes starting by 8
  else if (OPCODE_H0(opcode) == 0x8 && OPCODE_H3(opcode) == 0x0) {
    ins->type = OC8_IS_TYPE_LD_REG;
    ins->operands[0] = OPCODE_H1(opcode);
    ins->operands[1] = OPCODE_H2(opcode);
  }

  else if (OPCODE_H0(opcode) == 0x8 && OPCODE_H3(opcode) == 0x1) {
    ins->type = OC8_IS_TYPE_OR;
    ins->operands[0] = OPCODE_H1(opcode);
    ins->operands[1] = OPCODE_H2(opcode);
  }

  else if (OPCODE_H0(opcode) == 0x8 && OPCODE_H3(opcode) == 0x2) {
    ins->type = OC8_IS_TYPE_AND;
    ins->operands[0] = OPCODE_H1(opcode);
    ins->operands[1] = OPCODE_H2(opcode);
  }

  else if (OPCODE_H0(opcode) == 0x8 && OPCODE_H3(opcode) == 0x3) {
    ins->type = OC8_IS_TYPE_XOR;
    ins->operands[0] = OPCODE_H1(opcode);
    ins->operands[1] = OPCODE_H2(opcode);
  }

  else if (OPCODE_H0(opcode) == 0x8 && OPCODE_H3(opcode) == 0x4) {
    ins->type = OC8_IS_TYPE_ADD_REG;
    ins->operands[0] = OPCODE_H1(opcode);
    ins->operands[1] = OPCODE_H2(opcode);
  }

  else if (OPCODE_H0(opcode) == 0x8 && OPCODE_H3(opcode) == 0x5) {
    ins->type = OC8_IS_TYPE_SUB;
    ins->operands[0] = OPCODE_H1(opcode);
    ins->operands[1] = OPCODE_H2(opcode);
  }

  else if (OPCODE_H0(opcode) == 0x8 && OPCODE_H3(opcode) == 0x6) {
    ins->type = OC8_IS_TYPE_SHR;
    ins->operands[0] = OPCODE_H1(opcode);
    ins->operands[1] = OPCODE_H2(opcode);
  }

  else if (OPCODE_H0(opcode) == 0x8 && OPCODE_H3(opcode) == 0x7) {
    ins->type = OC8_IS_TYPE_SUBN;
    ins->operands[0] = OPCODE_H1(opcode);
    ins->operands[1] = OPCODE_H2(opcode);
  }

  else if (OPCODE_H0(opcode) == 0x8 && OPCODE_H3(opcode) == 0xE) {
    ins->type = OC8_IS_TYPE_SHL;
    ins->operands[0] = OPCODE_H1(opcode);
    ins->operands[1] = OPCODE_H2(opcode);
  }

  // opdocdes starting by 9
  else if (OPCODE_H0(opcode) == 0x9 && OPCODE_H3(opcode) == 0x0) {
    ins->type = OC8_IS_TYPE_SKIP_NE_REG;
    ins->operands[0] = OPCODE_H1(opcode);
    ins->operands[1] = OPCODE_H2(opcode);
  }

  // opcodes starting by A
  else if (OPCODE_H0(opcode) == 0xA) {
    ins->type = OC8_IS_TYPE_ST_I;
    ins->operands[0] = OPCODE_H123(opcode);
  }

  // opcodes starting by B
  else if (OPCODE_H0(opcode) == 0xB) {
    ins->type = OC8_IS_TYPE_JMP_V0;
    ins->operands[0] = OPCODE_H123(opcode);
  }

  // opcodes starting by C
  else if (OPCODE_H0(opcode) == 0xC) {
    ins->type = OC8_IS_TYPE_RAND;
    ins->operands[0] = OPCODE_H1(opcode);
    ins->operands[1] = OPCODE_H23(opcode);
  }

  // opcodes starting by D
  else if (OPCODE_H0(opcode) == 0xD) {
    ins->type = OC8_IS_TYPE_DRAW;
    ins->operands[0] = OPCODE_H1(opcode);
    ins->operands[1] = OPCODE_H2(opcode);
    ins->operands[2] = OPCODE_H3(opcode);
  }

  // opcodes starting by E
  else if (OPCODE_H0(opcode) == 0xE && OPCODE_H23(opcode) == 0x9E) {
    ins->type = OC8_IS_TYPE_SKIP_KP;
    ins->operands[0] = OPCODE_H1(opcode);
  }

  else if (OPCODE_H0(opcode) == 0xE && OPCODE_H23(opcode) == 0xA1) {
    ins->type = OC8_IS_TYPE_SKIP_KNP;
    ins->operands[0] = OPCODE_H1(opcode);
  }

  // opcodes starting by F
  else if (OPCODE_H0(opcode) == 0xF && OPCODE_H23(opcode) == 0x07) {
    ins->type = OC8_IS_TYPE_LD_DT;
    ins->operands[0] = OPCODE_H1(opcode);
  }

  else if (OPCODE_H0(opcode) == 0xF && OPCODE_H23(opcode) == 0x0A) {
    ins->type = OC8_IS_TYPE_LD_K;
    ins->operands[0] = OPCODE_H1(opcode);
  }

  else if (OPCODE_H0(opcode) == 0xF && OPCODE_H23(opcode) == 0x15) {
    ins->type = OC8_IS_TYPE_ST_DT;
    ins->operands[0] = OPCODE_H1(opcode);
  }

  else if (OPCODE_H0(opcode) == 0xF && OPCODE_H23(opcode) == 0x18) {
    ins->type = OC8_IS_TYPE_ST_ST;
    ins->operands[0] = OPCODE_H1(opcode);
  }

  else if (OPCODE_H0(opcode) == 0xF && OPCODE_H23(opcode) == 0x1E) {
    ins->type = OC8_IS_TYPE_ADD_I;
    ins->operands[0] = OPCODE_H1(opcode);
  }

  else if (OPCODE_H0(opcode) == 0xF && OPCODE_H23(opcode) == 0x29) {
    ins->type = OC8_IS_TYPE_LD_F;
    ins->operands[0] = OPCODE_H1(opcode);
  }

  else if (OPCODE_H0(opcode) == 0xF && OPCODE_H23(opcode) == 0x33) {
    ins->type = OC8_IS_TYPE_LD_B;
    ins->operands[0] = OPCODE_H1(opcode);
  }

  else if (OPCODE_H0(opcode) == 0xF && OPCODE_H23(opcode) == 0x55) {
    ins->type = OC8_IS_TYPE_ST_REGS;
    ins->operands[0] = OPCODE_H1(opcode);
  }

  else if (OPCODE_H0(opcode) == 0xF && OPCODE_H23(opcode) == 0x65) {
    ins->type = OC8_IS_TYPE_LD_REGS;
    ins->operands[0] = OPCODE_H1(opcode);
  }

  else {
    // Opcode not recognized
    fprintf(stderr, "Warning: Unknown code [%x]\n", (int)opcode);
    return 1;
  }

  return 0;
}

/// Encode the instruction `ins`
/// Write the opcode in 2 bytes of `buf`
/// Also write it in opcode field of `ins`
/// @returnds != 0 il failed to encode instruction
int oc8_is_encode_ins(const oc8_is_ins_t *ins, char *buf) {
  // @TODO
  (void)ins;
  (void)buf;
  return 0;
}
