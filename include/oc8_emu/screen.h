#ifndef OC8_EMU_SCREEN_H_
#define OC8_EMU_SCREEN_H_

//===--oc8_emu/screen.h - Screen matrix ---------------------------*- C -*-===//
//
// oc8_emu library
// Author: Steven Lariau
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Define the global screen matrix
///
//===----------------------------------------------------------------------===//

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define OC8_EMU_SCREEN_WIDTH (64)
#define OC8_EMU_SCREEN_HEIGHT (32)

/// The screen matrix
/// Every byte entry is 8 pixels
/// The emulator doesn't display the matrix,
/// It simply set this matrix
extern uint8_t
    g_oc8_emu_screen[OC8_EMU_SCREEN_WIDTH * OC8_EMU_SCREEN_HEIGHT / 8];

static inline int oc8_emu_screen_get_pix(unsigned x, unsigned y) {
  unsigned pos = y * OC8_EMU_SCREEN_WIDTH + x;
  unsigned idx = pos / 8;
  unsigned bit = pos % 8;
  unsigned val = g_oc8_emu_screen[idx];

  return (val >> bit) & 0x1;
}

static inline void oc8_emu_screen_set_pix(unsigned x, unsigned y, int v) {
  unsigned pos = y * OC8_EMU_SCREEN_WIDTH + x;
  unsigned idx = pos / 8;
  unsigned bit = pos % 8;
  unsigned val = g_oc8_emu_screen[idx];
  unsigned mask = 0x1 << bit;

  if (v)
    val |= mask;
  else
    val &= ~mask;

  g_oc8_emu_screen[idx] = val & 0xFF;
}

/// Called be `emu_init`
/// Make the whole scrren black
void oc8_emu_init_screen();

#ifdef __cplusplus
}
#endif

#endif // !OC8_EMU_SCREEN_H_
