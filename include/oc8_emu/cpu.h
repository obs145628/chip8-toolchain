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

#ifdef __cplusplus
extern "C" {
#endif

#define OC8_EMU_NB_REGS (16)

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

} oc8_emu_cpu_t;

// Global variable to access the CPU of the emulator
extern oc8_emu_cpu_t g_oc8_emu_cpu;

/// Called be `emu_init`
/// Setup the content of the CPU struct
void oc8_emu_init_cpu();

/// Must be called only once, to prepare the emu, before loading the ROM.
/// Initialize all required parts of the CHIP-8: CPU, mem, video output, etc
void oc8_emu_init();

/// Only run one instruction
/// Abort if the opcode is invalid
void oc8_emu_cpu_step();

/// Run Many instructions until an event pause the process
/// One of these events could be:
/// - Many ticks occured since begin (need to refresh GUI and other for
/// monothread programs)
/// - waitk instruction runned, and no key available yet
/// Abort if one instruction failed (invalid opcode)
void uc8_emu_cpu_loop();

#ifdef __cplusplus
}
#endif

#endif // !OC8_EMU_CPU_H_
