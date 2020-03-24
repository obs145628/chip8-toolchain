#include <catch2/catch.hpp>
#include <cstdint>
#include <cstring>
#include <map>
#include <string>
#include <vector>

#include "oc8_emu/oc8_emu.h"
#include "oc8_is/oc8_is.h"
#include "test_utils.hh"

static void check_sprite(size_t x0, size_t y0, const char *data) {
  size_t w = 8;
  size_t h = strlen(data) / w;

  for (size_t x = 0; x < w; ++x)
    for (size_t y = 0; y < h; ++y) {
      int rval = data[y * 8 + x] == '1';
      REQUIRE(rval == oc8_emu_screen_get_pix(x + x0, y + y0));
    }
}

TEST_CASE("Ins 1: 00E0", "") {
  EnvBuilder::get().fill_screen().opcodes("00E0").run();
  REQUIRE(g_oc8_emu_cpu.reg_pc == 0x202);
  for (std::size_t i = 0; i < sizeof(g_oc8_emu_screen); ++i)
    REQUIRE(g_oc8_emu_screen[i] == 0);
}

TEST_CASE("Ins 2: 00EE", "") {
  EnvBuilder::get().opcodes("2204 0000 00EE").run(0);
  oc8_emu_cpu_step();
  REQUIRE(g_oc8_emu_cpu.reg_pc == 0x204);
  REQUIRE(g_oc8_emu_cpu.reg_sp == 1);
  REQUIRE(g_oc8_emu_mem.stack[0] == 0x202);
  oc8_emu_cpu_step();
  REQUIRE(g_oc8_emu_cpu.reg_pc == 0x202);
  REQUIRE(g_oc8_emu_cpu.reg_sp == 0);
}

TEST_CASE("Ins 3: 1NNN", "") {
  EnvBuilder::get().opcodes("1578").run();
  REQUIRE(g_oc8_emu_cpu.reg_pc == 0x578);
}

TEST_CASE("Ins 4: 2NNN", "") {
  EnvBuilder::get().opcodes("2578").run();
  REQUIRE(g_oc8_emu_cpu.reg_pc == 0x578);
  REQUIRE(g_oc8_emu_cpu.reg_sp == 1);
  REQUIRE(g_oc8_emu_mem.stack[0] == 0x202);
}

TEST_CASE("Ins 5: 3XNN skip", "") {
  EnvBuilder::get().reg(4, 0x2B).opcodes("342B").run();
  REQUIRE(g_oc8_emu_cpu.reg_pc == 0x204);
}

TEST_CASE("Ins 5: 3XNN doesn't skip", "") {
  EnvBuilder::get().reg(4, 0x2D).opcodes("342B").run();
  REQUIRE(g_oc8_emu_cpu.reg_pc == 0x202);
}

TEST_CASE("Ins 6: 4XNN skip", "") {
  EnvBuilder::get().reg(7, 0x2B).opcodes("472C").run();
  REQUIRE(g_oc8_emu_cpu.reg_pc == 0x204);
}

TEST_CASE("Ins 6: 4XNN doesn't skip", "") {
  EnvBuilder::get().reg(7, 0x2B).opcodes("472B").run();
  REQUIRE(g_oc8_emu_cpu.reg_pc == 0x202);
}

TEST_CASE("Ins 7: 5XY0 skip", "") {
  EnvBuilder::get().reg({{7, 0x2B}, {8, 0x2B}}).opcodes("5780").run();
  REQUIRE(g_oc8_emu_cpu.reg_pc == 0x204);
}

TEST_CASE("Ins 7: 5XY0 doesn't skip", "") {
  EnvBuilder::get().reg({{7, 0x2B}, {8, 0x2D}}).opcodes("5780").run();
  REQUIRE(g_oc8_emu_cpu.reg_pc == 0x202);
}

TEST_CASE("Ins 8: 6XNN", "") {
  EnvBuilder::get().opcodes("678C").run();
  REQUIRE(g_oc8_emu_cpu.regs_data[7] == 0x8c);
  REQUIRE(g_oc8_emu_cpu.reg_pc == 0x202);
}

TEST_CASE("Ins 9: 7XNN", "") {
  EnvBuilder::get().reg(3, 12).opcodes("738C").run();
  REQUIRE(g_oc8_emu_cpu.regs_data[3] == 152);
  REQUIRE(g_oc8_emu_cpu.reg_pc == 0x202);
}

TEST_CASE("Ins 10: 8XY0", "") {
  EnvBuilder::get().reg({{3, 12}, {5, 7}}).opcodes("8350").run();
  REQUIRE(g_oc8_emu_cpu.regs_data[3] == 7);
  REQUIRE(g_oc8_emu_cpu.regs_data[5] == 7);
  REQUIRE(g_oc8_emu_cpu.reg_pc == 0x202);
}

