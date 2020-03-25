#include <catch2/catch.hpp>
#include <fstream>

#include "oc8_as/stream.h"

#define TMP_FILE "/tmp/ts_oc8_as.txt"

namespace {

void save_str(const char *data) {
  std::ofstream os(TMP_FILE);
  os << data;
}

} // namespace

TEST_CASE("Stream get str from raw", "") {
  oc8_as_stream_t is;
  oc8_as_stream_init_from_raw(&is, "hello", 5);
  REQUIRE(oc8_as_stream_get(&is) == 'h');
  REQUIRE(oc8_as_stream_get(&is) == 'e');
  REQUIRE(oc8_as_stream_get(&is) == 'l');
  REQUIRE(oc8_as_stream_get(&is) == 'l');
  REQUIRE(oc8_as_stream_get(&is) == 'o');
  REQUIRE(oc8_as_stream_get(&is) == EOF);
  REQUIRE(oc8_as_stream_get(&is) == EOF);
  oc8_as_stream_free(&is);
}

TEST_CASE("Stream get str from file", "") {
  save_str("hello");

  FILE *f = fopen(TMP_FILE, "r");
  oc8_as_stream_t is;
  oc8_as_stream_init_from_file(&is, f);
  REQUIRE(oc8_as_stream_get(&is) == 'h');
  REQUIRE(oc8_as_stream_get(&is) == 'e');
  REQUIRE(oc8_as_stream_get(&is) == 'l');
  REQUIRE(oc8_as_stream_get(&is) == 'l');
  REQUIRE(oc8_as_stream_get(&is) == 'o');
  REQUIRE(oc8_as_stream_get(&is) == EOF);
  REQUIRE(oc8_as_stream_get(&is) == EOF);
  oc8_as_stream_free(&is);
  fclose(f);
}

TEST_CASE("Stream empty from raw", "") {
  oc8_as_stream_t is;
  oc8_as_stream_init_from_raw(&is, "", 0);
  REQUIRE(oc8_as_stream_peek(&is) == EOF);
  REQUIRE(oc8_as_stream_get(&is) == EOF);
  REQUIRE(oc8_as_stream_peek(&is) == EOF);
  REQUIRE(oc8_as_stream_get(&is) == EOF);
  oc8_as_stream_free(&is);
}

TEST_CASE("Stream empty from file", "") {
  save_str("");

  FILE *f = fopen(TMP_FILE, "r");
  oc8_as_stream_t is;
  oc8_as_stream_init_from_file(&is, f);
  REQUIRE(oc8_as_stream_peek(&is) == EOF);
  REQUIRE(oc8_as_stream_get(&is) == EOF);
  REQUIRE(oc8_as_stream_peek(&is) == EOF);
  REQUIRE(oc8_as_stream_get(&is) == EOF);
  oc8_as_stream_free(&is);
  fclose(f);
}

TEST_CASE("Stream get/peek str from raw", "") {
  oc8_as_stream_t is;
  oc8_as_stream_init_from_raw(&is, "helo", 4);
  REQUIRE(oc8_as_stream_peek(&is) == 'h');
  REQUIRE(oc8_as_stream_peek(&is) == 'h');
  REQUIRE(oc8_as_stream_get(&is) == 'h');
  REQUIRE(oc8_as_stream_peek(&is) == 'e');
  REQUIRE(oc8_as_stream_get(&is) == 'e');
  REQUIRE(oc8_as_stream_get(&is) == 'l');
  REQUIRE(oc8_as_stream_peek(&is) == 'o');
  REQUIRE(oc8_as_stream_get(&is) == 'o');
  REQUIRE(oc8_as_stream_get(&is) == EOF);
  REQUIRE(oc8_as_stream_get(&is) == EOF);
  REQUIRE(oc8_as_stream_peek(&is) == EOF);
  REQUIRE(oc8_as_stream_peek(&is) == EOF);
  oc8_as_stream_free(&is);
}

TEST_CASE("Stream get/peek str from string", "") {
  save_str("helo");

  FILE *f = fopen(TMP_FILE, "r");
  oc8_as_stream_t is;
  oc8_as_stream_init_from_file(&is, f);
  REQUIRE(oc8_as_stream_peek(&is) == 'h');
  REQUIRE(oc8_as_stream_peek(&is) == 'h');
  REQUIRE(oc8_as_stream_get(&is) == 'h');
  REQUIRE(oc8_as_stream_peek(&is) == 'e');
  REQUIRE(oc8_as_stream_get(&is) == 'e');
  REQUIRE(oc8_as_stream_get(&is) == 'l');
  REQUIRE(oc8_as_stream_peek(&is) == 'o');
  REQUIRE(oc8_as_stream_get(&is) == 'o');
  REQUIRE(oc8_as_stream_get(&is) == EOF);
  REQUIRE(oc8_as_stream_get(&is) == EOF);
  REQUIRE(oc8_as_stream_peek(&is) == EOF);
  REQUIRE(oc8_as_stream_peek(&is) == EOF);
  oc8_as_stream_free(&is);
  fclose(f);
}
