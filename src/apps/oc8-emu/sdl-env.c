#include "sdl-env.h"

#include <stdio.h>
#include <stdlib.h>

sdl_env_t g_sdl_env;

static inline void sdl_checkc(int x) {
  if (x < 0) {
    fprintf(stderr, "sdl_env: SDL Error: %s. Aborting !\n", SDL_GetError());
    exit(1);
  }
}

static inline void sdl_checkp(void *ptr) {
  if (!ptr) {
    fprintf(stderr, "sdl_env: SDL Error: %s. Aborting !\n", SDL_GetError());
    exit(1);
  }
}

void sdl_env_init(const char *win_title, int win_width, int win_height) {
  sdl_checkc(SDL_Init(SDL_INIT_VIDEO));
  sdl_checkp((g_sdl_env.sdl_win = SDL_CreateWindow(
                  win_title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                  win_width, win_height, SDL_WINDOW_SHOWN)));
  g_sdl_env.sdl_surface = SDL_GetWindowSurface(g_sdl_env.sdl_win);
  g_sdl_env.img_buf = NULL;

  g_sdl_env.sdl_win_rect.x = 0;
  g_sdl_env.sdl_win_rect.y = 0;
  g_sdl_env.sdl_win_rect.w = win_width;
  g_sdl_env.sdl_win_rect.h = win_height;
}

void sdl_env_exit() {
  SDL_DestroyWindow(g_sdl_env.sdl_win);
  SDL_Quit();
}

int sdl_env_update() {

  // @TODO
  int quit = 0;

  SDL_Event e;
  while (SDL_PollEvent(&e) != 0) {
    if (e.type == SDL_QUIT)
      quit = 1;
  }

  sdl_env_render();

  return quit;
}

void sdl_env_render() {
  if (!g_sdl_env.img_buf)
    return;

  const uint8_t *pixels = g_sdl_env.img_buf;
  size_t width = g_sdl_env.img_width;
  size_t height = g_sdl_env.img_height;

  SDL_Surface *pixs_surface =
      SDL_CreateRGBSurfaceFrom((void *)pixels, width, height, 3 * 8, width * 3,
                               0x0000FF, 0x00FF00, 0xFF0000, 0);
  sdl_checkp(pixs_surface);

  sdl_checkc(SDL_BlitScaled(pixs_surface, NULL, g_sdl_env.sdl_surface,
                            &g_sdl_env.sdl_win_rect));

  SDL_FreeSurface(pixs_surface);
  SDL_UpdateWindowSurface(g_sdl_env.sdl_win);
}

void sdl_env_set_image(const uint8_t *pixs_buf, size_t width, size_t height) {
  g_sdl_env.img_buf = pixs_buf;
  g_sdl_env.img_width = width;
  g_sdl_env.img_height = height;
}