TEST_CASE("Ins 11: 8XY1", "") {
  EnvBuilder::get().reg({{3, 167}, {5, 96}}).opcodes("8351").run();
  REQUIRE(g_oc8_emu_cpu.regs_data[3] == 231);
  REQUIRE(g_oc8_emu_cpu.regs_data[5] == 96);
  REQUIRE(g_oc8_emu_cpu.reg_pc == 0x202);
}

TEST_CASE("Ins 12: 8XY2", "") {
  EnvBuilder::get().reg({{3, 167}, {5, 96}}).opcodes("8352").run();
  REQUIRE(g_oc8_emu_cpu.regs_data[3] == 32);
  REQUIRE(g_oc8_emu_cpu.regs_data[5] == 96);
  REQUIRE(g_oc8_emu_cpu.reg_pc == 0x202);
}

TEST_CASE("Ins 13: 8XY3", "") {
  EnvBuilder::get().reg({{3, 167}, {5, 96}}).opcodes("8353").run();
  REQUIRE(g_oc8_emu_cpu.regs_data[3] == 199);
  REQUIRE(g_oc8_emu_cpu.regs_data[5] == 96);
  REQUIRE(g_oc8_emu_cpu.reg_pc == 0x202);
}

TEST_CASE("Ins 14: 8XY4", "") {
  EnvBuilder::get().reg({{3, 23}, {5, 96}}).opcodes("8354").run();
  REQUIRE(g_oc8_emu_cpu.regs_data[3] == 119);
  REQUIRE(g_oc8_emu_cpu.regs_data[5] == 96);
  REQUIRE(g_oc8_emu_cpu.regs_data[15] == 0);
  REQUIRE(g_oc8_emu_cpu.reg_pc == 0x202);
}

TEST_CASE("Ins 14: 8XY4 carry", "") {
  EnvBuilder::get().reg({{3, 233}, {5, 96}}).opcodes("8354").run();
  REQUIRE(g_oc8_emu_cpu.regs_data[3] == 73);
  REQUIRE(g_oc8_emu_cpu.regs_data[5] == 96);
  REQUIRE(g_oc8_emu_cpu.regs_data[15] == 1);
  REQUIRE(g_oc8_emu_cpu.reg_pc == 0x202);
}

TEST_CASE("Ins 15: 8XY5", "") {
  EnvBuilder::get().reg({{3, 108}, {5, 96}}).opcodes("8355").run();
  REQUIRE(g_oc8_emu_cpu.regs_data[3] == 12);
  REQUIRE(g_oc8_emu_cpu.regs_data[5] == 96);
  REQUIRE(g_oc8_emu_cpu.regs_data[15] == 1);
  REQUIRE(g_oc8_emu_cpu.reg_pc == 0x202);
}

TEST_CASE("Ins 15: 8XY5 borrow", "") {
  EnvBuilder::get().reg({{3, 17}, {5, 96}}).opcodes("8355").run();
  REQUIRE(g_oc8_emu_cpu.regs_data[3] == 177);
  REQUIRE(g_oc8_emu_cpu.regs_data[5] == 96);
  REQUIRE(g_oc8_emu_cpu.regs_data[15] == 0);
  REQUIRE(g_oc8_emu_cpu.reg_pc == 0x202);
}

TEST_CASE("Ins 16: 8XY6", "") {
  EnvBuilder::get().reg({{3, 78}, {5, 96}}).opcodes("8356").run();
  REQUIRE(g_oc8_emu_cpu.regs_data[3] == 48);
  REQUIRE(g_oc8_emu_cpu.regs_data[5] == 96);
  REQUIRE(g_oc8_emu_cpu.regs_data[15] == 0);
  REQUIRE(g_oc8_emu_cpu.reg_pc == 0x202);
}

TEST_CASE("Ins 16: 8XY6 vf set", "") {
  EnvBuilder::get().reg({{3, 78}, {5, 201}}).opcodes("8356").run();
  REQUIRE(g_oc8_emu_cpu.regs_data[3] == 100);
  REQUIRE(g_oc8_emu_cpu.regs_data[5] == 201);
  REQUIRE(g_oc8_emu_cpu.regs_data[15] == 1);
  REQUIRE(g_oc8_emu_cpu.reg_pc == 0x202);
}

TEST_CASE("Ins 17: 8XY7", "") {
  EnvBuilder::get().reg({{3, 18}, {5, 96}}).opcodes("8357").run();
  REQUIRE(g_oc8_emu_cpu.regs_data[3] == 78);
  REQUIRE(g_oc8_emu_cpu.regs_data[5] == 96);
  REQUIRE(g_oc8_emu_cpu.regs_data[15] == 1);
  REQUIRE(g_oc8_emu_cpu.reg_pc == 0x202);
}

