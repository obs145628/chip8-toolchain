#include "oc8_debug/debug.h"

#include <odb/server_capi/server-app.h>

#include "oc8_debug/vm_api.h"
#include "oc8_emu/mem.h"

oc8_bin_file_t g_oc8_debug_bin_file = {.header = {.version = 0}};

static odb_server_app_t g_odb_app;
static int g_odb_app_active = 0;

static odb_vm_api_vtable_t *api_builder_fn(void *arg,
                                           odb_vm_api_data_t *out_data) {
  (void)arg;
  *out_data = oc8_odb_vm_make_data();
  return &g_oc8_odb_vm_api_table;
}

void oc8_debug_gen_bin_file() {
  if (g_oc8_debug_bin_file_loaded())
    return;

  const void *rom_ptr = g_oc8_emu_mem.ram + OC8_EMU_ROM_ADDR;
  size_t rom_size = OC8_EMU_RAM_SIZE - OC8_EMU_ROM_ADDR;
  oc8_bin_file_init_binary_rom(&g_oc8_debug_bin_file, rom_ptr, rom_size);
  oc8_bin_file_check(&g_oc8_debug_bin_file, /*is_bin=*/1);
}

void oc8_debug_init() {
  if (g_oc8_debug_bin_file_loaded()) {
    oc8_bin_file_free(&g_oc8_debug_bin_file);
    g_oc8_debug_bin_file.header.version = 0;

    if (g_odb_app_active) {
      odb_server_app_free(&g_odb_app);
      g_odb_app_active = 0;
    }
  }
}

void oc8_debug_step() {
  if (!g_odb_app_active) {
    // Initialize debugger
    oc8_debug_gen_bin_file();
    odb_server_app_init(&g_odb_app, api_builder_fn, NULL);
    g_odb_app_active = 1;
  }

  odb_server_app_loop(&g_odb_app);
}
