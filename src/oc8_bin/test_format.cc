#include <catch2/catch.hpp>
#include <sstream>
#include <string>
#include <vector>

#include "oc8_as/as.h"
#include "oc8_as/parser.h"
#include "oc8_as/sfile.h"
#include "oc8_bin/bin_reader.h"
#include "oc8_bin/bin_writer.h"
#include "oc8_bin/file.h"

#include "../../tests/test_src.h"

namespace {

oc8_as_sfile_t *parse_str(const std::string &str) {
  return oc8_as_parse_raw(str.c_str(), str.size());
}

void test_read_write(const char *code) {
  oc8_as_sfile_t *sf = parse_str(code);
  oc8_bin_file_t bf;
  oc8_as_sfile_check(sf);
  oc8_as_compile_sfile(sf, &bf);
  oc8_bin_file_check(&bf, 0);

  // Write 1
  size_t len1 = oc8_bin_write_file_raw(&bf, NULL);
  char *buff1 = (char *)malloc(len1);
  oc8_bin_write_file_raw(&bf, buff1);

  // Read 1
  oc8_bin_file_t bf2;
  oc8_bin_read_file_raw(&bf2, buff1, len1);
  oc8_bin_file_check(&bf2, 0);

  // Write 2
  size_t len2 = oc8_bin_write_file_raw(&bf2, NULL);
  REQUIRE(len1 == len2);
  char *buff2 = (char *)malloc(len2);
  oc8_bin_write_file_raw(&bf2, buff2);

  for (size_t i = 0; i < len1; ++i)
    REQUIRE(buff1[i] == buff2[i]);

  free(buff1);
  free(buff2);
  oc8_bin_file_free(&bf);
  oc8_bin_file_free(&bf2);
  oc8_as_sfile_free(sf);
}

} // namespace

TEST_CASE("format function fibo", "") { test_read_write(test_fibo_src); }

TEST_CASE("format function my_add", "") { test_read_write(test_my_add_src); }

TEST_CASE("format function sum_rec", "") { test_read_write(test_sum_rec_src); }

TEST_CASE("format call_add", "") { test_read_write(test_call_add_src); }

TEST_CASE("format call_add_mem", "") { test_read_write(test_call_add_mem_src); }

TEST_CASE("format function fact_table", "") {
  test_read_write(test_fact_table_src);
}
