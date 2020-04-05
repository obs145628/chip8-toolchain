#ifndef OC8_EMU_MEM_H_
#define OC8_EMU_MEM_H_

//===--oc8_emu/mem.h - Memory struct def --------------------------*- C -*-===//
//
// oc8_emu library
// Author: Steven Lariau
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Define the Memory struct, global struct that contains the whole RAM of the
/// CHIP-8
///
//===----------------------------------------------------------------------===//

#include <stdint.h>

#include "../oc8_bin/file.h"

#ifdef __cplusplus
extern "C" {
#endif

#define OC8_EMU_RAM_SIZE (4096)
#define OC8_EMU_STACK_SIZE (512)
#define OC8_EMU_ROM_ADDR (0x200)
#define OC8_EMU_FONT_HEXA_ADDR (0x100)

/// The CHIP-8 RAM
typedef struct {

  // The full 4K Ram array
  uint8_t ram[OC8_EMU_RAM_SIZE];

  // The stack used to store return address for subroutines
  uint16_t stack[OC8_EMU_STACK_SIZE];

} oc8_emu_mem_t;

// Global variable to access the emulator memory
extern oc8_emu_mem_t g_oc8_emu_mem;

/// Called be `emu_init`
/// Setup the content of the memory
void oc8_emu_init_mem();

/// Load a ROM into memory
/// @param rom_size must be <= 3584 to fit in memory
/// Also set the PC at the beginning of the rom
void oc8_emu_load_rom(const void *rom_bytes, unsigned rom_size);

/// Load ROM in `bf`
/// Also assign `bf` as the debug bin of the emu
/// `bf` is moved into emu and cannot be used (or free'd) after this
/// Also set the PC at the beginning of the rom
/// `oc8_bin_file_check` isn't called
void oc8_emu_load_bin(oc8_bin_file_t *bf);

/// Wrapper around `oc8_emu_load_rom` / `oc8_emu_load_bin`
/// Open / close the file, and do all the checks
/// Can load raw ROM file. or binary `.ocbin` file
/// Detect if the file is `.ocbin` (using magic number, not ext)
void oc8_emu_load_rom_file(const char *path);

#ifdef __cplusplus
}
#endif

#endif // !OC8_EMU_MEM_H_
