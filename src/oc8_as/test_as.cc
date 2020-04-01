#include <catch2/catch.hpp>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "../../tests/test_src.h"
#include "oc8_as/as.h"
#include "oc8_as/parser.h"
#include "oc8_as/sfile.h"
#include "oc8_bin/file.h"
#include "oc8_is/oc8_is.h"

namespace {

std::set<std::string> ext_syms(oc8_bin_file_t *bf) {
  std::set<std::string> res;
  for (std::size_t i = 0; i < bf->syms_defs_size; ++i) {
    oc8_bin_sym_def_t *def = &bf->syms_defs[i];
    if (def->addr == 0)
      res.emplace(def->name);
  }
  return res;
}

std::set<std::string> def_syms(oc8_bin_file_t *bf) {
  std::set<std::string> res;
  for (std::size_t i = 0; i < bf->syms_defs_size; ++i) {
    oc8_bin_sym_def_t *def = &bf->syms_defs[i];
    if (def->addr != 0)
      res.emplace(def->name);
  }
  return res;
}

std::string ref_at(oc8_bin_file_t *bf, uint16_t addr) {
  for (size_t i = 0; i < bf->syms_refs_size; ++i) {
    oc8_bin_sym_ref_t *ref = &bf->syms_refs[i];
    if (ref->ins_addr == addr)
      return bf->syms_defs[ref->sym_id].name;
  }
  return "";
}

std::string def_at(oc8_bin_file_t *bf, uint16_t addr) {
  for (size_t i = 0; i < bf->syms_defs_size; ++i) {
    oc8_bin_sym_def_t *def = &bf->syms_defs[i];
    if (def->addr == addr)
      return def->name;
  }
  return "";
}

oc8_bin_sym_type_t def_type(oc8_bin_file_t *bf, const std::string &name) {
  for (size_t i = 0; i < bf->syms_defs_size; ++i) {
    oc8_bin_sym_def_t *def = &bf->syms_defs[i];
    if (def->name == name)
      return def->type;
  }
  return OC8_BIN_SYM_TYPE_NO;
}

bool def_global(oc8_bin_file_t *bf, const std::string &name) {
  for (size_t i = 0; i < bf->syms_defs_size; ++i) {
    oc8_bin_sym_def_t *def = &bf->syms_defs[i];
    if (def->name == name)
      return def->is_global;
  }
  return false;
}

oc8_as_sfile_t *parse_str(const std::string &str) {
  return oc8_as_parse_raw(str.c_str(), str.size());
}

} // namespace

TEST_CASE("compile simple ins 7XNN", "") {
  oc8_as_sfile_t *sf = oc8_as_sfile_new();
  oc8_as_sfile_ins_add_imm(sf, 0x73, 0xB);

  oc8_bin_file_t bf;
  oc8_as_compile_sfile(sf, &bf);
  oc8_bin_file_check(&bf, 0);

  REQUIRE(bf.syms_defs_size == 0);
  REQUIRE(bf.syms_refs_size == 0);
  REQUIRE(bf.rom_size == 2);

  oc8_is_ins_t ins;
  REQUIRE(oc8_is_decode_ins(&ins, (const char *)bf.rom) == 0);
  REQUIRE(ins.type == OC8_IS_TYPE_7XNN);
  REQUIRE(ins.operands[0] == 0xB);
  REQUIRE(ins.operands[1] == 0x73);

  oc8_bin_file_free(&bf);
  oc8_as_sfile_free(sf);
}

TEST_CASE("compile 2 ins", "") {
  oc8_as_sfile_t *sf = oc8_as_sfile_new();
  oc8_as_sfile_ins_add_imm(sf, 0x73, 0xB);
  oc8_as_sfile_ins_bcd(sf, 0xE);

  oc8_bin_file_t bf;
  oc8_as_compile_sfile(sf, &bf);
  oc8_bin_file_check(&bf, 0);

  REQUIRE(bf.syms_defs_size == 0);
  REQUIRE(bf.syms_refs_size == 0);
  REQUIRE(bf.rom_size == 4);

  oc8_is_ins_t ins;
  REQUIRE(oc8_is_decode_ins(&ins, (const char *)bf.rom) == 0);
  REQUIRE(ins.type == OC8_IS_TYPE_7XNN);
  REQUIRE(ins.operands[0] == 0xB);
  REQUIRE(ins.operands[1] == 0x73);

  REQUIRE(oc8_is_decode_ins(&ins, (char *)bf.rom + 2) == 0);
  REQUIRE(ins.type == OC8_IS_TYPE_FX33);
  REQUIRE(ins.operands[0] == 0xE);

  oc8_bin_file_free(&bf);
  oc8_as_sfile_free(sf);
}

