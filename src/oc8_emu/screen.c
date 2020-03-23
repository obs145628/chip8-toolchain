#include "oc8_emu/screen.h"

#include <string.h>

uint8_t g_oc8_emu_screen[OC8_EMU_SCREEN_WIDTH * OC8_EMU_SCREEN_HEIGHT / 8];

void oc8_emu_init_screen() {
  memset(g_oc8_emu_screen, 0, sizeof(g_oc8_emu_screen));
}
