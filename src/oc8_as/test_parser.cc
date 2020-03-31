#include <catch2/catch.hpp>
#include <sstream>
#include <string>
#include <vector>

#include "oc8_as/parser.h"
#include "oc8_as/printer.h"
#include "oc8_as/sfile.h"

#include "../../tests/test_src.h"

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

TEST_CASE("parse dir byte", "") {
  oc8_as_sfile_t *sf = parse_str(".byte 0xF6");
  REQUIRE(trim(print_short(sf)) == ".byte 0xF6");
  oc8_as_sfile_free(sf);
}

TEST_CASE("parse dir word", "") {
  oc8_as_sfile_t *sf = parse_str(".word 0xF6C4");
  REQUIRE(trim(print_short(sf)) == ".word 0xF6C4");
  oc8_as_sfile_free(sf);
}

TEST_CASE("parse dir zero", "") {
  oc8_as_sfile_t *sf = parse_str(".zero 38");
  REQUIRE(trim(print_short(sf)) == ".zero 38");
  oc8_as_sfile_free(sf);
}

TEST_CASE("parse sins 7XNN", "") {
  oc8_as_sfile_t *sf = parse_str("add foo, %vb");
  REQUIRE(trim(print_short(sf)) == "add foo, %vb");
  oc8_as_sfile_free(sf);
}

TEST_CASE("parse sins 2NNN", "") {
  oc8_as_sfile_t *sf = parse_str("call foo");
  REQUIRE(trim(print_short(sf)) == "call foo");
  oc8_as_sfile_free(sf);
}

TEST_CASE("parse sins DXYN", "") {
  oc8_as_sfile_t *sf = parse_str("draw %ve, %v6, foo");
  REQUIRE(trim(print_short(sf)) == "draw %ve, %v6, foo");
  oc8_as_sfile_free(sf);
}

TEST_CASE("parse sins 1NNN", "") {
  oc8_as_sfile_t *sf = parse_str("jmp bar");
  REQUIRE(trim(print_short(sf)) == "jmp bar");
  oc8_as_sfile_free(sf);
}

TEST_CASE("parse sins BNNN", "") {
  oc8_as_sfile_t *sf = parse_str("jmp foo(%v0)");
  REQUIRE(trim(print_short(sf)) == "jmp foo(%v0)");
  oc8_as_sfile_free(sf);
}

TEST_CASE("parse sins 6XNN", "") {
  oc8_as_sfile_t *sf = parse_str("mov foo, %vc");
  REQUIRE(trim(print_short(sf)) == "mov foo, %vc");
  oc8_as_sfile_free(sf);
}

TEST_CASE("parse sins ANNN", "") {
  oc8_as_sfile_t *sf = parse_str("mov foo, %i");
  REQUIRE(trim(print_short(sf)) == "mov foo, %i");
  oc8_as_sfile_free(sf);
}

TEST_CASE("parse sins CXNN", "") {
  oc8_as_sfile_t *sf = parse_str("rand bar, %v7");
  REQUIRE(trim(print_short(sf)) == "rand bar, %v7");
  oc8_as_sfile_free(sf);
}

TEST_CASE("parse sins 3XNN", "") {
  oc8_as_sfile_t *sf = parse_str("skpe foo, %ve");
  REQUIRE(trim(print_short(sf)) == "skpe foo, %ve");
  oc8_as_sfile_free(sf);
}

TEST_CASE("parse sins 4XNN", "") {
  oc8_as_sfile_t *sf = parse_str("skpn foo, %ve");
  REQUIRE(trim(print_short(sf)) == "skpn foo, %ve");
  oc8_as_sfile_free(sf);
}

TEST_CASE("parse sins 0NNN", "") {
  oc8_as_sfile_t *sf = parse_str("sys __syscall");
  REQUIRE(trim(print_short(sf)) == "sys __syscall");
  oc8_as_sfile_free(sf);
}

TEST_CASE("parse simple label", "") {
  oc8_as_sfile_t *sf = parse_str("  cls\n"
                                 "foo:\n"
                                 "ret\n");
  auto code = split(trim(print_short(sf)), '\n');

  REQUIRE(code.size() == 3);
  REQUIRE(trim(code[0]) == "cls");
  REQUIRE(trim(code[1]) == "foo:");
  REQUIRE(trim(code[2]) == "ret");
  oc8_as_sfile_free(sf);
}

