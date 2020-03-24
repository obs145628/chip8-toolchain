#ifndef SDL_ENV_H_
#define SDL_ENV_H_

//===--sdl-env.h - SDL2 Wrapper -----------------------------------*- C -*-===//
//
// oc8-emu
// Author: Steven Lariau
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Really basic wrapper around SDL2
///
//===----------------------------------------------------------------------===//

#include <SDL2/SDL.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SDL_ENV_MAX_KEYS (1024)

/// Main struct, that controls every part of the SDL API
/// Makes sure things are init and released properly
typedef struct {
  SDL_Window *sdl_win;
  SDL_Surface *sdl_surface;
  SDL_Rect sdl_win_rect;

  const uint8_t *img_buf;
  size_t img_width;
  size_t img_height;

  uint8_t keys_states[SDL_ENV_MAX_KEYS];
} sdl_env_t;

extern sdl_env_t g_sdl_env;

/// Init the SDL and open a new window of size `win_width` * `win_height` pixels
void sdl_env_init(const char *win_title, int win_width, int win_height);

/// Close the window and release all the ressources
void sdl_env_exit();

/// Handle GUI events and redraw graphics
/// @returns a value != 0 if the user want to close the window
int sdl_env_update();

/// Only render display, doesn't handle other events
void sdl_env_render();

/// Load image from raw RGB pixels
/// Only does the rendering when update is called.
/// It means `pixs_buf` must remain valid until this function is called again
/// with a new one
void sdl_env_set_image(const uint8_t *pixs_buf, size_t width, size_t height);

/// Returns != 0 if key is pressed
static inline int sdl_env_keystate(size_t key) {
  return (int)g_sdl_env.keys_states[key];
}

#ifdef __cplusplus
}
#endif

#endif //! SDL_ENV_H_
