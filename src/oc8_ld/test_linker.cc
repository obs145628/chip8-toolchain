#include <catch2/catch.hpp>

#include <cstring>
#include <string>
#include <vector>

#include "oc8_as/as.h"
#include "oc8_as/parser.h"
#include "oc8_as/sfile.h"
#include "oc8_bin/file.h"
#include "oc8_emu/cpu.h"
#include "oc8_emu/mem.h"
#include "oc8_is/oc8_is.h"
#include "oc8_ld/linker.h"

#include "../../tests/test_src.h"

namespace {

void compile_str(const std::vector<const char *> &code, oc8_bin_file_t *bf) {
  std::vector<oc8_bin_file_t> objs(code.size());
  oc8_ld_linker_t ld;
  oc8_ld_linker_init(&ld, 1);

  for (std::size_t i = 0; i < code.size(); ++i) {
    oc8_as_sfile_t *sf = oc8_as_parse_raw(code[i], strlen(code[i]));
    oc8_bin_file_t *bf = &objs[i];
    oc8_as_sfile_check(sf);
    oc8_as_compile_sfile(sf, bf);
    oc8_bin_file_check(bf, 0);
    oc8_as_sfile_free(sf);
    oc8_ld_linker_add(&ld, bf);
  }

  oc8_ld_linker_link(&ld, bf);
  oc8_bin_file_check(bf, 1);

  for (auto &obj : objs)
    oc8_bin_file_free(&obj);
  oc8_ld_linker_free(&ld);
}

void setup_emu(oc8_bin_file_t *bf) {
  oc8_emu_init();
  oc8_emu_load_rom((const void *)bf->rom, bf->rom_size);
}

void run_emu_until(uint16_t addr, size_t max_steps = 1000) {
  size_t st;
  for (st = 0; st < max_steps && g_oc8_emu_cpu.reg_pc != addr; ++st)
    oc8_emu_cpu_step();
  REQUIRE(st < max_steps);
}

int my_fibo(int x) { return x <= 1 ? x : my_fibo(x - 1) + my_fibo(x - 2); }

int my_fact(int x) { return x <= 1 ? 1 : x * my_fact(x - 1); }

} // namespace

TEST_CASE("ins 7XNN", "") {
  oc8_bin_file_t bf;
  oc8_is_ins_t ins;
  compile_str({".globl _start\n"
               "_start:\n"
               "add 7, %v3"},
              &bf);

  REQUIRE(bf.syms_defs_size == 2);
  REQUIRE(bf.syms_refs_size == 1);
  REQUIRE(bf.rom_size == 4);
  REQUIRE(oc8_is_decode_ins(&ins, (const char *)bf.rom + 2) == 0);
  REQUIRE(ins.type == OC8_IS_TYPE_7XNN);
  REQUIRE(ins.operands[0] == 3);
  REQUIRE(ins.operands[1] == 7);

  setup_emu(&bf);
  g_oc8_emu_cpu.regs_data[3] = 4;
  REQUIRE(g_oc8_emu_cpu.reg_pc == 0x200);
  oc8_emu_cpu_step();
  REQUIRE(g_oc8_emu_cpu.reg_pc == 0x202);
  oc8_emu_cpu_step();
  REQUIRE(g_oc8_emu_cpu.reg_pc == 0x204);
  REQUIRE(g_oc8_emu_cpu.regs_data[3] == 11);

  oc8_bin_file_free(&bf);
}

TEST_CASE("jmp sym", "") {
  oc8_bin_file_t bf;
  oc8_is_ins_t ins;
  compile_str({".globl _start\n"
               "_start:\n"
               "mov 4, %v1\n"
               "jmp foo\n"
               "mov 5, %v1\n"
               "foo:\n"
               "mov 6, %v1\n"},
              &bf);

  REQUIRE(bf.syms_defs_size == 3);
  REQUIRE(bf.syms_refs_size == 2);
  REQUIRE(bf.rom_size == 10);
  REQUIRE(oc8_is_decode_ins(&ins, (const char *)bf.rom + 4) == 0);
  REQUIRE(ins.type == OC8_IS_TYPE_1NNN);
  REQUIRE(ins.operands[0] == 0x208);

  setup_emu(&bf);

  REQUIRE(g_oc8_emu_cpu.reg_pc == 0x200);
  oc8_emu_cpu_step();
  REQUIRE(g_oc8_emu_cpu.reg_pc == 0x202);
  oc8_emu_cpu_step();
  REQUIRE(g_oc8_emu_cpu.reg_pc == 0x204);
  oc8_emu_cpu_step();
  REQUIRE(g_oc8_emu_cpu.reg_pc == 0x208);
  oc8_emu_cpu_step();
  REQUIRE(g_oc8_emu_cpu.regs_data[1] == 6);

  oc8_bin_file_free(&bf);
}

