#include "oc8_emu/oc8_emu.h"
#include "oc8_is/oc8_is.h"
#include <catch2/catch.hpp>
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

#define OPCODE_SWAP(X) ((X << 8) | (X >> 8))

namespace {

class EnvBuilder {
public:
  static EnvBuilder start() { return EnvBuilder{}; }

  // Add opcodes, separted by spaces
  EnvBuilder &opcodes(std::string str) {
    for (;;) {
      auto spos = str.find(' ');
      std::string ins = spos == std::string::npos ? str : str.substr(0, spos);

      uint16_t op16 = std::stoi(ins, nullptr, 16);
      // need to swap to big-endian
      op16 = OPCODE_SWAP(op16);
      _code.push_back(op16);

      if (spos == std::string::npos)
        break;
      str = str.substr(spos + 1);
    }

    return *this;
  }

  EnvBuilder &fill_screen() {
    std::memset(g_oc8_emu_screen, 0xFF, sizeof(g_oc8_emu_screen));
    return *this;
  }

  void end(std::size_t nb_steps = 1) {
    oc8_emu_load_rom((const void *)&_code[0], _code.size() * 2);
    for (std::size_t i = 0; i < nb_steps; ++i)
      oc8_emu_cpu_step();
  }

private:
  EnvBuilder() { oc8_emu_init(); }
  std::vector<uint16_t> _code;
};

} // namespace

TEST_CASE("Ins 1: 00E0", "") {
  EnvBuilder::start().fill_screen().opcodes("00E0").end();
  for (std::size_t i = 0; i < sizeof(g_oc8_emu_screen); ++i)
    REQUIRE(g_oc8_emu_screen[i] == 0);
}