TEST_CASE("compile misc no syms", "") {
  oc8_as_sfile_t *sf = oc8_as_sfile_new();
  oc8_as_sfile_ins_draw(sf, 0xE, 0x6, 0XA);
  oc8_as_sfile_dir_align(sf, 8);
  oc8_as_sfile_dir_byte(sf, 0x47);
  oc8_as_sfile_dir_word(sf, 0x76B3);
  oc8_as_sfile_dir_zero(sf, 3);

  oc8_bin_file_t bf;
  oc8_as_compile_sfile(sf, &bf);
  oc8_bin_file_check(&bf, 0);

  REQUIRE(bf.syms_defs_size == 0);
  REQUIRE(bf.syms_refs_size == 0);
  REQUIRE(bf.rom_size == 14);

  oc8_is_ins_t ins;
  REQUIRE(oc8_is_decode_ins(&ins, (const char *)bf.rom) == 0);
  REQUIRE(ins.type == OC8_IS_TYPE_DXYN);
  REQUIRE(ins.operands[0] == 0xE);
  REQUIRE(ins.operands[1] == 0x6);
  REQUIRE(ins.operands[2] == 0xA);

  for (size_t i = 2; i < 8; ++i)
    REQUIRE(bf.rom[i] == 0);
  REQUIRE(bf.rom[8] == 0x47);
  REQUIRE(*((uint16_t *)&bf.rom[9]) == 0x76B3);
  for (size_t i = 11; i < 14; ++i)
    REQUIRE(bf.rom[i] == 0);

  oc8_bin_file_free(&bf);
  oc8_as_sfile_free(sf);
}

TEST_CASE("compile 2 syms use 0 syms defs", "") {
  oc8_as_sfile_t *sf = oc8_as_sfile_new();
  oc8_as_sfile_sins_jmp(sf, "foo");
  oc8_as_sfile_sins_jmp(sf, "bar");

  oc8_bin_file_t bf;
  oc8_as_compile_sfile(sf, &bf);
  oc8_bin_file_check(&bf, 0);

  REQUIRE(bf.syms_defs_size == 2);
  REQUIRE(bf.syms_refs_size == 2);
  REQUIRE(bf.rom_size == 4);

  REQUIRE(ext_syms(&bf) == std::set<std::string>{"foo", "bar"});
  REQUIRE(def_syms(&bf) == std::set<std::string>{});
  REQUIRE(ref_at(&bf, 0x200) == "foo");
  REQUIRE(ref_at(&bf, 0x202) == "bar");

  oc8_is_ins_t ins;
  REQUIRE(oc8_is_decode_ins(&ins, (const char *)bf.rom) == 0);
  REQUIRE(ins.type == OC8_IS_TYPE_1NNN);

  ins.opcode = 0;
  REQUIRE(oc8_is_decode_ins(&ins, (char *)bf.rom + 2) == 0);
  REQUIRE(ins.type == OC8_IS_TYPE_1NNN);

  oc8_bin_file_free(&bf);
  oc8_as_sfile_free(sf);
}

TEST_CASE("compile 0 syms use 2 syms defs", "") {
  oc8_as_sfile_t *sf = oc8_as_sfile_new();
  oc8_as_sfile_add_sym(sf, "foo");
  oc8_as_sfile_ins_jmp(sf, 44);
  oc8_as_sfile_add_sym(sf, "bar");
  oc8_as_sfile_ins_ret(sf);

  oc8_bin_file_t bf;
  oc8_as_compile_sfile(sf, &bf);
  oc8_bin_file_check(&bf, 0);

  REQUIRE(bf.syms_defs_size == 2);
  REQUIRE(bf.syms_refs_size == 0);
  REQUIRE(bf.rom_size == 4);

  REQUIRE(ext_syms(&bf) == std::set<std::string>{});
  REQUIRE(def_syms(&bf) == std::set<std::string>{"foo", "bar"});
  REQUIRE(def_at(&bf, 0x200) == "foo");
  REQUIRE(def_at(&bf, 0x202) == "bar");

  oc8_is_ins_t ins;
  REQUIRE(oc8_is_decode_ins(&ins, (const char *)bf.rom) == 0);
  REQUIRE(ins.type == OC8_IS_TYPE_1NNN);

  ins.opcode = 0;
  REQUIRE(oc8_is_decode_ins(&ins, (char *)bf.rom + 2) == 0);
  REQUIRE(ins.type == OC8_IS_TYPE_00EE);

  oc8_bin_file_free(&bf);
  oc8_as_sfile_free(sf);
}

