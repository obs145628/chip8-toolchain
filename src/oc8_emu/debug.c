#include "oc8_emu/debug.h"

#include "oc8_emu/mem.h"

oc8_bin_file_t g_oc8_emu_bin_file = {.header = {.version = 0}};

void oc8_emu_init_debug() {
  if (g_oc8_emu_bin_file_loaded()) {
    oc8_bin_file_free(&g_oc8_emu_bin_file);
    g_oc8_emu_bin_file.header.version = 0;
  }
}

void oc8_emu_gen_debug_bin_file() {
  if (g_oc8_emu_bin_file_loaded())
    return;

  const void *rom_ptr = g_oc8_emu_mem.ram + OC8_EMU_ROM_ADDR;
  size_t rom_size = OC8_EMU_RAM_SIZE - OC8_EMU_ROM_ADDR;
  oc8_bin_file_init_binary_rom(&g_oc8_emu_bin_file, rom_ptr, rom_size);
  oc8_bin_file_check(&g_oc8_emu_bin_file, /*is_bin=*/1);
}
