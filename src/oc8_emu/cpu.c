#include "oc8_emu/cpu.h"
#include "oc8_emu/input.h"
#include "oc8_emu/mem.h"
#include "oc8_emu/screen.h"
#include "oc8_is/ins.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define OPCODE_SIZE (2)

oc8_emu_cpu_t g_oc8_emu_cpu;

// Current instruction to be executed
static oc8_is_ins_t g_curr_ins;

void oc8_emu_init_cpu() {
  // Set all registers to 0
  // Even PC, will be set to right value when ROM is loaded
  memset(&g_oc8_emu_cpu, 0, sizeof(g_oc8_emu_cpu));
}

void oc8_emu_init() {
  oc8_emu_init_cpu();
  oc8_emu_init_keypad();
  oc8_emu_init_mem();
  oc8_emu_init_screen();
}

static void fetch_ins() {
  unsigned pc = g_oc8_emu_cpu.reg_pc;
  assert(pc < OC8_EMU_RAM_SIZE);
  if (pc & 0x1) {
    fprintf(stderr, "Warning: fetch instruction at unaligned address %x\n", pc);
  }

  const char *pc_ptr = (const char *)&g_oc8_emu_mem.ram[pc];
  if (oc8_is_decode_ins(&g_curr_ins, pc_ptr) != 0) {
    fprintf(stderr, "Failed to decode instruction at address %x\n", pc);
    exit(1);
  }
}

static void exec_ins_0NNN() {
  fprintf(stderr, "Instruction 0NNN not implemented. Aborting !\n");
}

static void exec_ins_00E0() {
  memset(g_oc8_emu_screen, 0, sizeof(g_oc8_emu_screen));
  g_oc8_emu_cpu.reg_pc += OPCODE_SIZE;
}

void oc8_emu_cpu_step() {
  // @TODO
  fetch_ins();
  g_oc8_emu_cpu.block_waitq = 0;

  switch (g_curr_ins.type) {
  case OC8_IS_TYPE_SYS:
    exec_ins_0NNN();
    break;
  case OC8_IS_TYPE_CLS:
    exec_ins_00E0();
    break;

  default:
    // unreachable
    assert(0);
  }
}

void uc8_emu_cpu_loop() {
  for (;;) {

    // @TODO

    oc8_emu_cpu_step();
    if (g_oc8_emu_cpu.block_waitq)
      break;
  }
}
