#ifndef OC8_EMU_INPUT_H_
#define OC8_EMU_INPUT_H_

//===--oc8_emu/input.h - CHIP-8 keypad ----------------------------*- C -*-===//
//
// oc8_emu library
// Author: Steven Lariau
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Define the Global keypad object
///
//===----------------------------------------------------------------------===//

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define OC8_EMU_NB_KEYS (16)

/// The global keypad object
/// val != 0: key pressed,
/// val = 0: key not pressed
/// The emulator by itself cannot set the keypad
/// Must be done by other program
/// (Eg: SDL app for the emu will set this)
extern int g_oc8_emu_keypad[OC8_EMU_NB_KEYS];

/// Called be `emu_init`
/// Set all keys as not pressed
void oc8_emu_init_keypad();

#ifdef __cplusplus
}
#endif

#endif // !OC8_EMU_INPUT_H_
