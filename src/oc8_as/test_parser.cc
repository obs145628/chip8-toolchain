#include <catch2/catch.hpp>
#include <sstream>
#include <string>
#include <vector>

#include "oc8_as/parser.h"
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

#if 0
std::vector<std::string> split(const std::string &str, char sep) {
  std::vector<std::string> res;
  std::istringstream is(str);
  std::string val;
  while (std::getline(is, val, sep))
    res.push_back(val);
  return res;
}
#endif

std::string print_short(oc8_as_sfile_t *sf) {
  char buff[512];
  oc8_as_sfile_check(sf);
  auto len = oc8_as_print_sfile(sf, buff, sizeof(buff), NULL, NULL);
  buff[len] = '\0';
  return std::string(buff);
}

oc8_as_sfile_t *parse_str(const std::string &str) {
  return oc8_as_parse_raw(str.c_str(), str.size());
}

} // namespace

TEST_CASE("parse ins 7XNN", "") {
  oc8_as_sfile_t *sf = parse_str("add 0x73, %vb");
  REQUIRE(trim(print_short(sf)) == "add 0x73, %vb");
  oc8_as_sfile_free(sf);
}

TEST_CASE("parse ins 8XY4", "") {
  oc8_as_sfile_t *sf = parse_str("add %v1, %vc");
  REQUIRE(trim(print_short(sf)) == "add %v1, %vc");
  oc8_as_sfile_free(sf);
}

TEST_CASE("parse ins FX1E", "") {
  oc8_as_sfile_t *sf = parse_str("add %v6, %i");
  REQUIRE(trim(print_short(sf)) == "add %v6, %i");
  oc8_as_sfile_free(sf);
}

TEST_CASE("parse ins 8XY2", "") {
  oc8_as_sfile_t *sf = parse_str("and %v2, %v4");
  REQUIRE(trim(print_short(sf)) == "and %v2, %v4");
  oc8_as_sfile_free(sf);
}

TEST_CASE("parse ins FX33", "") {
  oc8_as_sfile_t *sf = parse_str("bcd %ve");
  REQUIRE(trim(print_short(sf)) == "bcd %ve");
  oc8_as_sfile_free(sf);
}

TEST_CASE("parse ins 2NNN", "") {
  oc8_as_sfile_t *sf = parse_str("call 0x738");
  REQUIRE(trim(print_short(sf)) == "call 0x738");
  oc8_as_sfile_free(sf);
}

TEST_CASE("parse ins 00E0", "") {
  oc8_as_sfile_t *sf = parse_str("cls");
  REQUIRE(trim(print_short(sf)) == "cls");
  oc8_as_sfile_free(sf);
}

TEST_CASE("parse ins DXYN", "") {
  oc8_as_sfile_t *sf = parse_str("draw %ve, %v6, 0xA");
  REQUIRE(trim(print_short(sf)) == "draw %ve, %v6, 0xA");
  oc8_as_sfile_free(sf);
}

TEST_CASE("parse ins FX29", "") {
  oc8_as_sfile_t *sf = parse_str("fspr %vb");
  REQUIRE(trim(print_short(sf)) == "fspr %vb");
  oc8_as_sfile_free(sf);
}

TEST_CASE("parse ins 1NNN", "") {
  oc8_as_sfile_t *sf = parse_str("jmp 0xF1F");
  REQUIRE(trim(print_short(sf)) == "jmp 0xF1F");
  oc8_as_sfile_free(sf);
}

TEST_CASE("parse ins BNNN", "") {
  oc8_as_sfile_t *sf = parse_str("jmp 0xBCD(%v0)");
  REQUIRE(trim(print_short(sf)) == "jmp 0xBCD(%v0)");
  oc8_as_sfile_free(sf);
}

TEST_CASE("parse ins 6XNN", "") {
  oc8_as_sfile_t *sf = parse_str("mov 0x79, %vc");
  REQUIRE(trim(print_short(sf)) == "mov 0x79, %vc");
  oc8_as_sfile_free(sf);
}

TEST_CASE("parse ins 8XY0", "") {
  oc8_as_sfile_t *sf = parse_str("mov %v5, %v0");
  REQUIRE(trim(print_short(sf)) == "mov %v5, %v0");
  oc8_as_sfile_free(sf);
}

TEST_CASE("parse ins ANNN", "") {
  oc8_as_sfile_t *sf = parse_str("mov 0x790, %i");
  REQUIRE(trim(print_short(sf)) == "mov 0x790, %i");
  oc8_as_sfile_free(sf);
}

TEST_CASE("parse ins FX07", "") {
  oc8_as_sfile_t *sf = parse_str("mov %dt, %vc");
  REQUIRE(trim(print_short(sf)) == "mov %dt, %vc");
  oc8_as_sfile_free(sf);
}

TEST_CASE("parse ins FX15", "") {
  oc8_as_sfile_t *sf = parse_str("mov %v1, %dt");
  REQUIRE(trim(print_short(sf)) == "mov %v1, %dt");
  oc8_as_sfile_free(sf);
}

