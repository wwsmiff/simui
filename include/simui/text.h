#ifndef SIMUI_TEXT_H_
#define SIMUI_TEXT_H_

#include "vec2.h"
#include <stdint.h>
#define CHAR_LIMIT 256

struct SDL_Texture;
struct simui_context_t;

typedef struct simui_text_t {
  const char *data;
  struct SDL_Texture *texture;
  vec2f pos;
  vec2i size;
  uint64_t uuid;
} simui_text_t;

#ifdef __cplusplus
extern "C" {
#endif

uint64_t simui_window_text_create(struct simui_context_t *context,
                                  uint64_t window_uuid, const char *str,
                                  vec2f pos);
#ifdef __cplusplus
}
#endif

#endif // SIMUI_TEXT_H_
