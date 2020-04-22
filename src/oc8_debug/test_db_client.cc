#include "test_utils.hh"

#define CALL_ADD_TMP_PATH "/tmp/oc8_test_debug_call_add.c8bin"

namespace {

void gen_call_add_bin() {
  static bool gen = false;
  if (gen)
    return;
  gen = true;

  std::remove(CALL_ADD_TMP_PATH);
  compile_str_to_file({test_my_add_src, test_call_add_mem_src},
                      CALL_ADD_TMP_PATH);
}

void test_call_add_infos(SimpleCLIMode mode) {
  (void)mode;
  gen_call_add_bin();
  const char *cmds = ""
                     "preg u8 %v0\n"
                     "pregi %0\n"
                     "pregi %v1\n"
                     "pregi %2 %v3\n"
                     "pregi %v4 %5\n"
                     "pregi %pc %i\n"
                     "pregi %18 %19 %20\n"
                     "preg u8 %sp\n"
                     "preg u16 %pc\n"
                     "sreg u8 %v1 67\n"
                     "preg u8 %1\n"
                     "pregi %21\n"
                     "pregi %v10\n"
                     "pmem u8 @args 2\n"
                     "smem u16 @args 1892\n"
                     "pmem u8 @args 4\n"
                     "psym @_rom_begin\n"
                     "psym @1\n"
                     "psym @2\n"
                     "psym @_start\n"
                     "psym @foo\n"
                     "psym @4\n"
                     "s\n";
  auto vals = str_split(run_simplecli(mode, CALL_ADD_TMP_PATH, cmds), '\n');
  REQUIRE(vals.size() == 53);
  REQUIRE(vals[0] == "program stopped at 0x200 (<_rom_begin> + 0x0)");
  REQUIRE(vals[1] == "%v0: 0");
  REQUIRE(vals[2] == "Register v0 (#0):");
  REQUIRE(vals[3] == "  General purpose register");
  REQUIRE(vals[4] == "  size: 1 bytes");
  REQUIRE(vals[5] == "Register v1 (#1):");
  REQUIRE(vals[6] == "  General purpose register");
  REQUIRE(vals[7] == "  size: 1 bytes");
  REQUIRE(vals[8] == "Register v2 (#2):");
  REQUIRE(vals[9] == "  General purpose register");
  REQUIRE(vals[10] == "  size: 1 bytes");
  REQUIRE(vals[11] == "Register v3 (#3):");
  REQUIRE(vals[12] == "  General purpose register");
  REQUIRE(vals[13] == "  size: 1 bytes");
  REQUIRE(vals[14] == "Register v4 (#4):");
  REQUIRE(vals[15] == "  General purpose register");
  REQUIRE(vals[16] == "  size: 1 bytes");
  REQUIRE(vals[17] == "Register v5 (#5):");
  REQUIRE(vals[18] == "  General purpose register");
  REQUIRE(vals[19] == "  size: 1 bytes");
  REQUIRE(vals[20] == "Register pc (#16):");
  REQUIRE(vals[21] == "  Program counter");
  REQUIRE(vals[22] == "  size: 2 bytes");
  REQUIRE(vals[23] == "Register i (#17):");
  REQUIRE(vals[24] == "  General purpose register");
  REQUIRE(vals[25] == "  size: 2 bytes");
  REQUIRE(vals[26] == "Register sp (#18):");
  REQUIRE(vals[27] == "  Stack pointer");
  REQUIRE(vals[28] == "  size: 1 bytes");
  REQUIRE(vals[29] == "Register dt (#19):");
  REQUIRE(vals[30] == "  General purpose register");
  REQUIRE(vals[31] == "  size: 1 bytes");
  REQUIRE(vals[32] == "Register st (#20):");
  REQUIRE(vals[33] == "  General purpose register");
  REQUIRE(vals[34] == "  size: 1 bytes");
  REQUIRE(vals[35] == "%sp: 0");
  REQUIRE(vals[36] == "%pc: 512");
  REQUIRE(vals[37] == "%1: 67");
  REQUIRE(vals[38] == "Error: Invalid register identifier");
  REQUIRE(vals[39] == "Error: Invalid register name");
  REQUIRE(vals[40] == "8 13 ");
  REQUIRE(vals[41] == "100 7 0 0 ");
  REQUIRE(vals[42] == "Symbol _rom_begin (#0):");
  REQUIRE(vals[43] == "  address: 0x200");
  REQUIRE(vals[44] == "Symbol my_add (#1):");
  REQUIRE(vals[45] == "  address: 0x202");
  REQUIRE(vals[46] == "Symbol args (#2):");
  REQUIRE(vals[47] == "  address: 0x20e");
  REQUIRE(vals[48] == "Symbol _start (#3):");
  REQUIRE(vals[49] == "  address: 0x206");
  REQUIRE(vals[50] == "Error: Invalid symbol name");
  REQUIRE(vals[51] == "Error: Invalid symbol index");
  REQUIRE(vals[52] == "program stopped at 0x206 (<_rom_begin> + 0x6)");
}

} // namespace

TEST_CASE("simplecli_on_server call_add infos", "") {
  test_call_add_infos(SimpleCLIMode::ON_SERVER);
}
