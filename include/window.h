#ifndef SIMUI_WINDOW_H_
#define SIMUI_WINDOW_H_

#include "vec2.h"
#include <SDL2/SDL.h>
#include <stdbool.h>

struct simui_context_t;

typedef struct simui_window_t {
  vec2f pos;
  vec2f size;
  uint32_t priority;
  uint64_t uuid;
  SDL_Color color;
  bool moving;
} simui_window_t;

void simui_window_new(struct simui_context_t *context, vec2f pos, vec2f size);

#endif // SIMUI_WINDOW_H_