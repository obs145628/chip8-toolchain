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

TEST_CASE("encode Ins 0: 0NNN", "") {
  oc8_is_ins_t ins;
  decode_op(&ins, "0B9D");

  auto op = ins.opcode;
  ins.opcode = 0;
  REQUIRE(oc8_is_encode_ins(&ins, NULL) == 0);
  REQUIRE(ins.opcode == op);
}

TEST_CASE("encode Ins 1: 00E0", "") {
  oc8_is_ins_t ins;
  decode_op(&ins, "00E0");

  auto op = ins.opcode;
  ins.opcode = 0;
  REQUIRE(oc8_is_encode_ins(&ins, NULL) == 0);
  REQUIRE(ins.opcode == op);
}

TEST_CASE("encode Ins 2: 00EE", "") {
  oc8_is_ins_t ins;
  decode_op(&ins, "00EE");

  auto op = ins.opcode;
  ins.opcode = 0;
  REQUIRE(oc8_is_encode_ins(&ins, NULL) == 0);
  REQUIRE(ins.opcode == op);
}

TEST_CASE("encode Ins 3: 1NNN", "") {
  oc8_is_ins_t ins;
  decode_op(&ins, "17BE");

  auto op = ins.opcode;
  ins.opcode = 0;
  REQUIRE(oc8_is_encode_ins(&ins, NULL) == 0);
  REQUIRE(ins.opcode == op);
}

TEST_CASE("encode Ins 4: 2NNN", "") {
  oc8_is_ins_t ins;
  decode_op(&ins, "20F8");

  auto op = ins.opcode;
  ins.opcode = 0;
  REQUIRE(oc8_is_encode_ins(&ins, NULL) == 0);
  REQUIRE(ins.opcode == op);
}

TEST_CASE("encode Ins 5: 3XNN", "") {
  oc8_is_ins_t ins;
  decode_op(&ins, "37A8");

  auto op = ins.opcode;
  ins.opcode = 0;
  REQUIRE(oc8_is_encode_ins(&ins, NULL) == 0);
  REQUIRE(ins.opcode == op);
}

TEST_CASE("encode Ins 6: 4XNN", "") {
  oc8_is_ins_t ins;
  decode_op(&ins, "4281");

  auto op = ins.opcode;
  ins.opcode = 0;
  REQUIRE(oc8_is_encode_ins(&ins, NULL) == 0);
  REQUIRE(ins.opcode == op);
}

TEST_CASE("encode Ins 7: 5XY0", "") {
  oc8_is_ins_t ins;
  decode_op(&ins, "5B70");

  auto op = ins.opcode;
  ins.opcode = 0;
  REQUIRE(oc8_is_encode_ins(&ins, NULL) == 0);
  REQUIRE(ins.opcode == op);
}

TEST_CASE("encode Ins 8: 6XNN", "") {
  oc8_is_ins_t ins;
  decode_op(&ins, "6E2C");

  auto op = ins.opcode;
  ins.opcode = 0;
  REQUIRE(oc8_is_encode_ins(&ins, NULL) == 0);
  REQUIRE(ins.opcode == op);
}

TEST_CASE("encode Ins 9: 7XNN", "") {
  oc8_is_ins_t ins;
  decode_op(&ins, "7FED");

  auto op = ins.opcode;
  ins.opcode = 0;
  REQUIRE(oc8_is_encode_ins(&ins, NULL) == 0);
  REQUIRE(ins.opcode == op);
}

TEST_CASE("encode Ins 10: 8XY0", "") {
  oc8_is_ins_t ins;
  decode_op(&ins, "84E0");

  auto op = ins.opcode;
  ins.opcode = 0;
  REQUIRE(oc8_is_encode_ins(&ins, NULL) == 0);
  REQUIRE(ins.opcode == op);
}

TEST_CASE("encode Ins 11: 8XY1", "") {
  oc8_is_ins_t ins;
  decode_op(&ins, "84E1");

  auto op = ins.opcode;
  ins.opcode = 0;
  REQUIRE(oc8_is_encode_ins(&ins, NULL) == 0);
  REQUIRE(ins.opcode == op);
}

