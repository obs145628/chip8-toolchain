#define _POSIX_C_SOURCE 200809L

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "oc8_emu/oc8_emu.h"
#include "sdl-env.h"

uint8_t *img_buf;

static void load_image() {
  for (int y = 0; y < OC8_EMU_SCREEN_HEIGHT; ++y) {
    for (int x = 0; x < OC8_EMU_SCREEN_WIDTH; ++x) {

      uint8_t val = oc8_emu_screen_get_pix(x, y) ? 0xFF : 0;
      img_buf[3 * OC8_EMU_SCREEN_WIDTH * y + 3 * x + 0] = val;
      img_buf[3 * OC8_EMU_SCREEN_WIDTH * y + 3 * x + 1] = val;
      img_buf[3 * OC8_EMU_SCREEN_WIDTH * y + 3 * x + 2] = val;
    }
  }

  sdl_env_render();
}

int main(int argc, char **argv) {
  if (argc < 2) {
    fprintf(stderr, "Usage: ./oc8-emu <file>\n");
    return 1;
  }

  oc8_emu_init();
  oc8_emu_load_rom_file(argv[1]);
  sdl_env_init("oc8-emu", 640, 320);
  img_buf = (uint8_t *)malloc(OC8_EMU_SCREEN_HEIGHT * OC8_EMU_SCREEN_WIDTH * 3);
  sdl_env_set_image(img_buf, OC8_EMU_SCREEN_WIDTH, OC8_EMU_SCREEN_HEIGHT);
  load_image();

  for (;;) {
    // Run one cycle
    oc8_emu_cpu_cycle();
    if (g_oc8_emu_cpu.screen_changed)
      load_image();

    // Run GUI loop
    if (sdl_env_update() != 0)
      break;
  }

  sdl_env_exit();
  free(img_buf);
  return 0;
}
