#pragma once

#include "oc8_emu/oc8_emu.h"
#include "oc8_is/oc8_is.h"

#define OPCODE_SWAP(X) ((X << 8) | (X >> 8))

class EnvBuilder {
public:
  static EnvBuilder get() { return EnvBuilder{}; }

  // Add opcodes, separted by spaces
  EnvBuilder &opcodes(std::string str) {
    for (;;) {
      auto spos = str.find(' ');
      std::string ins = spos == std::string::npos ? str : str.substr(0, spos);

      uint16_t op16 = std::stoi(ins, nullptr, 16);
      // need to swap to big-endian
      op16 = OPCODE_SWAP(op16);
      _code.push_back(op16);

      if (spos == std::string::npos)
        break;
      str = str.substr(spos + 1);
    }

    return *this;
  }

  EnvBuilder &fill_screen() {
    std::memset(g_oc8_emu_screen, 0xFF, sizeof(g_oc8_emu_screen));
    return *this;
  }

  EnvBuilder &reg(std::size_t idx, uint8_t val) {
    g_oc8_emu_cpu.regs_data[idx] = val;
    return *this;
  }

  EnvBuilder &reg_i(uint16_t val) {
    g_oc8_emu_cpu.reg_i = val;
    return *this;
  }

  EnvBuilder &reg_dt(uint8_t val) {
    g_oc8_emu_cpu.reg_dt = val;
    return *this;
  }

  EnvBuilder &reg_st(uint8_t val) {
    g_oc8_emu_cpu.reg_st = val;
    return *this;
  }

  EnvBuilder &reg_pc(uint16_t val) {
    g_oc8_emu_cpu.reg_pc = val;
    return *this;
  }

  EnvBuilder &reg(const std::map<std::size_t, uint8_t> &vals) {
    for (const auto &it : vals) {
      g_oc8_emu_cpu.regs_data[it.first] = it.second;
    }
    return *this;
  }

  EnvBuilder &seed(unsigned long s) {
    g_oc8_emu_cpu.rg_seed = s;
    return *this;
  }

  EnvBuilder &sprite(size_t addr, const char *data) {
    size_t len = strlen(data);
    size_t nbytes = len / 8;
    for (size_t i = 0; i < nbytes; ++i) {
      uint8_t val = 0;
      for (size_t j = 0; j < 8; ++j)
        val = (val << 1) | (data[i * 8 + j] == '1');
      g_oc8_emu_mem.ram[addr + i] = val;
    }
    return *this;
  }

  EnvBuilder &byte(std::size_t addr, uint8_t val) {
    g_oc8_emu_mem.ram[addr] = val;
    return *this;
  }

  EnvBuilder &byte(const std::map<std::size_t, uint8_t> &vals) {
    for (auto &it : vals)
      g_oc8_emu_mem.ram[it.first] = it.second;
    return *this;
  }

  EnvBuilder &keyp(size_t key) {
    g_oc8_emu_keypad[key] = 1;
    return *this;
  }

  void run(std::size_t nb_steps = 1) {
    oc8_emu_load_rom((const void *)&_code[0], _code.size() * 2);
    for (std::size_t i = 0; i < nb_steps; ++i)
      oc8_emu_cpu_step();
  }

private:
  EnvBuilder() { oc8_emu_init(); }
  std::vector<uint16_t> _code;
};
