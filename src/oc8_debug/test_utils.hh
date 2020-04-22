#include <catch2/catch.hpp>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include "../../tests/test_src.h"
#include "oc8_as/as.h"
#include "oc8_as/parser.h"
#include "oc8_as/sfile.h"
#include "oc8_bin/bin_writer.h"
#include "oc8_bin/file.h"
#include "oc8_debug/vm_api.h"
#include "oc8_emu/cpu.h"
#include "oc8_emu/mem.h"
#include "oc8_ld/linker.h"

#define BIN_OC8 std::string(BUILD_DIR "/bin/oc8-emu")

inline void write_file_str(const std::string &path, const std::string &data) {
  FILE *f = std::fopen(path.c_str(), "wb");
  REQUIRE(std::fwrite(data.c_str(), 1, data.size(), f) == data.size());
  std::fclose(f);
}

inline std::string read_file_str(const std::string &path) {
  FILE *f = std::fopen(path.c_str(), "rb");
  REQUIRE(std::fseek(f, 0, SEEK_END) == 0);
  size_t size = std::ftell(f);
  REQUIRE(std::fseek(f, 0, SEEK_SET) == 0);

  std::string res;
  res.resize(size);
  REQUIRE(std::fread(&res[0], 1, size, f) == size);
  std::fclose(f);
  return res;
}

enum class SimpleCLIMode { ON_SERVER };

inline std::string run_simplecli_onserver(const std::string &rom_path,
                                          const std::string &db_cmds) {
  const char *tmp_in_file = "/tmp/test_oc8_odb_simplecli_onserver_input.txt";
  const char *tmp_out_file = "/tmp/test_oc8_odb_simplecli_onserver_output.txt";

  std::remove(tmp_in_file);
  std::remove(tmp_out_file);

  write_file_str(tmp_in_file, db_cmds);

  std::string cmd = "ODB_CONF_ENABLED=1 ODB_CONF_MODE_SERVER_CLI=1 " + BIN_OC8 +
                    " " + rom_path + " --no-gui < " + std::string(tmp_in_file) +
                    " > " + std::string(tmp_out_file);
  std::system(cmd.c_str());
  return read_file_str(tmp_out_file);
}

inline std::string run_simplecli(SimpleCLIMode mode,
                                 const std::string &rom_path,
                                 const std::string &db_cmds) {
  switch (mode) {
  case SimpleCLIMode::ON_SERVER:
    return run_simplecli_onserver(rom_path, db_cmds);
  };

  return "???";
}

inline std::vector<std::string> str_split(const std::string &str, char sep) {
  std::istringstream is(str);
  std::vector<std::string> res;

  std::string line;
  while (std::getline(is, line, sep))
    res.push_back(line);
  return res;
}

inline void compile_str(const std::vector<const char *> &code,
                        oc8_bin_file_t *bf) {
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

inline void compile_str_to_file(const std::vector<const char *> &code,
                                const std::string &out_path) {
  oc8_bin_file_t bf;
  compile_str(code, &bf);
  oc8_bin_write_to_file(&bf, out_path.c_str());
  oc8_bin_file_free(&bf);
}