TEST_CASE("compile 3 syms use 3 syms defs", "") {
  oc8_as_sfile_t *sf = oc8_as_sfile_new();
  oc8_as_sfile_add_sym(sf, "foo");
  oc8_as_sfile_sins_jmp(sf, "bar");
  oc8_as_sfile_add_sym(sf, "bar");
  oc8_as_sfile_sins_jmp(sf, "ping");
  oc8_as_sfile_add_sym(sf, "pong");
  oc8_as_sfile_sins_jmp(sf, "foo");

  oc8_bin_file_t bf;
  oc8_as_compile_sfile(sf, &bf);
  oc8_bin_file_check(&bf, 0);

  REQUIRE(bf.syms_defs_size == 4);
  REQUIRE(bf.syms_refs_size == 3);
  REQUIRE(bf.rom_size == 6);

  REQUIRE(ext_syms(&bf) == std::set<std::string>{"ping"});
  REQUIRE(def_syms(&bf) == std::set<std::string>{"foo", "bar", "pong"});
  REQUIRE(def_at(&bf, 0x200) == "foo");
  REQUIRE(def_at(&bf, 0x202) == "bar");
  REQUIRE(def_at(&bf, 0x204) == "pong");
  REQUIRE(ref_at(&bf, 0x200) == "bar");
  REQUIRE(ref_at(&bf, 0x202) == "ping");
  REQUIRE(ref_at(&bf, 0x204) == "foo");

  oc8_is_ins_t ins;
  REQUIRE(oc8_is_decode_ins(&ins, (const char *)bf.rom) == 0);
  REQUIRE(ins.type == OC8_IS_TYPE_1NNN);

  ins.opcode = 0;
  REQUIRE(oc8_is_decode_ins(&ins, (char *)bf.rom + 2) == 0);
  REQUIRE(ins.type == OC8_IS_TYPE_1NNN);

  ins.opcode = 0;
  REQUIRE(oc8_is_decode_ins(&ins, (char *)bf.rom + 4) == 0);
  REQUIRE(ins.type == OC8_IS_TYPE_1NNN);

  oc8_bin_file_free(&bf);
  oc8_as_sfile_free(sf);
}

TEST_CASE("compile fibo", "") {
  oc8_as_sfile_t *sf = parse_str(test_fibo_src);
  oc8_bin_file_t bf;
  oc8_as_compile_sfile(sf, &bf);
  oc8_bin_file_check(&bf, 0);

  REQUIRE(bf.syms_defs_size == 3);
  REQUIRE(bf.syms_refs_size == 2);
  REQUIRE(bf.rom_size == 22);
  REQUIRE(ext_syms(&bf) == std::set<std::string>{});
  REQUIRE(def_syms(&bf) == std::set<std::string>{"fibo", "L0", "L1"});
  REQUIRE(def_type(&bf, "fibo") == OC8_BIN_SYM_TYPE_FUN);
  REQUIRE(def_global(&bf, "fibo") == true);
  REQUIRE(def_at(&bf, 0x200) == "fibo");
  REQUIRE(def_at(&bf, 0x204) == "L0");
  REQUIRE(def_at(&bf, 0x212) == "L1");
  REQUIRE(ref_at(&bf, 0x206) == "L1");
  REQUIRE(ref_at(&bf, 0x210) == "L0");

  oc8_bin_file_free(&bf);
  oc8_as_sfile_free(sf);
}

TEST_CASE("compile fact_table", "") {
  oc8_as_sfile_t *sf = parse_str(test_fact_table_src);
  oc8_bin_file_t bf;
  oc8_as_compile_sfile(sf, &bf);
  oc8_bin_file_check(&bf, 0);

  REQUIRE(bf.syms_defs_size == 5);
  REQUIRE(bf.syms_refs_size == 3);
  REQUIRE(bf.rom_size == 12 * 2 + 7);
  REQUIRE(ext_syms(&bf) == std::set<std::string>{});
  REQUIRE(def_syms(&bf) ==
          std::set<std::string>{"fact", "Lmax", "Ltable", "Lend", "table"});
  REQUIRE(def_type(&bf, "fact") == OC8_BIN_SYM_TYPE_FUN);
  REQUIRE(def_global(&bf, "fact") == true);
  REQUIRE(def_type(&bf, "table") == OC8_BIN_SYM_TYPE_OBJ);
  REQUIRE(def_global(&bf, "table") == false);
  REQUIRE(def_type(&bf, "Lend") == OC8_BIN_SYM_TYPE_NO);
  REQUIRE(def_global(&bf, "Lend") == false);
  REQUIRE(def_at(&bf, 0x200) == "fact");
  REQUIRE(def_at(&bf, 0x20c) == "Lmax");
  REQUIRE(def_at(&bf, 0x210) == "Ltable");
  REQUIRE(def_at(&bf, 0x216) == "Lend");
  REQUIRE(def_at(&bf, 0x218) == "table");
  REQUIRE(ref_at(&bf, 0x20a) == "Ltable");
  REQUIRE(ref_at(&bf, 0x20e) == "Lend");
  REQUIRE(ref_at(&bf, 0x210) == "table");

  oc8_bin_file_free(&bf);
  oc8_as_sfile_free(sf);
}
