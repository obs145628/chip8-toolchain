#include "oc8_is/ins.h"

#include <stdio.h>
#include <string.h>

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
  ins->opcode = opcode;

  // instructions opcodes stored in big endian, must be swapped for LE systems.
  // @TODO add conditions to not swap for BE systems
  opcode = OPCODE_SWAP(opcode);

  // opcodes starting by 0
  if (opcode == 0x00E0) {
    ins->type = OC8_IS_TYPE_00E0;
  }

  else if (opcode == 0x00EE) {
    ins->type = OC8_IS_TYPE_00EE;
  }

  else if (OPCODE_H0(opcode) == 0x0) {
    ins->type = OC8_IS_TYPE_0NNN;
    ins->operands[0] = OPCODE_H123(opcode);
  }

  // opcodes starting by 1
  else if (OPCODE_H0(opcode) == 1) {
    ins->type = OC8_IS_TYPE_1NNN;
    ins->operands[0] = OPCODE_H123(opcode);
  }

  // opcodes starting by 2
  else if (OPCODE_H0(opcode) == 2) {
    ins->type = OC8_IS_TYPE_2NNN;
    ins->operands[0] = OPCODE_H123(opcode);
  }

  // opcodes starting by 3
  else if (OPCODE_H0(opcode) == 3) {
    ins->type = OC8_IS_TYPE_3XNN;
    ins->operands[0] = OPCODE_H1(opcode);
    ins->operands[1] = OPCODE_H23(opcode);
  }

  // opcodes starting by 4
  else if (OPCODE_H0(opcode) == 4) {
    ins->type = OC8_IS_TYPE_4XNN;
    ins->operands[0] = OPCODE_H1(opcode);
    ins->operands[1] = OPCODE_H23(opcode);
  }

  // opcodes starting by 5
  else if (OPCODE_H0(opcode) == 5 && OPCODE_H3(opcode) == 0) {
    ins->type = OC8_IS_TYPE_5XY0;
    ins->operands[0] = OPCODE_H1(opcode);
    ins->operands[1] = OPCODE_H2(opcode);
  }

  // opcodes starting by 6
  else if (OPCODE_H0(opcode) == 6) {
    ins->type = OC8_IS_TYPE_6XNN;
    ins->operands[0] = OPCODE_H1(opcode);
    ins->operands[1] = OPCODE_H23(opcode);
  }

  // opcodes starting by 7
  else if (OPCODE_H0(opcode) == 7) {
    ins->type = OC8_IS_TYPE_7XNN;
    ins->operands[0] = OPCODE_H1(opcode);
    ins->operands[1] = OPCODE_H23(opcode);
  }

  // opcodes starting by 8
  else if (OPCODE_H0(opcode) == 0x8 && OPCODE_H3(opcode) == 0x0) {
    ins->type = OC8_IS_TYPE_8XY0;
    ins->operands[0] = OPCODE_H1(opcode);
    ins->operands[1] = OPCODE_H2(opcode);
  }

  else if (OPCODE_H0(opcode) == 0x8 && OPCODE_H3(opcode) == 0x1) {
    ins->type = OC8_IS_TYPE_8XY1;
    ins->operands[0] = OPCODE_H1(opcode);
    ins->operands[1] = OPCODE_H2(opcode);
  }

  else if (OPCODE_H0(opcode) == 0x8 && OPCODE_H3(opcode) == 0x2) {
    ins->type = OC8_IS_TYPE_8XY2;
    ins->operands[0] = OPCODE_H1(opcode);
    ins->operands[1] = OPCODE_H2(opcode);
  }

  else if (OPCODE_H0(opcode) == 0x8 && OPCODE_H3(opcode) == 0x3) {
    ins->type = OC8_IS_TYPE_8XY3;
    ins->operands[0] = OPCODE_H1(opcode);
    ins->operands[1] = OPCODE_H2(opcode);
  }

  else if (OPCODE_H0(opcode) == 0x8 && OPCODE_H3(opcode) == 0x4) {
    ins->type = OC8_IS_TYPE_8XY4;
    ins->operands[0] = OPCODE_H1(opcode);
    ins->operands[1] = OPCODE_H2(opcode);
  }

  else if (OPCODE_H0(opcode) == 0x8 && OPCODE_H3(opcode) == 0x5) {
    ins->type = OC8_IS_TYPE_8XY5;
    ins->operands[0] = OPCODE_H1(opcode);
    ins->operands[1] = OPCODE_H2(opcode);
  }

  else if (OPCODE_H0(opcode) == 0x8 && OPCODE_H3(opcode) == 0x6) {
    ins->type = OC8_IS_TYPE_8XY6;
    ins->operands[0] = OPCODE_H1(opcode);
    ins->operands[1] = OPCODE_H2(opcode);
  }

  else if (OPCODE_H0(opcode) == 0x8 && OPCODE_H3(opcode) == 0x7) {
    ins->type = OC8_IS_TYPE_8XY7;
    ins->operands[0] = OPCODE_H1(opcode);
    ins->operands[1] = OPCODE_H2(opcode);
  }

  else if (OPCODE_H0(opcode) == 0x8 && OPCODE_H3(opcode) == 0xE) {
    ins->type = OC8_IS_TYPE_8XYE;
    ins->operands[0] = OPCODE_H1(opcode);
    ins->operands[1] = OPCODE_H2(opcode);
  }

  // opdocdes starting by 9
  else if (OPCODE_H0(opcode) == 0x9 && OPCODE_H3(opcode) == 0x0) {
    ins->type = OC8_IS_TYPE_9XY0;
    ins->operands[0] = OPCODE_H1(opcode);
    ins->operands[1] = OPCODE_H2(opcode);
  }

  // opcodes starting by A
  else if (OPCODE_H0(opcode) == 0xA) {
    ins->type = OC8_IS_TYPE_ANNN;
    ins->operands[0] = OPCODE_H123(opcode);
  }

  // opcodes starting by B
  else if (OPCODE_H0(opcode) == 0xB) {
    ins->type = OC8_IS_TYPE_BNNN;
    ins->operands[0] = OPCODE_H123(opcode);
  }

  // opcodes starting by C
  else if (OPCODE_H0(opcode) == 0xC) {
    ins->type = OC8_IS_TYPE_CXNN;
    ins->operands[0] = OPCODE_H1(opcode);
    ins->operands[1] = OPCODE_H23(opcode);
  }

  // opcodes starting by D
  else if (OPCODE_H0(opcode) == 0xD) {
    ins->type = OC8_IS_TYPE_DXYN;
    ins->operands[0] = OPCODE_H1(opcode);
    ins->operands[1] = OPCODE_H2(opcode);
    ins->operands[2] = OPCODE_H3(opcode);
  }

  // opcodes starting by E
  else if (OPCODE_H0(opcode) == 0xE && OPCODE_H23(opcode) == 0x9E) {
    ins->type = OC8_IS_TYPE_EX9E;
    ins->operands[0] = OPCODE_H1(opcode);
  }

  else if (OPCODE_H0(opcode) == 0xE && OPCODE_H23(opcode) == 0xA1) {
    ins->type = OC8_IS_TYPE_EXA1;
    ins->operands[0] = OPCODE_H1(opcode);
  }

  // opcodes starting by F
  else if (OPCODE_H0(opcode) == 0xF && OPCODE_H23(opcode) == 0x07) {
    ins->type = OC8_IS_TYPE_FX07;
    ins->operands[0] = OPCODE_H1(opcode);
  }

  else if (OPCODE_H0(opcode) == 0xF && OPCODE_H23(opcode) == 0x0A) {
    ins->type = OC8_IS_TYPE_FX0A;
    ins->operands[0] = OPCODE_H1(opcode);
  }

  else if (OPCODE_H0(opcode) == 0xF && OPCODE_H23(opcode) == 0x15) {
    ins->type = OC8_IS_TYPE_FX15;
    ins->operands[0] = OPCODE_H1(opcode);
  }

  else if (OPCODE_H0(opcode) == 0xF && OPCODE_H23(opcode) == 0x18) {
    ins->type = OC8_IS_TYPE_FX18;
    ins->operands[0] = OPCODE_H1(opcode);
  }

  else if (OPCODE_H0(opcode) == 0xF && OPCODE_H23(opcode) == 0x1E) {
    ins->type = OC8_IS_TYPE_FX1E;
    ins->operands[0] = OPCODE_H1(opcode);
  }

  else if (OPCODE_H0(opcode) == 0xF && OPCODE_H23(opcode) == 0x29) {
    ins->type = OC8_IS_TYPE_FX29;
    ins->operands[0] = OPCODE_H1(opcode);
  }

  else if (OPCODE_H0(opcode) == 0xF && OPCODE_H23(opcode) == 0x33) {
    ins->type = OC8_IS_TYPE_FX33;
    ins->operands[0] = OPCODE_H1(opcode);
  }

  else if (OPCODE_H0(opcode) == 0xF && OPCODE_H23(opcode) == 0x55) {
    ins->type = OC8_IS_TYPE_FX55;
    ins->operands[0] = OPCODE_H1(opcode);
  }

  else if (OPCODE_H0(opcode) == 0xF && OPCODE_H23(opcode) == 0x65) {
    ins->type = OC8_IS_TYPE_FX65;
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
/// @returns != 0 il failed to encode instruction
int oc8_is_encode_ins(oc8_is_ins_t *ins, char *buf) {
  if (ins->opcode) {
    if (buf)
      memcpy(buf, &ins->opcode, 2);
    return 0;
  }

  uint16_t opcode;

  switch (ins->type) {
    // @TODO encode all instructions
  case OC8_IS_TYPE_0NNN:
    opcode = ins->operands[0];
    break;
  case OC8_IS_TYPE_00E0:
    opcode = 0x00E0;
    break;
  case OC8_IS_TYPE_00EE:
    opcode = 0x00EE;
    break;
  case OC8_IS_TYPE_1NNN:
    opcode = (0x1 << 12) | ins->operands[0];
    break;
  case OC8_IS_TYPE_2NNN:
    opcode = (0x2 << 12) | ins->operands[0];
    break;
  case OC8_IS_TYPE_3XNN:
    opcode = (0x3 << 12) | (ins->operands[0] << 8) | ins->operands[1];
    break;
  case OC8_IS_TYPE_4XNN:
    opcode = (0x4 << 12) | (ins->operands[0] << 8) | ins->operands[1];
    break;
  case OC8_IS_TYPE_5XY0:
    opcode = (0x5 << 12) | (ins->operands[0] << 8) | (ins->operands[1] << 4);
    break;
  case OC8_IS_TYPE_6XNN:
    opcode = (0x6 << 12) | (ins->operands[0] << 8) | ins->operands[1];
    break;
  case OC8_IS_TYPE_7XNN:
    opcode = (0x7 << 12) | (ins->operands[0] << 8) | ins->operands[1];
    break;
  case OC8_IS_TYPE_8XY0:
    opcode = (0x8 << 12) | (ins->operands[0] << 8) | (ins->operands[1] << 4);
    break;

  case OC8_IS_TYPE_8XY1:
    opcode = 0x8001 | (ins->operands[0] << 8) | (ins->operands[1] << 4);
    break;

  case OC8_IS_TYPE_8XY2:
    opcode = 0x8002 | (ins->operands[0] << 8) | (ins->operands[1] << 4);
    break;

  case OC8_IS_TYPE_8XY3:
    opcode = 0x8003 | (ins->operands[0] << 8) | (ins->operands[1] << 4);
    break;

  case OC8_IS_TYPE_8XY4:
    opcode = 0x8004 | (ins->operands[0] << 8) | (ins->operands[1] << 4);
    break;

  case OC8_IS_TYPE_8XY5:
    opcode = 0x8005 | (ins->operands[0] << 8) | (ins->operands[1] << 4);
    break;

  case OC8_IS_TYPE_8XY6:
    opcode = 0x8006 | (ins->operands[0] << 8) | (ins->operands[1] << 4);
    break;

  case OC8_IS_TYPE_8XY7:
    opcode = 0x8007 | (ins->operands[0] << 8) | (ins->operands[1] << 4);
    break;

  case OC8_IS_TYPE_8XYE:
    opcode = 0x800E | (ins->operands[0] << 8) | (ins->operands[1] << 4);
    break;

  case OC8_IS_TYPE_9XY0:
    opcode = 0x9000 | (ins->operands[0] << 8) | (ins->operands[1] << 4);
    break;

  case OC8_IS_TYPE_ANNN:
    opcode = 0xA000 | ins->operands[0];
    break;

  case OC8_IS_TYPE_BNNN:
    opcode = 0xB000 | ins->operands[0];
    break;

  case OC8_IS_TYPE_CXNN:
    opcode = 0xC000 | (ins->operands[0] << 8) | ins->operands[1];
    break;

  case OC8_IS_TYPE_DXYN:
    opcode = 0xD000 | (ins->operands[0] << 8) | (ins->operands[1] << 4) |
             ins->operands[2];
    break;

  case OC8_IS_TYPE_EX9E:
    opcode = 0xE09E | (ins->operands[0] << 8);
    break;

  case OC8_IS_TYPE_EXA1:
    opcode = 0xE0A1 | (ins->operands[0] << 8);
    break;

  case OC8_IS_TYPE_FX07:
    opcode = 0xF007 | (ins->operands[0] << 8);
    break;

  case OC8_IS_TYPE_FX0A:
    opcode = 0xF00A | (ins->operands[0] << 8);
    break;

  case OC8_IS_TYPE_FX15:
    opcode = 0xF015 | (ins->operands[0] << 8);
    break;

  case OC8_IS_TYPE_FX18:
    opcode = 0xF018 | (ins->operands[0] << 8);
    break;

  case OC8_IS_TYPE_FX1E:
    opcode = 0xF01E | (ins->operands[0] << 8);
    break;

  case OC8_IS_TYPE_FX29:
    opcode = 0xF029 | (ins->operands[0] << 8);
    break;

  case OC8_IS_TYPE_FX33:
    opcode = 0xF033 | (ins->operands[0] << 8);
    break;

  case OC8_IS_TYPE_FX55:
    opcode = 0xF055 | (ins->operands[0] << 8);
    break;

  case OC8_IS_TYPE_FX65:
    opcode = 0xF065 | (ins->operands[0] << 8);
    break;

  default:
    return 1;
  }

  ins->opcode = OPCODE_SWAP(opcode);
  if (buf) {
    memcpy(buf, &ins->opcode, 2);
  }
  return 0;
}
