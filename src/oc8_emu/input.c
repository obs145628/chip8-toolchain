#include "oc8_emu/input.h"

#include <string.h>

int g_oc8_emu_keypad[OC8_EMU_NB_KEYS];

void oc8_emu_init_keypad() {
  memset(g_oc8_emu_keypad, 0, sizeof(g_oc8_emu_keypad));
}
