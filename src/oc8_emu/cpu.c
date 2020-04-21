#define _POSIX_C_SOURCE 200809L

#include "oc8_emu/cpu.h"
#include "oc8_debug/debug.h"
#include "oc8_emu/input.h"
#include "oc8_emu/mem.h"
#include "oc8_emu/screen.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define TIMER_ROUND_DURATION (1e6 / 60)
#define MIN_SLEEP_TIME_US (200)

// Implementation in exec_ins.c
void oc8_emu_exec_ins();

oc8_emu_cpu_t g_oc8_emu_cpu;

static uint64_t time_us() {
  struct timespec spec;
  clock_gettime(CLOCK_REALTIME, &spec);
  uint64_t s = spec.tv_sec;
  uint64_t us = spec.tv_nsec / 1.0e3;
  return 1e6 * s + us;
}

void oc8_emu_init_cpu() {
  // Set all registers to 0
  // Even PC, will be set to right value when ROM is loaded
  memset(&g_oc8_emu_cpu, 0, sizeof(g_oc8_emu_cpu));

  uint64_t now = time_us();
  g_oc8_emu_cpu.timer_last_update = now;
  g_oc8_emu_cpu.last_cycle_time = now;
  g_oc8_emu_cpu.cpu_speed = 500;
}

void oc8_emu_init() {
  oc8_emu_init_cpu();
  oc8_emu_init_keypad();
  oc8_emu_init_mem();
  oc8_emu_init_screen();
  oc8_debug_init();
}

static void decrease_timers(unsigned val) {
  uint8_t tval = (uint8_t)val;
  if (tval > g_oc8_emu_cpu.reg_dt)
    g_oc8_emu_cpu.reg_dt = 0;
  else
    g_oc8_emu_cpu.reg_dt -= tval;
  if (tval > g_oc8_emu_cpu.reg_st)
    g_oc8_emu_cpu.reg_st = 0;
  else
    g_oc8_emu_cpu.reg_st -= tval;
}

static void fetch_ins() {
  unsigned pc = g_oc8_emu_cpu.reg_pc;
  assert(pc < OC8_EMU_RAM_SIZE);
  if (pc & 0x1) {
    fprintf(stderr, "Warning: fetch instruction at unaligned address %x\n", pc);
  }

  const char *pc_ptr = (const char *)&g_oc8_emu_mem.ram[pc];
  if (oc8_is_decode_ins(&g_oc8_emu_cpu.curr_ins, pc_ptr) != 0) {
    fprintf(stderr, "Failed to decode instruction at address %x\n", pc);
    exit(1);
  }
}

void oc8_emu_cpu_step() {
  // Update timers if necessary
  uint64_t now = time_us();
  g_oc8_emu_cpu.last_cycle_time = now;
  unsigned timer_dec =
      (now - g_oc8_emu_cpu.timer_last_update) / TIMER_ROUND_DURATION;
  if (timer_dec > 0) {
    decrease_timers(timer_dec);
    g_oc8_emu_cpu.timer_last_update +=
        (uint64_t)timer_dec * TIMER_ROUND_DURATION;
  }

  // Update debugger before fetch / exec
  oc8_debug_step();

  // Fecth instruction
  fetch_ins();
  g_oc8_emu_cpu.block_waitq = 0;
  g_oc8_emu_cpu.screen_changed = 0;
  ++g_oc8_emu_cpu.counter_ins;

  // Exec instruction
  oc8_emu_exec_ins();
}

void oc8_emu_cpu_cycle() {

  for (;;) {
    // Check if we waited enough time, and break if we did
    unsigned fq = g_oc8_emu_cpu.cpu_speed;
    uint64_t cycle_elapse_us = time_us() - g_oc8_emu_cpu.last_cycle_time;
    uint64_t cycle_wait_us = 1e6L / fq;
    if (cycle_elapse_us > cycle_wait_us)
      break;

    // Either busy or sleep wait
    uint64_t wait_us = cycle_wait_us - cycle_elapse_us;
    if (wait_us > MIN_SLEEP_TIME_US)
      sleep(0);
  }

  oc8_emu_cpu_step();
}