TEST_CASE("encode Ins 12: 8XY2", "") {
  oc8_is_ins_t ins;
  decode_op(&ins, "84E2");

  auto op = ins.opcode;
  ins.opcode = 0;
  REQUIRE(oc8_is_encode_ins(&ins, NULL) == 0);
  REQUIRE(ins.opcode == op);
}

TEST_CASE("encode Ins 13: 8XY3", "") {
  oc8_is_ins_t ins;
  decode_op(&ins, "84E3");

  auto op = ins.opcode;
  ins.opcode = 0;
  REQUIRE(oc8_is_encode_ins(&ins, NULL) == 0);
  REQUIRE(ins.opcode == op);
}

TEST_CASE("encode Ins 14: 8XY4", "") {
  oc8_is_ins_t ins;
  decode_op(&ins, "87E4");

  auto op = ins.opcode;
  ins.opcode = 0;
  REQUIRE(oc8_is_encode_ins(&ins, NULL) == 0);
  REQUIRE(ins.opcode == op);
}

TEST_CASE("encode Ins 15: 8XY5", "") {
  oc8_is_ins_t ins;
  decode_op(&ins, "87E5");

  auto op = ins.opcode;
  ins.opcode = 0;
  REQUIRE(oc8_is_encode_ins(&ins, NULL) == 0);
  REQUIRE(ins.opcode == op);
}

TEST_CASE("encode Ins 16: 8XY6", "") {
  oc8_is_ins_t ins;
  decode_op(&ins, "87E6");

  auto op = ins.opcode;
  ins.opcode = 0;
  REQUIRE(oc8_is_encode_ins(&ins, NULL) == 0);
  REQUIRE(ins.opcode == op);
}

TEST_CASE("encode Ins 17: 8XY7", "") {
  oc8_is_ins_t ins;
  decode_op(&ins, "8B47");

  auto op = ins.opcode;
  ins.opcode = 0;
  REQUIRE(oc8_is_encode_ins(&ins, NULL) == 0);
  REQUIRE(ins.opcode == op);
}

TEST_CASE("encode Ins 18: 8XYE", "") {
  oc8_is_ins_t ins;
  decode_op(&ins, "87CE");

  auto op = ins.opcode;
  ins.opcode = 0;
  REQUIRE(oc8_is_encode_ins(&ins, NULL) == 0);
  REQUIRE(ins.opcode == op);
}

TEST_CASE("encode Ins 19: 9XY0", "") {
  oc8_is_ins_t ins;
  decode_op(&ins, "9480");

  auto op = ins.opcode;
  ins.opcode = 0;
  REQUIRE(oc8_is_encode_ins(&ins, NULL) == 0);
  REQUIRE(ins.opcode == op);
}

TEST_CASE("encode Ins 20: ANNN", "") {
  oc8_is_ins_t ins;
  decode_op(&ins, "A3BE");

  auto op = ins.opcode;
  ins.opcode = 0;
  REQUIRE(oc8_is_encode_ins(&ins, NULL) == 0);
  REQUIRE(ins.opcode == op);
}

TEST_CASE("encode Ins 21: BNNN", "") {
  oc8_is_ins_t ins;
  decode_op(&ins, "B3CE");

  auto op = ins.opcode;
  ins.opcode = 0;
  REQUIRE(oc8_is_encode_ins(&ins, NULL) == 0);
  REQUIRE(ins.opcode == op);
}

TEST_CASE("encode encode Ins 22: CXNN", "") {
  oc8_is_ins_t ins;
  decode_op(&ins, "C38E");

  auto op = ins.opcode;
  ins.opcode = 0;
  REQUIRE(oc8_is_encode_ins(&ins, NULL) == 0);
  REQUIRE(ins.opcode == op);
}

