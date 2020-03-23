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

static void exec_ins_00EE() {
  unsigned sp = g_oc8_emu_cpu.reg_sp;
  assert(sp);
  unsigned new_pc = g_oc8_emu_mem.stack[--sp] & 0xFFF;
  g_oc8_emu_cpu.reg_sp = sp;
  g_oc8_emu_cpu.reg_pc = new_pc;
}

static void exec_ins_1NNN() {
  unsigned new_pc = g_curr_ins.operands[0] & 0xFFF;
  g_oc8_emu_cpu.reg_pc = new_pc;
}

static void exec_ins_2NNN() {
  unsigned next_ins = g_oc8_emu_cpu.reg_pc + OPCODE_SIZE;
  unsigned new_pc = g_curr_ins.operands[0] & 0xFFF;
  unsigned sp = g_oc8_emu_cpu.reg_sp;
  g_oc8_emu_mem.stack[sp++] = next_ins;

  g_oc8_emu_cpu.reg_sp = sp;
  g_oc8_emu_cpu.reg_pc = new_pc;
}

static void exec_ins_3XNN() {
  unsigned vx = g_curr_ins.operands[0];
  unsigned imm = g_curr_ins.operands[1];

  if (g_oc8_emu_cpu.regs_data[vx] == imm)
    g_oc8_emu_cpu.reg_pc += 2 * OPCODE_SIZE;
  else
    g_oc8_emu_cpu.reg_pc += OPCODE_SIZE;
}

static void exec_ins_4XNN() {
  unsigned vx = g_curr_ins.operands[0];
  unsigned imm = g_curr_ins.operands[1];

  if (g_oc8_emu_cpu.regs_data[vx] != imm)
    g_oc8_emu_cpu.reg_pc += 2 * OPCODE_SIZE;
  else
    g_oc8_emu_cpu.reg_pc += OPCODE_SIZE;
}

static void exec_ins_5XY0() {
  unsigned vx = g_curr_ins.operands[0];
  unsigned vy = g_curr_ins.operands[1];

  if (g_oc8_emu_cpu.regs_data[vx] == g_oc8_emu_cpu.regs_data[vy])
    g_oc8_emu_cpu.reg_pc += 2 * OPCODE_SIZE;
  else
    g_oc8_emu_cpu.reg_pc += OPCODE_SIZE;
}

static void exec_ins_6XNN() {
  unsigned vx = g_curr_ins.operands[0];
  unsigned imm = g_curr_ins.operands[1];

  g_oc8_emu_cpu.regs_data[vx] = imm & 0xFF;
  g_oc8_emu_cpu.reg_pc += OPCODE_SIZE;
}

static void exec_ins_7XNN() {
  unsigned vx = g_curr_ins.operands[0];
  uint8_t imm = g_curr_ins.operands[1];

  g_oc8_emu_cpu.regs_data[vx] += imm;
  g_oc8_emu_cpu.reg_pc += OPCODE_SIZE;
}

static void exec_ins_8XY0() {
  unsigned vx = g_curr_ins.operands[0];
  unsigned vy = g_curr_ins.operands[1];

  g_oc8_emu_cpu.regs_data[vx] = g_oc8_emu_cpu.regs_data[vy];
  g_oc8_emu_cpu.reg_pc += OPCODE_SIZE;
}

static void exec_ins_8XY1() {
  unsigned vx = g_curr_ins.operands[0];
  unsigned vy = g_curr_ins.operands[1];

  g_oc8_emu_cpu.regs_data[vx] |= g_oc8_emu_cpu.regs_data[vy];
  g_oc8_emu_cpu.reg_pc += OPCODE_SIZE;
}

static void exec_ins_8XY2() {
  unsigned vx = g_curr_ins.operands[0];
  unsigned vy = g_curr_ins.operands[1];

  g_oc8_emu_cpu.regs_data[vx] &= g_oc8_emu_cpu.regs_data[vy];
  g_oc8_emu_cpu.reg_pc += OPCODE_SIZE;
}

static void exec_ins_8XY3() {
  unsigned vx = g_curr_ins.operands[0];
  unsigned vy = g_curr_ins.operands[1];

  g_oc8_emu_cpu.regs_data[vx] ^= g_oc8_emu_cpu.regs_data[vy];
  g_oc8_emu_cpu.reg_pc += OPCODE_SIZE;
}