TEST_CASE("Ins 17: 8XY7 borrow", "") {
  EnvBuilder::get().reg({{3, 108}, {5, 96}}).opcodes("8357").run();
  REQUIRE(g_oc8_emu_cpu.regs_data[3] == 244);
  REQUIRE(g_oc8_emu_cpu.regs_data[5] == 96);
  REQUIRE(g_oc8_emu_cpu.regs_data[15] == 0);
  REQUIRE(g_oc8_emu_cpu.reg_pc == 0x202);
}

TEST_CASE("Ins 18: 8XYE", "") {
  EnvBuilder::get().reg({{3, 78}, {5, 95}}).opcodes("835E").run();
  REQUIRE(g_oc8_emu_cpu.regs_data[3] == 190);
  REQUIRE(g_oc8_emu_cpu.regs_data[5] == 95);
  REQUIRE(g_oc8_emu_cpu.regs_data[15] == 0);
  REQUIRE(g_oc8_emu_cpu.reg_pc == 0x202);
}

TEST_CASE("Ins 18: 8XYE vf set", "") {
  EnvBuilder::get().reg({{3, 78}, {5, 143}}).opcodes("835E").run();
  REQUIRE(g_oc8_emu_cpu.regs_data[3] == 30);
  REQUIRE(g_oc8_emu_cpu.regs_data[5] == 143);
  REQUIRE(g_oc8_emu_cpu.regs_data[15] == 1);
  REQUIRE(g_oc8_emu_cpu.reg_pc == 0x202);
}

TEST_CASE("Ins 19: 9XY0 skip", "") {
  EnvBuilder::get().reg({{3, 78}, {5, 79}}).opcodes("9350").run();
  REQUIRE(g_oc8_emu_cpu.reg_pc == 0x204);
}

TEST_CASE("Ins 19: 9XY0 don't skip", "") {
  EnvBuilder::get().reg({{3, 78}, {5, 78}}).opcodes("9350").run();
  REQUIRE(g_oc8_emu_cpu.reg_pc == 0x202);
}

TEST_CASE("Ins 20: ANNN", "") {
  EnvBuilder::get().reg_i(123).opcodes("A7B4").run();
  REQUIRE(g_oc8_emu_cpu.reg_i == 0x7B4);
  REQUIRE(g_oc8_emu_cpu.reg_pc == 0x202);
}

TEST_CASE("Ins 21: BNNN", "") {
  EnvBuilder::get().reg(0, 0x45).opcodes("B7B4").run();
  REQUIRE(g_oc8_emu_cpu.reg_pc == 2041);
}

TEST_CASE("Ins 22: CXNN", "") {
  EnvBuilder::get().reg(7, 88).seed(19).opcodes("C7B4").run();
  REQUIRE(g_oc8_emu_cpu.regs_data[7] == 68);
  REQUIRE(g_oc8_emu_cpu.reg_pc == 0x202);
}

TEST_CASE("Ins 23: DXYN", "") {

  const char *my_sp = "..111..1"
                      "...11.11"
                      "....1..."
                      "1......1";

  EnvBuilder::get()
      .reg({{0, 4}, {1, 7}})
      .sprite(2500, my_sp)
      .reg_i(2500)
      .opcodes("D014")
      .run();
  REQUIRE(g_oc8_emu_cpu.reg_pc == 0x202);
  REQUIRE(g_oc8_emu_cpu.regs_data[15] == 0);

  check_sprite(4, 7, my_sp);
}

TEST_CASE("Ins 24: EX9E skip", "") {
  EnvBuilder::get().reg(2, 6).keyp(6).opcodes("E29E").run();
  REQUIRE(g_oc8_emu_cpu.reg_pc == 0x204);
}

TEST_CASE("Ins 24: EX9E doesn't skip", "") {
  EnvBuilder::get().reg(2, 6).keyp(5).opcodes("E29E").run();
  REQUIRE(g_oc8_emu_cpu.reg_pc == 0x202);
}

TEST_CASE("Ins 25: EXA1 skip", "") {
  EnvBuilder::get().reg(2, 6).keyp(5).opcodes("E2A1").run();
  REQUIRE(g_oc8_emu_cpu.reg_pc == 0x204);
}

TEST_CASE("Ins 25: EXA1 doesn't skip", "") {
  EnvBuilder::get().reg(2, 6).keyp(6).opcodes("E2A1").run();
  REQUIRE(g_oc8_emu_cpu.reg_pc == 0x202);
}

TEST_CASE("Ins 26: FX07", "") {
  EnvBuilder::get().reg(3, 6).reg_dt(67).opcodes("F307").run();
  REQUIRE(g_oc8_emu_cpu.regs_data[3] == 67);
  REQUIRE(g_oc8_emu_cpu.reg_pc == 0x202);
}

