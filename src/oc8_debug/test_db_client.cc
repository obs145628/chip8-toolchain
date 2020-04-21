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
                     "step\n";
  auto vals = str_split(run_simplecli(mode, CALL_ADD_TMP_PATH, cmds), '\n');
  REQUIRE(vals.size() == 2);
  REQUIRE(vals[0] == "");
}

} // namespace

TEST_CASE("simplecli_on_server call_add infos", "") {
  test_call_add_infos(SimpleCLIMode::ON_SERVER);
}
