#include <catch2/catch.hpp>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

#include "oc8_as/as.h"
#include "oc8_as/parser.h"
#include "oc8_as/sfile.h"
#include "oc8_bin/bin_reader.h"
#include "oc8_bin/file.h"
#include "oc8_emu/cpu.h"
#include "oc8_emu/mem.h"
#include "oc8_is/oc8_is.h"
#include "oc8_ld/linker.h"

#include "../../tests/test_src.h"

#define BIN_AS BUILD_DIR "/bin/oc8-as"
#define BIN_OBJD BUILD_DIR "/bin/oc8-objdump"
#define TMP_AS_FILE "/tmp/oc8_test_objdump_in.c8s"
#define TMP_OBJ_FILE "/tmp/oc8_test_objdump_in.c88"

namespace {

void write_bin(const std::string &path, const void *buf, size_t len) {
  FILE *os = std::fopen(path.c_str(), "wb");
  REQUIRE(os);
  REQUIRE(std::fwrite(buf, 1, len, os) == len);
  fclose(os);
}

void run_objdump(const char *code) {
  write_bin(TMP_AS_FILE, code, std::strlen(code));

  std::string as_cmd = BIN_AS + std::string(" ") + std::string(TMP_AS_FILE) +
                       " -o " + std::string(TMP_OBJ_FILE);
  REQUIRE(std::system(as_cmd.c_str()) == 0);

  std::string od_cmd =
      BIN_OBJD + std::string(" ") + TMP_OBJ_FILE + std::string(" > /dev/null");
  REQUIRE(std::system(od_cmd.c_str()) == 0);
}

} // namespace

TEST_CASE("objdump fibo_src", "") { run_objdump(test_fibo_src); }

TEST_CASE("objdump my_add_src", "") { run_objdump(test_my_add_src); }

TEST_CASE("objdump sum_rec_src", "") { run_objdump(test_sum_rec_src); }

TEST_CASE("objdump call_add_src", "") { run_objdump(test_call_add_src); }

TEST_CASE("objdump call_add_mem_src", "") {
  run_objdump(test_call_add_mem_src);
}

TEST_CASE("objdump fact_table_src", "") { run_objdump(test_fact_table_src); }