static void exec_ins_8XY4() {
  unsigned vx = g_curr_ins.operands[0];
  unsigned vy = g_curr_ins.operands[1];

  g_oc8_emu_cpu.regs_data[OC8_EMU_REG_FLAG] =
      (unsigned)g_oc8_emu_cpu.regs_data[vx] +
          (unsigned)g_oc8_emu_cpu.regs_data[vy] >
      255;
  g_oc8_emu_cpu.regs_data[vx] += g_oc8_emu_cpu.regs_data[vy];
  g_oc8_emu_cpu.reg_pc += OPCODE_SIZE;
}

static void exec_ins_8XY5() {
  unsigned vx = g_curr_ins.operands[0];
  unsigned vy = g_curr_ins.operands[1];

  g_oc8_emu_cpu.regs_data[OC8_EMU_REG_FLAG] =
      g_oc8_emu_cpu.regs_data[vx] > g_oc8_emu_cpu.regs_data[vy];
  g_oc8_emu_cpu.regs_data[vx] -= g_oc8_emu_cpu.regs_data[vy];
  g_oc8_emu_cpu.reg_pc += OPCODE_SIZE;
}

static void exec_ins_8XY6() {
  unsigned vx = g_curr_ins.operands[0];
  unsigned vy = g_curr_ins.operands[1];

  g_oc8_emu_cpu.regs_data[OC8_EMU_REG_FLAG] = g_oc8_emu_cpu.regs_data[vy] & 0x1;
  g_oc8_emu_cpu.regs_data[vx] = g_oc8_emu_cpu.regs_data[vy] >> 1;
  g_oc8_emu_cpu.reg_pc += OPCODE_SIZE;
}

static void exec_ins_8XY7() {
  unsigned vx = g_curr_ins.operands[0];
  unsigned vy = g_curr_ins.operands[1];

  g_oc8_emu_cpu.regs_data[OC8_EMU_REG_FLAG] =
      g_oc8_emu_cpu.regs_data[vy] > g_oc8_emu_cpu.regs_data[vx];
  g_oc8_emu_cpu.regs_data[vx] =
      g_oc8_emu_cpu.regs_data[vy] - g_oc8_emu_cpu.regs_data[vx];
  g_oc8_emu_cpu.reg_pc += OPCODE_SIZE;
}

static void exec_ins_8XYE() {
  unsigned vx = g_curr_ins.operands[0];
  unsigned vy = g_curr_ins.operands[1];

  g_oc8_emu_cpu.regs_data[OC8_EMU_REG_FLAG] =
      g_oc8_emu_cpu.regs_data[vy] & 0x80 ? 1 : 0;
  g_oc8_emu_cpu.regs_data[vx] = g_oc8_emu_cpu.regs_data[vy] << 1;
  g_oc8_emu_cpu.reg_pc += OPCODE_SIZE;
}

static void exec_ins_9XY0() {
  unsigned vx = g_curr_ins.operands[0];
  unsigned vy = g_curr_ins.operands[1];

  if (g_oc8_emu_cpu.regs_data[vx] != g_oc8_emu_cpu.regs_data[vy])
    g_oc8_emu_cpu.reg_pc += 2 * OPCODE_SIZE;
  else
    g_oc8_emu_cpu.reg_pc += OPCODE_SIZE;
}

static void exec_ins_ANNN() {
  unsigned addr = g_curr_ins.operands[0] & 0xFFF;
  g_oc8_emu_cpu.reg_i = addr;
  g_oc8_emu_cpu.reg_pc += OPCODE_SIZE;
}

static void exec_ins_BNNN() {
  unsigned addr = g_curr_ins.operands[0] & 0xFFF;
  unsigned new_pc = g_oc8_emu_cpu.regs_data[0] + addr;
  if (new_pc >= 4096)
    fprintf(stderr, "Warning: pc overflows when executing BNNN: %u\n", new_pc);

  g_oc8_emu_cpu.reg_pc = new_pc & 0xFFF;
}

static void exec_ins_CXNN() {
  unsigned vx = g_curr_ins.operands[0];
  unsigned imm = g_curr_ins.operands[1];

  g_oc8_emu_cpu.rg_seed = g_oc8_emu_cpu.rg_seed * 1103515245 + 12345;
  g_oc8_emu_cpu.regs_data[vx] = (g_oc8_emu_cpu.rg_seed / 65536) % imm;
  g_oc8_emu_cpu.reg_pc += OPCODE_SIZE;
}

