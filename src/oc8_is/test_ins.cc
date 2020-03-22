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

  REQUIRE(2 + 2 == 4);
}
