#ifndef SIMUI_TEXT_H_
#define SIMUI_TEXT_H_

#include "vec2.h"
#include <stdint.h>
#define CHAR_LIMIT 256

struct SDL_Texture;
struct simui_context_t;

typedef struct simui_text_t {
  char data[CHAR_LIMIT];
  struct SDL_Texture *texture;
  vec2f pos;
  vec2i size;
  uint64_t uuid;
} simui_text_t;

uint64_t simui_text_create(struct simui_context_t *context, const char *str,
                           vec2f pos);

uint64_t simui_window_text_create(struct simui_context_t *context,
                                  const char *str, vec2f pos);

#endif // SIMUI_TEXT_H_