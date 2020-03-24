#include <catch2/catch.hpp>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <map>
#include <string>
#include <vector>

#include <iostream>

#include "oc8_emu/oc8_emu.h"
#include "oc8_is/oc8_is.h"
#include "test_utils.hh"

namespace {

void setup_inf_loop() { EnvBuilder::get().opcodes("1200").run(0); }

/// compute the expected nb of us to do `n` cycles
std::uint64_t exp_time(std::uint64_t n) {
  return n * 1000 * 1000 / g_oc8_emu_cpu.cpu_speed;
}

/// Run `n` cycles and compute how many us it takes
std::uint64_t true_time(std::uint64_t n) {
  auto begin = std::chrono::high_resolution_clock::now();

  for (std::size_t i = 0; i < n; ++i)
    oc8_emu_cpu_cycle();

  auto end = std::chrono::high_resolution_clock::now();
  auto dur = std::chrono::duration_cast<std::chrono::microseconds>(end - begin)
                 .count();
  return dur;
}

// Expected time in us for the Delay Timer to go from n to 0
std::uint64_t exp_time_dt(std::uint64_t n) { return n * 1000 * 1000 / 60; }

/// Compute how many us it takes for Delay Timer to go from n to 0
std::uint64_t true_time_dt(int freq, std::uint64_t n) {
  setup_inf_loop();
  g_oc8_emu_cpu.cpu_speed = freq;
  g_oc8_emu_cpu.reg_dt = n;
  auto begin = std::chrono::high_resolution_clock::now();

  while (g_oc8_emu_cpu.reg_dt != 0)
    oc8_emu_cpu_cycle();

  auto end = std::chrono::high_resolution_clock::now();
  auto dur = std::chrono::duration_cast<std::chrono::microseconds>(end - begin)
                 .count();
  return dur;
}

/// Test the CPU frequency using many samples
/// @param freq - The frequency of the CPU
/// @param ncycles - number of cycles run for each sample
/// @param nsamples - number of samples run
/// @param erange - correct range = [exp - exp/erange, exp + exp/erange]
/// @param nvalid - min number of correct samples to validate the test
void test_freq(int freq, std::uint64_t ncycles, std::size_t nsamples,
               std::size_t erange, std::size_t nvalid) {
  setup_inf_loop();
  g_oc8_emu_cpu.cpu_speed = freq;

  // build error range
  auto exp = exp_time(ncycles);
  auto exp_low = exp - exp / erange;
  auto exp_high = exp + exp / erange;

  // run samples
  std::size_t nok = 0;
  for (std::size_t i = 0; i < nsamples; ++i) {
    auto t = true_time(ncycles);
    nok += t >= exp_low && t <= exp_high;
  }

  REQUIRE(nok >= nvalid);
}

/// Test the Delay Timer register using many samples
/// @param freq - The frequency of the CPU
/// @param dt_val - Start value of Delay Timer register
/// @param nsamples - number of samples run
/// @param erange - correct range = [exp - exp/erange, exp + exp/erange]
/// @param nvalid - min number of correct samples to validate the test
void test_delay_reg(int freq, std::uint64_t dt_val, std::size_t nsamples,
                    std::size_t erange, std::size_t nvalid) {
  // build error range
  auto exp = exp_time_dt(dt_val);
  auto exp_low = exp - exp / erange;
  auto exp_high = exp + exp / erange;

  // run samples
  std::size_t nok = 0;
  for (std::size_t i = 0; i < nsamples; ++i) {
    auto t = true_time_dt(freq, dt_val);
    nok += t >= exp_low && t <= exp_high;
  }

  REQUIRE(nok >= nvalid);
}

} // namespace

TEST_CASE("Timing 250Hz", "") {
  test_freq(/*freq=*/250, /*nycles=*/50, /*nsamples=*/10, /*erange=*/100,
            /*nvalid=*/8);
}

TEST_CASE("Timing 500Hz", "") {
  test_freq(/*freq=*/500, /*nycles=*/50, /*nsamples=*/10, /*erange=*/100,
            /*nvalid=*/8);
}

TEST_CASE("Timing 1000Hz", "") {
  test_freq(/*freq=*/1000, /*nycles=*/50, /*nsamples=*/10, /*erange=*/100,
            /*nvalid=*/8);
}

TEST_CASE("Timing 2000Hz", "") {
  test_freq(/*freq=*/2000, /*nycles=*/50, /*nsamples=*/10, /*erange=*/100,
            /*nvalid=*/8);
}

TEST_CASE("Delay Timer Register 250Hz", "") {
  test_delay_reg(/*freq=*/250, /*dt_val=*/10, /*nsamples=*/10, /*erange=*/100,
                 /*nvalid=*/8);
}

TEST_CASE("Delay Timer Register 500Hz", "") {
  test_delay_reg(/*freq=*/500, /*dt_val=*/10, /*nsamples=*/10, /*erange=*/100,
                 /*nvalid=*/8);
}

TEST_CASE("Delay Timer Register 1000Hz", "") {
  test_delay_reg(/*freq=*/1000, /*dt_val=*/10, /*nsamples=*/10, /*erange=*/100,
                 /*nvalid=*/8);
}

TEST_CASE("Delay Timer Register 200Hz", "") {
  test_delay_reg(/*freq=*/2000, /*dt_val=*/10, /*nsamples=*/10, /*erange=*/100,
                 /*nvalid=*/8);
}