TEST_CASE("parse ins FX18", "") {
  oc8_as_sfile_t *sf = parse_str("mov %vf, %st");
  REQUIRE(trim(print_short(sf)) == "mov %vf, %st");
  oc8_as_sfile_free(sf);
}

TEST_CASE("parse ins FX55", "") {
  oc8_as_sfile_t *sf = parse_str("movm %v4, %i");
  REQUIRE(trim(print_short(sf)) == "movm %v4, %i");
  oc8_as_sfile_free(sf);
}

TEST_CASE("parse ins FX65", "") {
  oc8_as_sfile_t *sf = parse_str("movm %i, %v4");
  REQUIRE(trim(print_short(sf)) == "movm %i, %v4");
  oc8_as_sfile_free(sf);
}

TEST_CASE("parse ins 8XY1", "") {
  oc8_as_sfile_t *sf = parse_str("or %v4, %vb");
  REQUIRE(trim(print_short(sf)) == "or %v4, %vb");
  oc8_as_sfile_free(sf);
}

TEST_CASE("parse ins CXNN", "") {
  oc8_as_sfile_t *sf = parse_str("rand 0x56, %v7");
  REQUIRE(trim(print_short(sf)) == "rand 0x56, %v7");
  oc8_as_sfile_free(sf);
}

TEST_CASE("parse ins 00EE", "") {
  oc8_as_sfile_t *sf = parse_str("ret");
  REQUIRE(trim(print_short(sf)) == "ret");
  oc8_as_sfile_free(sf);
}

TEST_CASE("parse ins 8XYE", "") {
  oc8_as_sfile_t *sf = parse_str("shl %v2, %v1");
  REQUIRE(trim(print_short(sf)) == "shl %v2, %v1");
  oc8_as_sfile_free(sf);
}

TEST_CASE("parse ins 8XY6", "") {
  oc8_as_sfile_t *sf = parse_str("shr %v2, %v1");
  REQUIRE(trim(print_short(sf)) == "shr %v2, %v1");
  oc8_as_sfile_free(sf);
}

TEST_CASE("parse ins 3XNN", "") {
  oc8_as_sfile_t *sf = parse_str("skpe 0x67, %ve");
  REQUIRE(trim(print_short(sf)) == "skpe 0x67, %ve");
  oc8_as_sfile_free(sf);
}

TEST_CASE("parse ins 5XY0", "") {
  oc8_as_sfile_t *sf = parse_str("skpe %vc, %ve");
  REQUIRE(trim(print_short(sf)) == "skpe %vc, %ve");
  oc8_as_sfile_free(sf);
}

TEST_CASE("parse ins 4XNN", "") {
  oc8_as_sfile_t *sf = parse_str("skpn 0x67, %ve");
  REQUIRE(trim(print_short(sf)) == "skpn 0x67, %ve");
  oc8_as_sfile_free(sf);
}

TEST_CASE("parse ins 9XY0", "") {
  oc8_as_sfile_t *sf = parse_str("skpn %vc, %ve");
  REQUIRE(trim(print_short(sf)) == "skpn %vc, %ve");
  oc8_as_sfile_free(sf);
}

TEST_CASE("parse ins EX9E", "") {
  oc8_as_sfile_t *sf = parse_str("skpkp %v8");
  REQUIRE(trim(print_short(sf)) == "skpkp %v8");
  oc8_as_sfile_free(sf);
}

TEST_CASE("parse ins EXAI", "") {
  oc8_as_sfile_t *sf = parse_str("skpkn %v8");
  REQUIRE(trim(print_short(sf)) == "skpkn %v8");
  oc8_as_sfile_free(sf);
}

TEST_CASE("parse ins 8XY5", "") {
  oc8_as_sfile_t *sf = parse_str("sub %v8, %vc");
  REQUIRE(trim(print_short(sf)) == "sub %v8, %vc");
  oc8_as_sfile_free(sf);
}

TEST_CASE("parse ins 8XY7", "") {
  oc8_as_sfile_t *sf = parse_str("subn %v8, %vc");
  REQUIRE(trim(print_short(sf)) == "subn %v8, %vc");
  oc8_as_sfile_free(sf);
}

TEST_CASE("parse ins 0NNN", "") {
  oc8_as_sfile_t *sf = parse_str("sys 0x843");
  REQUIRE(trim(print_short(sf)) == "sys 0x843");
  oc8_as_sfile_free(sf);
}

TEST_CASE("parse ins FX0A", "") {
  oc8_as_sfile_t *sf = parse_str("waitk %v6");
  REQUIRE(trim(print_short(sf)) == "waitk %v6");
  oc8_as_sfile_free(sf);
}

TEST_CASE("parse ins 8XY3", "") {
  oc8_as_sfile_t *sf = parse_str("xor %v9, %vc");
  REQUIRE(trim(print_short(sf)) == "xor %v9, %vc");
  oc8_as_sfile_free(sf);
}

#if 0

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

#endif
