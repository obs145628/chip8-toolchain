#include <catch2/catch.hpp>
#include <sstream>
#include <string>
#include <vector>

#include "oc8_as/printer.h"
#include "oc8_as/sfile.h"

namespace {

bool is_wspace(char c) {
  return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

std::string trim(const std::string &str) {
  std::size_t bpos = 0;
  while (bpos < str.length() && is_wspace(str[bpos]))
    ++bpos;
  if (bpos == str.length())
    return "";

  std::size_t epos = str.length() - 1;
  while (is_wspace(str[epos]))
    --epos;

  return std::string(str.begin() + bpos, str.begin() + epos + 1);
}

std::vector<std::string> split(const std::string &str, char sep) {
  std::vector<std::string> res;
  std::istringstream is(str);
  std::string val;
  while (std::getline(is, val, sep))
    res.push_back(val);
  return res;
}

std::string print_short(oc8_as_sfile_t *sf) {
  char buff[512];
  oc8_as_sfile_check(sf);
  auto len = oc8_as_print_sfile(sf, buff, sizeof(buff), NULL, NULL);
  buff[len] = '\0';
  return std::string(buff);
}

} // namespace

TEST_CASE("sfile ins 7XNN", "") {
  oc8_as_sfile_t *sf = oc8_as_sfile_new();
  oc8_as_sfile_ins_add_imm(sf, 0x73, 0xB);
  REQUIRE(trim(print_short(sf)) == "add 0x73, %vb");
  oc8_as_sfile_free(sf);
}

TEST_CASE("sfile ins 8XY4", "") {
  oc8_as_sfile_t *sf = oc8_as_sfile_new();
  oc8_as_sfile_ins_add(sf, 0x1, 0xC);
  REQUIRE(trim(print_short(sf)) == "add %v1, %vc");
  oc8_as_sfile_free(sf);
}

TEST_CASE("sfile ins FX1E", "") {
  oc8_as_sfile_t *sf = oc8_as_sfile_new();
  oc8_as_sfile_ins_add_i(sf, 0x6);
  REQUIRE(trim(print_short(sf)) == "add %v6, %i");
  oc8_as_sfile_free(sf);
}

TEST_CASE("sfile ins 8XY2", "") {
  oc8_as_sfile_t *sf = oc8_as_sfile_new();
  oc8_as_sfile_ins_and(sf, 0x2, 0x4);
  REQUIRE(trim(print_short(sf)) == "and %v2, %v4");
  oc8_as_sfile_free(sf);
}

TEST_CASE("sfile ins FX33", "") {
  oc8_as_sfile_t *sf = oc8_as_sfile_new();
  oc8_as_sfile_ins_bcd(sf, 0xE);
  REQUIRE(trim(print_short(sf)) == "bcd %ve");
  oc8_as_sfile_free(sf);
}

TEST_CASE("sfile ins 2NNN", "") {
  oc8_as_sfile_t *sf = oc8_as_sfile_new();
  oc8_as_sfile_ins_call(sf, 0x738);
  REQUIRE(trim(print_short(sf)) == "call 0x738");
  oc8_as_sfile_free(sf);
}

TEST_CASE("sfile ins 00E0", "") {
  oc8_as_sfile_t *sf = oc8_as_sfile_new();
  oc8_as_sfile_ins_cls(sf);
  REQUIRE(trim(print_short(sf)) == "cls");
  oc8_as_sfile_free(sf);
}

TEST_CASE("sfile ins DXYN", "") {
  oc8_as_sfile_t *sf = oc8_as_sfile_new();
  oc8_as_sfile_ins_draw(sf, 0xE, 0x6, 0XA);
  REQUIRE(trim(print_short(sf)) == "draw %ve, %v6, 0xA");
  oc8_as_sfile_free(sf);
}

TEST_CASE("sfile ins FX29", "") {
  oc8_as_sfile_t *sf = oc8_as_sfile_new();
  oc8_as_sfile_ins_fspr(sf, 0xB);
  REQUIRE(trim(print_short(sf)) == "fspr %vb");
  oc8_as_sfile_free(sf);
}

TEST_CASE("sfile ins 1NNN", "") {
  oc8_as_sfile_t *sf = oc8_as_sfile_new();
  oc8_as_sfile_ins_jmp(sf, 0xF1F);
  REQUIRE(trim(print_short(sf)) == "jmp 0xF1F");
  oc8_as_sfile_free(sf);
}

TEST_CASE("sfile ins BNNN", "") {
  oc8_as_sfile_t *sf = oc8_as_sfile_new();
  oc8_as_sfile_ins_jmp_v0(sf, 0xBCD);
  REQUIRE(trim(print_short(sf)) == "jmp 0xBCD(%v0)");
  oc8_as_sfile_free(sf);
}

TEST_CASE("sfile ins 6XNN", "") {
  oc8_as_sfile_t *sf = oc8_as_sfile_new();
  oc8_as_sfile_ins_mov_imm(sf, 0x79, 0xC);
  REQUIRE(trim(print_short(sf)) == "mov 0x79, %vc");
  oc8_as_sfile_free(sf);
}

TEST_CASE("sfile ins 8XY0", "") {
  oc8_as_sfile_t *sf = oc8_as_sfile_new();
  oc8_as_sfile_ins_mov(sf, 0x5, 0x0);
  REQUIRE(trim(print_short(sf)) == "mov %v5, %v0");
  oc8_as_sfile_free(sf);
}

TEST_CASE("sfile ins ANNN", "") {
  oc8_as_sfile_t *sf = oc8_as_sfile_new();
  oc8_as_sfile_ins_mov_i(sf, 0x790);
  REQUIRE(trim(print_short(sf)) == "mov 0x790, %i");
  oc8_as_sfile_free(sf);
}

TEST_CASE("sfile ins FX07", "") {
  oc8_as_sfile_t *sf = oc8_as_sfile_new();
  oc8_as_sfile_ins_mov_fdt(sf, 0xC);
  REQUIRE(trim(print_short(sf)) == "mov %dt, %vc");
  oc8_as_sfile_free(sf);
}

TEST_CASE("sfile ins FX15", "") {
  oc8_as_sfile_t *sf = oc8_as_sfile_new();
  oc8_as_sfile_ins_mov_dt(sf, 0x1);
  REQUIRE(trim(print_short(sf)) == "mov %v1, %dt");
  oc8_as_sfile_free(sf);
}

TEST_CASE("sfile ins FX18", "") {
  oc8_as_sfile_t *sf = oc8_as_sfile_new();
  oc8_as_sfile_ins_mov_st(sf, 0xF);
  REQUIRE(trim(print_short(sf)) == "mov %vf, %st");
  oc8_as_sfile_free(sf);
}

TEST_CASE("sfile ins FX55", "") {
  oc8_as_sfile_t *sf = oc8_as_sfile_new();
  oc8_as_sfile_ins_movm_st(sf, 0x4);
  REQUIRE(trim(print_short(sf)) == "movm %v4, %i");
  oc8_as_sfile_free(sf);
}

TEST_CASE("sfile ins FX65", "") {
  oc8_as_sfile_t *sf = oc8_as_sfile_new();
  oc8_as_sfile_ins_movm_ld(sf, 0x4);
  REQUIRE(trim(print_short(sf)) == "movm %i, %v4");
  oc8_as_sfile_free(sf);
}

TEST_CASE("sfile ins 8XY1", "") {
  oc8_as_sfile_t *sf = oc8_as_sfile_new();
  oc8_as_sfile_ins_or(sf, 0x4, 0xB);
  REQUIRE(trim(print_short(sf)) == "or %v4, %vb");
  oc8_as_sfile_free(sf);
}

TEST_CASE("sfile ins CXNN", "") {
  oc8_as_sfile_t *sf = oc8_as_sfile_new();
  oc8_as_sfile_ins_rand(sf, 0x56, 0x7);
  REQUIRE(trim(print_short(sf)) == "rand 0x56, %v7");
  oc8_as_sfile_free(sf);
}

TEST_CASE("sfile ins 00EE", "") {
  oc8_as_sfile_t *sf = oc8_as_sfile_new();
  oc8_as_sfile_ins_ret(sf);
  REQUIRE(trim(print_short(sf)) == "ret");
  oc8_as_sfile_free(sf);
}

TEST_CASE("sfile ins 8XYE", "") {
  oc8_as_sfile_t *sf = oc8_as_sfile_new();
  oc8_as_sfile_ins_shl(sf, 0x2, 0x1);
  REQUIRE(trim(print_short(sf)) == "shl %v2, %v1");
  oc8_as_sfile_free(sf);
}

TEST_CASE("sfile ins 8XY6", "") {
  oc8_as_sfile_t *sf = oc8_as_sfile_new();
  oc8_as_sfile_ins_shr(sf, 0x2, 0x1);
  REQUIRE(trim(print_short(sf)) == "shr %v2, %v1");
  oc8_as_sfile_free(sf);
}

TEST_CASE("sfile ins 3XNN", "") {
  oc8_as_sfile_t *sf = oc8_as_sfile_new();
  oc8_as_sfile_ins_skpe_imm(sf, 0x67, 0xE);
  REQUIRE(trim(print_short(sf)) == "skpe 0x67, %ve");
  oc8_as_sfile_free(sf);
}

TEST_CASE("sfile ins 5XY0", "") {
  oc8_as_sfile_t *sf = oc8_as_sfile_new();
  oc8_as_sfile_ins_skpe(sf, 0xC, 0xE);
  REQUIRE(trim(print_short(sf)) == "skpe %vc, %ve");
  oc8_as_sfile_free(sf);
}

TEST_CASE("sfile ins 4XNN", "") {
  oc8_as_sfile_t *sf = oc8_as_sfile_new();
  oc8_as_sfile_ins_skpn_imm(sf, 0x67, 0xE);
  REQUIRE(trim(print_short(sf)) == "skpn 0x67, %ve");
  oc8_as_sfile_free(sf);
}

TEST_CASE("sfile ins 9XY0", "") {
  oc8_as_sfile_t *sf = oc8_as_sfile_new();
  oc8_as_sfile_ins_skpn(sf, 0xC, 0xE);
  REQUIRE(trim(print_short(sf)) == "skpn %vc, %ve");
  oc8_as_sfile_free(sf);
}

TEST_CASE("sfile ins EX9E", "") {
  oc8_as_sfile_t *sf = oc8_as_sfile_new();
  oc8_as_sfile_ins_skpkp(sf, 0x8);
  REQUIRE(trim(print_short(sf)) == "skpkp %v8");
  oc8_as_sfile_free(sf);
}

TEST_CASE("sfile ins EXAI", "") {
  oc8_as_sfile_t *sf = oc8_as_sfile_new();
  oc8_as_sfile_ins_skpkn(sf, 0x8);
  REQUIRE(trim(print_short(sf)) == "skpkn %v8");
  oc8_as_sfile_free(sf);
}

TEST_CASE("sfile ins 8XY5", "") {
  oc8_as_sfile_t *sf = oc8_as_sfile_new();
  oc8_as_sfile_ins_sub(sf, 0x8, 0xC);
  REQUIRE(trim(print_short(sf)) == "sub %v8, %vc");
  oc8_as_sfile_free(sf);
}

TEST_CASE("sfile ins 8XY7", "") {
  oc8_as_sfile_t *sf = oc8_as_sfile_new();
  oc8_as_sfile_ins_subn(sf, 0x8, 0xC);
  REQUIRE(trim(print_short(sf)) == "subn %v8, %vc");
  oc8_as_sfile_free(sf);
}

TEST_CASE("sfile ins 0NNN", "") {
  oc8_as_sfile_t *sf = oc8_as_sfile_new();
  oc8_as_sfile_ins_sys(sf, 0x843);
  REQUIRE(trim(print_short(sf)) == "sys 0x843");
  oc8_as_sfile_free(sf);
}

TEST_CASE("sfile ins FX0A", "") {
  oc8_as_sfile_t *sf = oc8_as_sfile_new();
  oc8_as_sfile_ins_waitk(sf, 0x6);
  REQUIRE(trim(print_short(sf)) == "waitk %v6");
  oc8_as_sfile_free(sf);
}

TEST_CASE("sfile ins 8XY3", "") {
  oc8_as_sfile_t *sf = oc8_as_sfile_new();
  oc8_as_sfile_ins_xor(sf, 0x9, 0xC);
  REQUIRE(trim(print_short(sf)) == "xor %v9, %vc");
  oc8_as_sfile_free(sf);
}

TEST_CASE("sfile dir byte", "") {
  oc8_as_sfile_t *sf = oc8_as_sfile_new();
  oc8_as_sfile_dir_byte(sf, 0xF6);
  REQUIRE(trim(print_short(sf)) == ".byte 0xF6");
  oc8_as_sfile_free(sf);
}

TEST_CASE("sfile dir word", "") {
  oc8_as_sfile_t *sf = oc8_as_sfile_new();
  oc8_as_sfile_dir_word(sf, 0xF6C4);
  REQUIRE(trim(print_short(sf)) == ".word 0xF6C4");
  oc8_as_sfile_free(sf);
}

TEST_CASE("sfile dir zero", "") {
  oc8_as_sfile_t *sf = oc8_as_sfile_new();
  oc8_as_sfile_dir_zero(sf, 38);
  REQUIRE(trim(print_short(sf)) == ".zero 38");
  oc8_as_sfile_free(sf);
}

TEST_CASE("sfile sins 7XNN", "") {
  oc8_as_sfile_t *sf = oc8_as_sfile_new();
  oc8_as_sfile_sins_add_imm(sf, "foo", 0xB);
  REQUIRE(trim(print_short(sf)) == "add foo, %vb");
  oc8_as_sfile_free(sf);
}

TEST_CASE("sfile sins 2NNN", "") {
  oc8_as_sfile_t *sf = oc8_as_sfile_new();
  oc8_as_sfile_sins_call(sf, "foo");
  REQUIRE(trim(print_short(sf)) == "call foo");
  oc8_as_sfile_free(sf);
}

TEST_CASE("sfile sins DXYN", "") {
  oc8_as_sfile_t *sf = oc8_as_sfile_new();
  oc8_as_sfile_sins_draw(sf, 0xE, 0x6, "foo");
  REQUIRE(trim(print_short(sf)) == "draw %ve, %v6, foo");
  oc8_as_sfile_free(sf);
}

TEST_CASE("sfile sins 1NNN", "") {
  oc8_as_sfile_t *sf = oc8_as_sfile_new();
  oc8_as_sfile_sins_jmp(sf, "foo");
  REQUIRE(trim(print_short(sf)) == "jmp foo");
  oc8_as_sfile_free(sf);
}

TEST_CASE("sfile sins BNNN", "") {
  oc8_as_sfile_t *sf = oc8_as_sfile_new();
  oc8_as_sfile_sins_jmp_v0(sf, "foo");
  REQUIRE(trim(print_short(sf)) == "jmp foo(%v0)");
  oc8_as_sfile_free(sf);
}

TEST_CASE("sfile sins 6XNN", "") {
  oc8_as_sfile_t *sf = oc8_as_sfile_new();
  oc8_as_sfile_sins_mov_imm(sf, "foo", 0xC);
  REQUIRE(trim(print_short(sf)) == "mov foo, %vc");
  oc8_as_sfile_free(sf);
}

TEST_CASE("sfile sins ANNN", "") {
  oc8_as_sfile_t *sf = oc8_as_sfile_new();
  oc8_as_sfile_sins_mov_i(sf, "foo");
  REQUIRE(trim(print_short(sf)) == "mov foo, %i");
  oc8_as_sfile_free(sf);
}

TEST_CASE("sfile sins CXNN", "") {
  oc8_as_sfile_t *sf = oc8_as_sfile_new();
  oc8_as_sfile_sins_rand(sf, "bar", 0x7);
  REQUIRE(trim(print_short(sf)) == "rand bar, %v7");
  oc8_as_sfile_free(sf);
}

TEST_CASE("sfile sins 3XNN", "") {
  oc8_as_sfile_t *sf = oc8_as_sfile_new();
  oc8_as_sfile_sins_skpe_imm(sf, "foo", 0xE);
  REQUIRE(trim(print_short(sf)) == "skpe foo, %ve");
  oc8_as_sfile_free(sf);
}

TEST_CASE("sfile sins 4XNN", "") {
  oc8_as_sfile_t *sf = oc8_as_sfile_new();
  oc8_as_sfile_sins_skpn_imm(sf, "foo", 0xE);
  REQUIRE(trim(print_short(sf)) == "skpn foo, %ve");
  oc8_as_sfile_free(sf);
}

TEST_CASE("sfile sins 0NNN", "") {
  oc8_as_sfile_t *sf = oc8_as_sfile_new();
  oc8_as_sfile_sins_sys(sf, "__syscall");
  REQUIRE(trim(print_short(sf)) == "sys __syscall");
  oc8_as_sfile_free(sf);
}

TEST_CASE("sfile simple label", "") {
  oc8_as_sfile_t *sf = oc8_as_sfile_new();
  oc8_as_sfile_ins_cls(sf);
  oc8_as_sfile_add_sym(sf, "foo");
  oc8_as_sfile_ins_ret(sf);
  auto code = split(trim(print_short(sf)), '\n');

  REQUIRE(code.size() == 3);
  REQUIRE(trim(code[0]) == "cls");
  REQUIRE(trim(code[1]) == "foo:");
  REQUIRE(trim(code[2]) == "ret");
  oc8_as_sfile_free(sf);
}

TEST_CASE("sfile infos label", "") {
  oc8_as_sfile_t *sf = oc8_as_sfile_new();
  oc8_as_sfile_dir_globl(sf, "foo");
  oc8_as_sfile_ins_cls(sf);
  oc8_as_sfile_dir_size(sf, "foo", 8);
  oc8_as_sfile_add_sym(sf, "foo");
  oc8_as_sfile_ins_ret(sf);
  oc8_as_sfile_dir_type(sf, "foo", OC8_AS_DATA_SYM_TYPE_FUN);
  auto code = split(trim(print_short(sf)), '\n');

  REQUIRE(code.size() == 6);
  REQUIRE(trim(code[0]) == "cls");
  REQUIRE(trim(code[1]) == ".size foo, 8");
  REQUIRE(trim(code[2]) == ".type foo, @function");
  REQUIRE(trim(code[3]) == ".globl foo");
  REQUIRE(trim(code[4]) == "foo:");
  REQUIRE(trim(code[5]) == "ret");
  oc8_as_sfile_free(sf);
}

TEST_CASE("sfile addr values", "") {
  oc8_as_sfile_t *sf = oc8_as_sfile_new();
  REQUIRE(sf->curr_addr == 0);

  oc8_as_sfile_dir_align(sf, 0x2);
  REQUIRE(sf->curr_addr == 0);
  oc8_as_sfile_dir_byte(sf, 0);
  REQUIRE(sf->curr_addr == 1);
  oc8_as_sfile_dir_align(sf, 0x2);
  REQUIRE(sf->curr_addr == 2);
  oc8_as_sfile_dir_align(sf, 0x2);
  REQUIRE(sf->curr_addr == 2);

  oc8_as_sfile_dir_align(sf, 0x4);
  REQUIRE(sf->curr_addr == 4);
  oc8_as_sfile_dir_byte(sf, 0);
  REQUIRE(sf->curr_addr == 5);
  oc8_as_sfile_dir_align(sf, 0x4);
  REQUIRE(sf->curr_addr == 8);
  oc8_as_sfile_dir_align(sf, 0x4);
  REQUIRE(sf->curr_addr == 8);

  oc8_as_sfile_dir_word(sf, 0);
  REQUIRE(sf->curr_addr == 10);
  oc8_as_sfile_dir_align(sf, 0x4);
  REQUIRE(sf->curr_addr == 12);
  oc8_as_sfile_ins_cls(sf);
  REQUIRE(sf->curr_addr == 14);

  oc8_as_sfile_free(sf);
}

TEST_CASE("sfile function fibo", "") {
  oc8_as_sfile_t *sf = oc8_as_sfile_new();

  oc8_as_sfile_dir_globl(sf, "fibo");
  oc8_as_sfile_dir_type(sf, "fibo", OC8_AS_DATA_SYM_TYPE_FUN);
  oc8_as_sfile_add_sym(sf, "fibo");
  oc8_as_sfile_ins_mov_imm(sf, 0, 0x1);
  oc8_as_sfile_ins_mov_imm(sf, 1, 0x2);

  oc8_as_sfile_add_sym(sf, "L0");
  oc8_as_sfile_ins_skpn_imm(sf, 0, 0x0);
  oc8_as_sfile_sins_jmp(sf, "L1");
  oc8_as_sfile_ins_mov(sf, 0x2, 0x3);
  oc8_as_sfile_ins_add(sf, 0x1, 0x2);
  oc8_as_sfile_ins_mov(sf, 0x3, 0x1);
  oc8_as_sfile_ins_add_imm(sf, 0xFF, 0x0);
  oc8_as_sfile_sins_jmp(sf, "L0");

  oc8_as_sfile_add_sym(sf, "L1");
  oc8_as_sfile_ins_mov(sf, 0x1, 0x0);
  oc8_as_sfile_ins_ret(sf);
  auto code = split(trim(print_short(sf)), '\n');

  REQUIRE(code.size() == 16);
  REQUIRE(trim(code[0]) == ".type fibo, @function");
  REQUIRE(trim(code[1]) == ".globl fibo");
  REQUIRE(trim(code[2]) == "fibo:");
  REQUIRE(trim(code[3]) == "mov 0x0, %v1");
  REQUIRE(trim(code[4]) == "mov 0x1, %v2");
  REQUIRE(trim(code[5]) == "L0:");
  REQUIRE(trim(code[6]) == "skpn 0x0, %v0");
  REQUIRE(trim(code[7]) == "jmp L1");
  REQUIRE(trim(code[8]) == "mov %v2, %v3");
  REQUIRE(trim(code[9]) == "add %v1, %v2");
  REQUIRE(trim(code[10]) == "mov %v3, %v1");
  REQUIRE(trim(code[11]) == "add 0xFF, %v0");
  REQUIRE(trim(code[12]) == "jmp L0");
  REQUIRE(trim(code[13]) == "L1:");
  REQUIRE(trim(code[14]) == "mov %v1, %v0");
  REQUIRE(trim(code[15]) == "ret");
  oc8_as_sfile_free(sf);
}