static void exec_ins_DXYN() {
  unsigned vx = g_curr_ins.operands[0];
  unsigned vy = g_curr_ins.operands[1];
  unsigned x0 = g_oc8_emu_cpu.regs_data[vx] % OC8_EMU_SCREEN_WIDTH;
  unsigned y0 = g_oc8_emu_cpu.regs_data[vy] % OC8_EMU_SCREEN_HEIGHT;
  unsigned w = 8;
  unsigned h = g_curr_ins.operands[2];
  unsigned addr = g_oc8_emu_cpu.reg_i;
  unsigned vf = 0;

  for (unsigned y = 0; y < h && y + y0 < OC8_EMU_SCREEN_HEIGHT; ++y) {
    unsigned hline = g_oc8_emu_mem.ram[addr++];
    for (unsigned x = 0; x < w && x + x0 < OC8_EMU_SCREEN_WIDTH; ++x) {
      if ((hline & (0x1 << (7 - x))) == 0)
        continue;

      int old_val = oc8_emu_screen_get_pix(x + x0, y + y0);
      oc8_emu_screen_set_pix(x + x0, y + y0, !old_val);
      if (old_val)
        vf = 1;
    }
  }

  g_oc8_emu_cpu.regs_data[OC8_EMU_REG_FLAG] = vf;
  g_oc8_emu_cpu.reg_pc += OPCODE_SIZE;
}

static void exec_ins_EX9E() {
  unsigned vx = g_curr_ins.operands[0];
  unsigned key = g_oc8_emu_cpu.regs_data[vx];

  if (g_oc8_emu_keypad[key])
    g_oc8_emu_cpu.reg_pc += 2 * OPCODE_SIZE;
  else
    g_oc8_emu_cpu.reg_pc += OPCODE_SIZE;
}

static void exec_ins_EXA1() {
  unsigned vx = g_curr_ins.operands[0];
  unsigned key = g_oc8_emu_cpu.regs_data[vx];

  if (!g_oc8_emu_keypad[key])
    g_oc8_emu_cpu.reg_pc += 2 * OPCODE_SIZE;
  else
    g_oc8_emu_cpu.reg_pc += OPCODE_SIZE;
}

static void exec_ins_FX07() {
  unsigned vx = g_curr_ins.operands[0];
  g_oc8_emu_cpu.regs_data[vx] = g_oc8_emu_cpu.reg_dt;
  g_oc8_emu_cpu.reg_pc += OPCODE_SIZE;
}

static int get_keypress() {
  for (int i = 0; i < OC8_EMU_NB_KEYS; ++i)
    if (g_oc8_emu_keypad[i])
      return i;
  return -1;
}

static void exec_ins_FX0A() {
  int key = get_keypress();
  if (key == -1) {
    g_oc8_emu_cpu.block_waitq = 1;
    return;
  }

  unsigned vx = g_curr_ins.operands[0];
  g_oc8_emu_cpu.regs_data[vx] = (uint8_t)key;
  g_oc8_emu_cpu.reg_pc += OPCODE_SIZE;
}

static void exec_ins_FX15() {
  unsigned vx = g_curr_ins.operands[0];
  g_oc8_emu_cpu.reg_dt = g_oc8_emu_cpu.regs_data[vx];
  g_oc8_emu_cpu.reg_pc += OPCODE_SIZE;
}

static void exec_ins_FX18() {
  unsigned vx = g_curr_ins.operands[0];
  g_oc8_emu_cpu.reg_st = g_oc8_emu_cpu.regs_data[vx];
  g_oc8_emu_cpu.reg_pc += OPCODE_SIZE;
}

static void exec_ins_FX1E() {
  unsigned vx = g_curr_ins.operands[0];
  g_oc8_emu_cpu.reg_i =
      (g_oc8_emu_cpu.reg_i + g_oc8_emu_cpu.regs_data[vx]) & 0xFFF;
  g_oc8_emu_cpu.reg_pc += OPCODE_SIZE;
}

static void exec_ins_FX29() {
  unsigned vx = g_curr_ins.operands[0];
  unsigned digit = g_oc8_emu_cpu.regs_data[vx] & 0xF;
  g_oc8_emu_cpu.reg_i = OC8_EMU_FONT_HEXA_ADDR + 5 * digit;
  g_oc8_emu_cpu.reg_pc += OPCODE_SIZE;
}

static void exec_ins_FX33() {
  unsigned vx = g_curr_ins.operands[0];
  unsigned i = g_oc8_emu_cpu.reg_i;
  unsigned val = g_oc8_emu_cpu.regs_data[vx];

  g_oc8_emu_mem.ram[i + 0] = val / 100;
  g_oc8_emu_mem.ram[i + 1] = (val % 100) / 10;
  g_oc8_emu_mem.ram[i + 2] = val % 10;
  g_oc8_emu_cpu.reg_pc += OPCODE_SIZE;
}

