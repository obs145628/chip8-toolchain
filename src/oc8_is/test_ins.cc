#include <catch2/catch.hpp>
#include <cstdint>
#include <string>

#include "oc8_is/ins.h"

#define OPCODE_SWAP(X) ((X << 8) | (X >> 8))

namespace {

void decode_op(oc8_is_ins_t *ins, const std::string &opcode) {
  uint16_t op16 = std::stoi(opcode, nullptr, 16);
  // need to swap to big-endian
  op16 = OPCODE_SWAP(op16);
  oc8_is_decode_ins(ins, (const char *)&op16);
}

} // namespace

TEST_CASE("Ins 0: 0NNN", "") {
  oc8_is_ins_t ins;
  decode_op(&ins, "0B9D");
  REQUIRE(ins.type == OC8_IS_TYPE_SYS);
  REQUIRE(ins.operands[0] == 0xB9D);
}

TEST_CASE("Ins 1: 00E0", "") {
  oc8_is_ins_t ins;
  decode_op(&ins, "00E0");
  REQUIRE(ins.type == OC8_IS_TYPE_CLS);
}

TEST_CASE("Ins 2: 00EE", "") {
  oc8_is_ins_t ins;
  decode_op(&ins, "00EE");
  REQUIRE(ins.type == OC8_IS_TYPE_RET);
}

TEST_CASE("Ins 3: 1NNN", "") {
  oc8_is_ins_t ins;
  decode_op(&ins, "17BE");
  REQUIRE(ins.type == OC8_IS_TYPE_JMP);
  REQUIRE(ins.operands[0] == 0x7BE);
}

TEST_CASE("Ins 4: 2NNN", "") {
  oc8_is_ins_t ins;
  decode_op(&ins, "20F8");
  REQUIRE(ins.type == OC8_IS_TYPE_CALL);
  REQUIRE(ins.operands[0] == 0x0F8);
}

TEST_CASE("Ins 5: 3XNN", "") {
  oc8_is_ins_t ins;
  decode_op(&ins, "37A8");
  REQUIRE(ins.type == OC8_IS_TYPE_SKIP_E_IMM);
  REQUIRE(ins.operands[0] == 7);
  REQUIRE(ins.operands[1] == 0xA8);
}

TEST_CASE("Ins 6: 4XNN", "") {
  oc8_is_ins_t ins;
  decode_op(&ins, "4281");
  REQUIRE(ins.type == OC8_IS_TYPE_SKIP_NE_IMM);
  REQUIRE(ins.operands[0] == 2);
  REQUIRE(ins.operands[1] == 0x81);
}

TEST_CASE("Ins 7: 5XY0", "") {
  oc8_is_ins_t ins;
  decode_op(&ins, "5B70");
  REQUIRE(ins.type == OC8_IS_TYPE_SKIP_E_REG);
  REQUIRE(ins.operands[0] == 0xB);
  REQUIRE(ins.operands[1] == 0x7);
}

TEST_CASE("Ins 8: 6XNN", "") {
  oc8_is_ins_t ins;
  decode_op(&ins, "6E2C");
  REQUIRE(ins.type == OC8_IS_TYPE_LD_IMM);
  REQUIRE(ins.operands[0] == 0xE);
  REQUIRE(ins.operands[1] == 0x2c);
}

TEST_CASE("Ins 9: 7XNN", "") {
  oc8_is_ins_t ins;
  decode_op(&ins, "7FED");
  REQUIRE(ins.type == OC8_IS_TYPE_ADD_IMM);
  REQUIRE(ins.operands[0] == 0xF);
  REQUIRE(ins.operands[1] == 0xED);
}

TEST_CASE("Ins 10: 8XY0", "") {
  oc8_is_ins_t ins;
  decode_op(&ins, "84E0");
  REQUIRE(ins.type == OC8_IS_TYPE_LD_REG);
  REQUIRE(ins.operands[0] == 0x4);
  REQUIRE(ins.operands[1] == 0xE);
}

TEST_CASE("Ins 11: 8XY1", "") {
  oc8_is_ins_t ins;
  decode_op(&ins, "84E1");
  REQUIRE(ins.type == OC8_IS_TYPE_OR);
  REQUIRE(ins.operands[0] == 0x4);
  REQUIRE(ins.operands[1] == 0xE);
}

TEST_CASE("Ins 12: 8XY2", "") {
  oc8_is_ins_t ins;
  decode_op(&ins, "84E2");
  REQUIRE(ins.type == OC8_IS_TYPE_AND);
  REQUIRE(ins.operands[0] == 0x4);
  REQUIRE(ins.operands[1] == 0xE);
}

TEST_CASE("Ins 13: 8XY3", "") {
  oc8_is_ins_t ins;
  decode_op(&ins, "84E3");
  REQUIRE(ins.type == OC8_IS_TYPE_XOR);
  REQUIRE(ins.operands[0] == 0x4);
  REQUIRE(ins.operands[1] == 0xE);
}

TEST_CASE("Ins 14: 8XY4", "") {
  oc8_is_ins_t ins;
  decode_op(&ins, "87E4");
  REQUIRE(ins.type == OC8_IS_TYPE_ADD_REG);
  REQUIRE(ins.operands[0] == 0x7);
  REQUIRE(ins.operands[1] == 0xE);
}

TEST_CASE("Ins 16: 8XY5", "") {
  oc8_is_ins_t ins;
  decode_op(&ins, "87E5");
  REQUIRE(ins.type == OC8_IS_TYPE_SUB);
  REQUIRE(ins.operands[0] == 0x7);
  REQUIRE(ins.operands[1] == 0xE);
}

TEST_CASE("Ins 16: 8XY6", "") {
  oc8_is_ins_t ins;
  decode_op(&ins, "87E6");
  REQUIRE(ins.type == OC8_IS_TYPE_SHR);
  REQUIRE(ins.operands[0] == 0x7);
  REQUIRE(ins.operands[1] == 0xE);
}

TEST_CASE("Ins 17: 8XY7", "") {
  oc8_is_ins_t ins;
  decode_op(&ins, "8B47");
  REQUIRE(ins.type == OC8_IS_TYPE_SUBN);
  REQUIRE(ins.operands[0] == 0xB);
  REQUIRE(ins.operands[1] == 0x4);
}

TEST_CASE("Ins 18: 8XYE", "") {
  oc8_is_ins_t ins;
  decode_op(&ins, "87CE");
  REQUIRE(ins.type == OC8_IS_TYPE_SHL);
  REQUIRE(ins.operands[0] == 0x7);
  REQUIRE(ins.operands[1] == 0xC);
}

TEST_CASE("Ins 19: 9XY0", "") {
  oc8_is_ins_t ins;
  decode_op(&ins, "9480");
  REQUIRE(ins.type == OC8_IS_TYPE_SKIP_NE_REG);
  REQUIRE(ins.operands[0] == 0x4);
  REQUIRE(ins.operands[1] == 0x8);
}

TEST_CASE("Ins 20: ANNN", "") {
  oc8_is_ins_t ins;
  decode_op(&ins, "A3BE");
  REQUIRE(ins.type == OC8_IS_TYPE_ST_I);
  REQUIRE(ins.operands[0] == 0x3BE);
}

TEST_CASE("Ins 21: BNNN", "") {
  oc8_is_ins_t ins;
  decode_op(&ins, "B3CE");
  REQUIRE(ins.type == OC8_IS_TYPE_JMP_V0);
  REQUIRE(ins.operands[0] == 0x3CE);
}