TEST_CASE("encode Ins 23: DXYN", "") {
  oc8_is_ins_t ins;
  decode_op(&ins, "D81C");

  auto op = ins.opcode;
  ins.opcode = 0;
  REQUIRE(oc8_is_encode_ins(&ins, NULL) == 0);
  REQUIRE(ins.opcode == op);
}

TEST_CASE("encode Ins 24: EX9E", "") {
  oc8_is_ins_t ins;
  decode_op(&ins, "E69E");

  auto op = ins.opcode;
  ins.opcode = 0;
  REQUIRE(oc8_is_encode_ins(&ins, NULL) == 0);
  REQUIRE(ins.opcode == op);
}

TEST_CASE("encode Ins 25: EXA1", "") {
  oc8_is_ins_t ins;
  decode_op(&ins, "E6A1");

  auto op = ins.opcode;
  ins.opcode = 0;
  REQUIRE(oc8_is_encode_ins(&ins, NULL) == 0);
  REQUIRE(ins.opcode == op);
}

TEST_CASE("encode Ins 26: FX07", "") {
  oc8_is_ins_t ins;
  decode_op(&ins, "F607");

  auto op = ins.opcode;
  ins.opcode = 0;
  REQUIRE(oc8_is_encode_ins(&ins, NULL) == 0);
  REQUIRE(ins.opcode == op);
}

TEST_CASE("encode Ins 27: FX0A", "") {
  oc8_is_ins_t ins;
  decode_op(&ins, "F60A");

  auto op = ins.opcode;
  ins.opcode = 0;
  REQUIRE(oc8_is_encode_ins(&ins, NULL) == 0);
  REQUIRE(ins.opcode == op);
}

TEST_CASE("encode Ins 28: FX15", "") {
  oc8_is_ins_t ins;
  decode_op(&ins, "F615");

  auto op = ins.opcode;
  ins.opcode = 0;
  REQUIRE(oc8_is_encode_ins(&ins, NULL) == 0);
  REQUIRE(ins.opcode == op);
}

TEST_CASE("encode Ins 29: FX18", "") {
  oc8_is_ins_t ins;
  decode_op(&ins, "F618");

  auto op = ins.opcode;
  ins.opcode = 0;
  REQUIRE(oc8_is_encode_ins(&ins, NULL) == 0);
  REQUIRE(ins.opcode == op);
}

TEST_CASE("encode Ins 30: FX1E", "") {
  oc8_is_ins_t ins;
  decode_op(&ins, "F61E");

  auto op = ins.opcode;
  ins.opcode = 0;
  REQUIRE(oc8_is_encode_ins(&ins, NULL) == 0);
  REQUIRE(ins.opcode == op);
}

TEST_CASE("encode Ins 31: FX29", "") {
  oc8_is_ins_t ins;
  decode_op(&ins, "F629");

  auto op = ins.opcode;
  ins.opcode = 0;
  REQUIRE(oc8_is_encode_ins(&ins, NULL) == 0);
  REQUIRE(ins.opcode == op);
}

TEST_CASE("encode Ins 32: FX33", "") {
  oc8_is_ins_t ins;
  decode_op(&ins, "F633");

  auto op = ins.opcode;
  ins.opcode = 0;
  REQUIRE(oc8_is_encode_ins(&ins, NULL) == 0);
  REQUIRE(ins.opcode == op);
}

TEST_CASE("encode Ins 33: FX55", "") {
  oc8_is_ins_t ins;
  decode_op(&ins, "F655");

  auto op = ins.opcode;
  ins.opcode = 0;
  REQUIRE(oc8_is_encode_ins(&ins, NULL) == 0);
  REQUIRE(ins.opcode == op);
}

TEST_CASE("encode Ins 34: FX65", "") {
  oc8_is_ins_t ins;
  decode_op(&ins, "FA65");

  auto op = ins.opcode;
  ins.opcode = 0;
  REQUIRE(oc8_is_encode_ins(&ins, NULL) == 0);
  REQUIRE(ins.opcode == op);
}