static void exec_ins_FX55() {
  unsigned vx = g_curr_ins.operands[0];
  unsigned i = g_oc8_emu_cpu.reg_i;

  for (unsigned vi = 0; vi <= vx; ++vi)
    g_oc8_emu_mem.ram[i + vi] = g_oc8_emu_cpu.regs_data[vi];

  g_oc8_emu_cpu.reg_i += vx + 1;
  g_oc8_emu_cpu.reg_pc += OPCODE_SIZE;
}

static void exec_ins_FX65() {
  unsigned vx = g_curr_ins.operands[0];
  unsigned i = g_oc8_emu_cpu.reg_i;

  for (unsigned vi = 0; vi <= vx; ++vi)
    g_oc8_emu_cpu.regs_data[vi] = g_oc8_emu_mem.ram[i + vi];

  g_oc8_emu_cpu.reg_i += vx + 1;
  g_oc8_emu_cpu.reg_pc += OPCODE_SIZE;
}

void oc8_emu_cpu_step() {
  fetch_ins();
  g_oc8_emu_cpu.block_waitq = 0;

  switch (g_curr_ins.type) {
  case OC8_IS_TYPE_SYS:
    exec_ins_0NNN();
    break;
  case OC8_IS_TYPE_CLS:
    exec_ins_00E0();
    break;
  case OC8_IS_TYPE_RET:
    exec_ins_00EE();
    break;
  case OC8_IS_TYPE_JMP:
    exec_ins_1NNN();
    break;
  case OC8_IS_TYPE_CALL:
    exec_ins_2NNN();
    break;
  case OC8_IS_TYPE_SKIP_E_IMM:
    exec_ins_3XNN();
    break;
  case OC8_IS_TYPE_SKIP_NE_IMM:
    exec_ins_4XNN();
    break;
  case OC8_IS_TYPE_SKIP_E_REG:
    exec_ins_5XY0();
    break;
  case OC8_IS_TYPE_LD_IMM:
    exec_ins_6XNN();
    break;
  case OC8_IS_TYPE_ADD_IMM:
    exec_ins_7XNN();
    break;
  case OC8_IS_TYPE_LD_REG:
    exec_ins_8XY0();
    break;
  case OC8_IS_TYPE_OR:
    exec_ins_8XY1();
    break;
  case OC8_IS_TYPE_AND:
    exec_ins_8XY2();
    break;
  case OC8_IS_TYPE_XOR:
    exec_ins_8XY3();
    break;
  case OC8_IS_TYPE_ADD_REG:
    exec_ins_8XY4();
    break;
  case OC8_IS_TYPE_SUB:
    exec_ins_8XY5();
    break;
  case OC8_IS_TYPE_SHR:
    exec_ins_8XY6();
    break;
  case OC8_IS_TYPE_SUBN:
    exec_ins_8XY7();
    break;
  case OC8_IS_TYPE_SHL:
    exec_ins_8XYE();
    break;
  case OC8_IS_TYPE_SKIP_NE_REG:
    exec_ins_9XY0();
    break;
  case OC8_IS_TYPE_ST_I:
    exec_ins_ANNN();
    break;
  case OC8_IS_TYPE_JMP_V0:
    exec_ins_BNNN();
    break;
  case OC8_IS_TYPE_RAND:
    exec_ins_CXNN();
    break;
  case OC8_IS_TYPE_DRAW:
    exec_ins_DXYN();
    break;
  case OC8_IS_TYPE_SKIP_KP:
    exec_ins_EX9E();
    break;
  case OC8_IS_TYPE_SKIP_KNP:
    exec_ins_EXA1();
    break;
  case OC8_IS_TYPE_LD_DT:
    exec_ins_FX07();
    break;
  case OC8_IS_TYPE_LD_K:
    exec_ins_FX0A();
    break;
  case OC8_IS_TYPE_ST_DT:
    exec_ins_FX15();
    break;
  case OC8_IS_TYPE_ST_ST:
    exec_ins_FX18();
    break;
  case OC8_IS_TYPE_ADD_I:
    exec_ins_FX1E();
    break;
  case OC8_IS_TYPE_LD_F:
    exec_ins_FX29();
    break;
  case OC8_IS_TYPE_LD_B:
    exec_ins_FX33();
    break;
  case OC8_IS_TYPE_ST_REGS:
    exec_ins_FX55();
    break;
  case OC8_IS_TYPE_LD_REGS:
    exec_ins_FX65();
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