TEST_CASE("parse infos label", "") {
  oc8_as_sfile_t *sf = parse_str("  .globl foo\n"
                                 "  cls\n"
                                 "  .size foo, 8\n"
                                 "foo:\n"
                                 "  ret\n"
                                 "  .type foo, @function\n");
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

TEST_CASE("parse function fibo", "") {
  oc8_as_sfile_t *sf = parse_str(test_fibo_src);
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

TEST_CASE("parse function my_add", "") {
  oc8_as_sfile_t *sf = parse_str(test_my_add_src);
  auto code = split(trim(print_short(sf)), '\n');

  REQUIRE(code.size() == 5);
  REQUIRE(trim(code[0]) == ".type my_add, @function");
  REQUIRE(trim(code[1]) == ".globl my_add");
  REQUIRE(trim(code[2]) == "my_add:");
  REQUIRE(trim(code[3]) == "add %v1, %v0");
  REQUIRE(trim(code[4]) == "ret");
  oc8_as_sfile_free(sf);
}

TEST_CASE("parse function sum_rec", "") {
  oc8_as_sfile_t *sf = parse_str(test_sum_rec_src);
  auto code = split(trim(print_short(sf)), '\n');

  REQUIRE(code.size() == 10);
  REQUIRE(trim(code[0]) == ".type sum_rec, @function");
  REQUIRE(trim(code[1]) == ".globl sum_rec");
  REQUIRE(trim(code[2]) == "sum_rec:");
  REQUIRE(trim(code[3]) == "skpe 0x0, %v0");
  REQUIRE(trim(code[4]) == "ret");
  REQUIRE(trim(code[5]) == "mov %v0, %v4");
  REQUIRE(trim(code[6]) == "add 0x1, %v0");
  REQUIRE(trim(code[7]) == "call sum_rec");
  REQUIRE(trim(code[8]) == "add %v4, %v0");
  REQUIRE(trim(code[9]) == "ret");
  oc8_as_sfile_free(sf);
}

TEST_CASE("parse file call_add", "") {
  oc8_as_sfile_t *sf = parse_str(test_call_add_src);
  auto code = split(trim(print_short(sf)), '\n');

  REQUIRE(code.size() == 5);
  REQUIRE(trim(code[0]) == "_start:");
  REQUIRE(trim(code[1]) == "mov 0x6, %v0");
  REQUIRE(trim(code[2]) == "mov 0x8, %v1");
  REQUIRE(trim(code[3]) == "call my_add");
  REQUIRE(trim(code[4]) == "mov %v0, %vf");
  oc8_as_sfile_free(sf);
}

TEST_CASE("parse file call_add_mem", "") {
  oc8_as_sfile_t *sf = parse_str(test_call_add_mem_src);
  auto code = split(trim(print_short(sf)), '\n');

  REQUIRE(code.size() == 10);
  REQUIRE(trim(code[0]) == ".type _start, @function");
  REQUIRE(trim(code[1]) == "_start:");
  REQUIRE(trim(code[2]) == "mov args, %i");
  REQUIRE(trim(code[3]) == "movm %i, %v1");
  REQUIRE(trim(code[4]) == "call my_add");
  REQUIRE(trim(code[5]) == "mov %v0, %vf");
  REQUIRE(trim(code[6]) == ".type args, @object");
  REQUIRE(trim(code[7]) == "args:");
  REQUIRE(trim(code[8]) == ".byte 0x8");
  REQUIRE(trim(code[9]) == ".byte 0xD");
  oc8_as_sfile_free(sf);
}

TEST_CASE("parse file fact_table_src", "") {
  oc8_as_sfile_t *sf = parse_str(test_fact_table_src);
  auto code = split(trim(print_short(sf)), '\n');

  REQUIRE(code.size() == 27);
  REQUIRE(trim(code[0]) == ".type fact, @function");
  REQUIRE(trim(code[1]) == ".globl fact");
  REQUIRE(trim(code[2]) == "fact:");
  REQUIRE(trim(code[3]) == "mov %v0, %v1");
  REQUIRE(trim(code[4]) == "add 0x2, %v1");
  REQUIRE(trim(code[5]) == "mov 0xF8, %v2");
  REQUIRE(trim(code[6]) == "and %v2, %v1");
  REQUIRE(trim(code[7]) == "skpn 0x0, %v1");
  REQUIRE(trim(code[8]) == "jmp Ltable");
  REQUIRE(trim(code[9]) == "Lmax:");
  REQUIRE(trim(code[10]) == "mov 0xFF, %v0");
  REQUIRE(trim(code[11]) == "jmp Lend");
  REQUIRE(trim(code[12]) == "Ltable:");
  REQUIRE(trim(code[13]) == "mov table, %i");
  REQUIRE(trim(code[14]) == "add %v0, %i");
  REQUIRE(trim(code[15]) == "movm %i, %v0");
  REQUIRE(trim(code[16]) == "Lend:");
  REQUIRE(trim(code[17]) == "ret");
  REQUIRE(trim(code[18]) == ".type table, @object");
  REQUIRE(trim(code[19]) == "table:");
  REQUIRE(trim(code[20]) == ".byte 0x1");
  REQUIRE(trim(code[21]) == ".byte 0x1");
  REQUIRE(trim(code[22]) == ".byte 0x2");
  REQUIRE(trim(code[23]) == ".byte 0x3");
  REQUIRE(trim(code[24]) == ".byte 0x6");
  REQUIRE(trim(code[25]) == ".byte 0x18");
  REQUIRE(trim(code[26]) == ".byte 0x78");
  oc8_as_sfile_free(sf);
}
