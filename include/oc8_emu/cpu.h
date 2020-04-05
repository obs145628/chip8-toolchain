#ifndef OC8_EMU_CPU_H_
#define OC8_EMU_CPU_H_

//===--oc8_emu/cpu.h - CPU struct def -----------------------------*- C -*-===//
//
// oc8_emu library
// Author: Steven Lariau
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Define the CPU struct, and the global CPU used by the emulator
///
//===----------------------------------------------------------------------===//

#include <stdint.h>

#include "oc8_is/ins.h"

#ifdef __cplusplus
extern "C" {
#endif

#define OC8_EMU_NB_REGS (16)
#define OC8_EMU_REG_FLAG (0xF)

/// All data needed by the CHIP-8 CPU
typedef struct {
  // Program Counter
  uint16_t reg_pc;

  // Address Register
  uint16_t reg_i;

  // General Purpose Data Regisers
  uint8_t regs_data[OC8_EMU_NB_REGS];

  // Delay Timer
  uint8_t reg_dt;

  // Sound Timer
  uint8_t reg_st;

  // Stack Pointer
  uint8_t reg_sp;

  // Set to 1 if tried to exec waitk (FX0A), and no key where available
  // Otherwhise set to 0
  int block_waitq;

  // Set to 1 if last instruction changed screen, 0 otherwhise
  int screen_changed;

  // Used by CXNN to generate random numbers
  unsigned long rg_seed;

  // Last time in us Delay and Sound Timers were updated
  uint64_t timer_last_update;

  // Last time in us the last cycle was executed
  uint64_t last_cycle_time;

  // CPU frequency in Hertz (nb cycles per second)
  unsigned cpu_speed;

  // Total number of executed instructions
  // Also count when not executing FX0A (wait for a keypress)
  unsigned counter_ins;

  // Last instruction fetched
  oc8_is_ins_t curr_ins;

} oc8_emu_cpu_t;

// Global variable to access the CPU of the emulator
extern oc8_emu_cpu_t g_oc8_emu_cpu;

/// Called be `emu_init`
/// Setup the content of the CPU struct
void oc8_emu_init_cpu();

/// Must be called only once, to prepare the emu, before loading the ROM.
/// Initialize all required parts of the CHIP-8: CPU, mem, video output, etc
/// Can be called multiple times to reset and restart the CPU (done in TS)
void oc8_emu_init();

/// Run one instruction, ignoring the clock speed
/// Abort if the opcode is invalid
/// If the instruction to be run is FX0A (wait for a keypress), and none is
/// available, the instruction isn't executed and `block_waitq` is set to 1.
/// This is to allow a monothread program to load update the keypad sate before
/// calling `oc8_emu_cpu_step()` again
void oc8_emu_cpu_step();

/// Run one cycle
/// Runs only one instruction, but will sleep a few milliseconds before if
/// needed, to makes sure the CPU runs at the wanted clock speed
void oc8_emu_cpu_cycle();

#ifdef __cplusplus
}
#endif

#endif // !OC8_EMU_CPU_H_