TEST_CASE("Ins 27: FX0A not wait", "") {
  EnvBuilder::get().reg(2, 55).keyp(6).opcodes("F20A").run();
  REQUIRE(g_oc8_emu_cpu.block_waitq == false);
  REQUIRE(g_oc8_emu_cpu.regs_data[2] == 6);
  REQUIRE(g_oc8_emu_cpu.reg_pc == 0x202);
}

TEST_CASE("Ins 27: FX0A wait", "") {
  EnvBuilder::get().reg(2, 55).opcodes("F20A").run();
  REQUIRE(g_oc8_emu_cpu.block_waitq == true);
  REQUIRE(g_oc8_emu_cpu.reg_pc == 0x200);
  REQUIRE(g_oc8_emu_cpu.regs_data[2] == 55);

  oc8_emu_cpu_step();
  REQUIRE(g_oc8_emu_cpu.block_waitq == true);
  REQUIRE(g_oc8_emu_cpu.reg_pc == 0x200);
  REQUIRE(g_oc8_emu_cpu.regs_data[2] == 55);

  g_oc8_emu_keypad[8] = 1;
  oc8_emu_cpu_step();
  REQUIRE(g_oc8_emu_cpu.block_waitq == false);
  REQUIRE(g_oc8_emu_cpu.reg_pc == 0x202);
  REQUIRE(g_oc8_emu_cpu.regs_data[2] == 8);
}

TEST_CASE("Ins 28: FX15", "") {
  EnvBuilder::get().reg_dt(10).reg(2, 55).opcodes("F215").run();
  REQUIRE(g_oc8_emu_cpu.reg_dt == 55);
  REQUIRE(g_oc8_emu_cpu.reg_pc == 0x202);
}

TEST_CASE("Ins 29: FX18", "") {
  EnvBuilder::get().reg_st(10).reg(2, 55).opcodes("F218").run();
  REQUIRE(g_oc8_emu_cpu.reg_st == 55);
  REQUIRE(g_oc8_emu_cpu.reg_pc == 0x202);
}

TEST_CASE("Ins 30: FX1E", "") {
  EnvBuilder::get().reg_i(76).reg(2, 58).opcodes("F21E").run();
  REQUIRE(g_oc8_emu_cpu.reg_i == 134);
  REQUIRE(g_oc8_emu_cpu.reg_pc == 0x202);
}

TEST_CASE("Ins 31: FX29", "") {
  const char *sp4 = "1..1...."
                    "1..1...."
                    "1111...."
                    "...1...."
                    "...1....";

  EnvBuilder::get().reg({{0, 6}, {1, 10}, {3, 4}}).opcodes("F329 D015").run(2);
  REQUIRE(g_oc8_emu_cpu.reg_i == OC8_EMU_FONT_HEXA_ADDR + 4 * 5);
  REQUIRE(g_oc8_emu_cpu.reg_pc == 0x204);
  check_sprite(6, 10, sp4);
}

TEST_CASE("Ins 32: FX33", "") {
  EnvBuilder::get().reg_i(2000).reg(2, 174).opcodes("F233").run();
  REQUIRE(g_oc8_emu_cpu.reg_pc == 0x202);
  REQUIRE(g_oc8_emu_mem.ram[2000] == 1);
  REQUIRE(g_oc8_emu_mem.ram[2001] == 7);
  REQUIRE(g_oc8_emu_mem.ram[2002] == 4);
}

TEST_CASE("Ins 33: FX55", "") {
  EnvBuilder::get()
      .reg_i(2000)
      .reg({{0, 6}, {1, 25}, {2, 18}})
      .opcodes("F255")
      .run();
  REQUIRE(g_oc8_emu_cpu.reg_pc == 0x202);
  REQUIRE(g_oc8_emu_cpu.reg_i == 2003);
  REQUIRE(g_oc8_emu_mem.ram[2000] == 6);
  REQUIRE(g_oc8_emu_mem.ram[2001] == 25);
  REQUIRE(g_oc8_emu_mem.ram[2002] == 18);
}

TEST_CASE("Ins 34: FX65", "") {
  EnvBuilder::get()
      .reg_i(2000)
      .byte({{2000, 6}, {2001, 25}, {2002, 18}})
      .opcodes("F265")
      .run();
  REQUIRE(g_oc8_emu_cpu.reg_pc == 0x202);
  REQUIRE(g_oc8_emu_cpu.reg_i == 2003);
  REQUIRE(g_oc8_emu_cpu.regs_data[0] == 6);
  REQUIRE(g_oc8_emu_cpu.regs_data[1] == 25);
  REQUIRE(g_oc8_emu_cpu.regs_data[2] == 18);
}