TEST_CASE("function fibo", "") {
  oc8_bin_file_t bf;
  compile_str({test_fibo_src, ".globl _start\n"
                              "_start:\n"
                              "jmp fibo"},
              &bf);

  REQUIRE(bf.syms_defs_size == 5);
  REQUIRE(bf.syms_refs_size == 4);
  REQUIRE(bf.rom_size == 26);

  for (size_t i = 0; i < 15; ++i) {
    setup_emu(&bf);
    g_oc8_emu_cpu.regs_data[0] = i;
    run_emu_until(534);
    if (i < 14)
      REQUIRE(g_oc8_emu_cpu.regs_data[0] == my_fibo(i));
    else
      REQUIRE(g_oc8_emu_cpu.regs_data[0] < my_fibo(i));
  }

  oc8_bin_file_free(&bf);
}

TEST_CASE("program call_add", "") {
  oc8_bin_file_t bf;
  compile_str({test_my_add_src, test_call_add_src}, &bf);

  REQUIRE(bf.syms_defs_size == 3);
  REQUIRE(bf.syms_refs_size == 2);
  REQUIRE(bf.rom_size == 14);

  setup_emu(&bf);

  REQUIRE(g_oc8_emu_cpu.reg_pc == 0x200);
  oc8_emu_cpu_step();
  REQUIRE(g_oc8_emu_cpu.reg_pc == 0x206);
  oc8_emu_cpu_step();
  oc8_emu_cpu_step();
  REQUIRE(g_oc8_emu_cpu.regs_data[0] == 6);
  REQUIRE(g_oc8_emu_cpu.regs_data[1] == 8);
  oc8_emu_cpu_step();
  REQUIRE(g_oc8_emu_cpu.reg_pc == 0x202);
  oc8_emu_cpu_step();
  oc8_emu_cpu_step();
  REQUIRE(g_oc8_emu_cpu.reg_pc == 0x20c);
  oc8_emu_cpu_step();
  REQUIRE(g_oc8_emu_cpu.regs_data[15] == 14);

  oc8_bin_file_free(&bf);
}

TEST_CASE("program call_add_mem_src", "") {
  oc8_bin_file_t bf;
  compile_str({test_my_add_src, test_call_add_mem_src}, &bf);

  REQUIRE(bf.syms_defs_size == 4);
  REQUIRE(bf.syms_refs_size == 3);
  REQUIRE(bf.rom_size == 16);

  setup_emu(&bf);

  REQUIRE(g_oc8_emu_cpu.reg_pc == 0x200);
  oc8_emu_cpu_step();
  REQUIRE(g_oc8_emu_cpu.reg_pc == 0x206);
  oc8_emu_cpu_step();
  oc8_emu_cpu_step();
  REQUIRE(g_oc8_emu_cpu.regs_data[0] == 8);
  REQUIRE(g_oc8_emu_cpu.regs_data[1] == 13);
  oc8_emu_cpu_step();
  REQUIRE(g_oc8_emu_cpu.reg_pc == 0x202);
  oc8_emu_cpu_step();
  oc8_emu_cpu_step();
  REQUIRE(g_oc8_emu_cpu.reg_pc == 0x20c);
  oc8_emu_cpu_step();
  REQUIRE(g_oc8_emu_cpu.regs_data[15] == 21);

  oc8_bin_file_free(&bf);
}

TEST_CASE("function fact_table", "") {
  oc8_bin_file_t bf;
  compile_str({test_fact_table_src, ".globl _start\n"
                                    "_start:\n"
                                    "jmp fact"},
              &bf);

  REQUIRE(bf.syms_defs_size == 7);
  REQUIRE(bf.syms_refs_size == 5);
  REQUIRE(bf.rom_size == 34);

  for (size_t i = 0; i < 32; ++i) {
    setup_emu(&bf);
    g_oc8_emu_cpu.regs_data[0] = i;
    run_emu_until(0x202 + 22);
    if (i < 6)
      REQUIRE(g_oc8_emu_cpu.regs_data[0] == my_fact(i));
    else
      REQUIRE(g_oc8_emu_cpu.regs_data[0] == 0xFF);
  }

  oc8_bin_file_free(&bf);
}
