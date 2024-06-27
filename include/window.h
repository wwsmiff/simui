#ifndef SIMUI_WINDOW_H_
#define SIMUI_WINDOW_H_

#include "vec2.h"
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdint.h>

#define UUID_LIMIT 512

struct simui_context_t;
struct simui_text_t;

typedef struct simui_window_t {
  vec2f pos;
  vec2f size;
  uint64_t priority;
  uint64_t uuid;
  uint64_t title_uuid;
  uint64_t font_uuid_buffer[UUID_LIMIT];
  size_t font_uuid_buffer_index;
  uint64_t widget_uuid_buffer[UUID_LIMIT];
  size_t widget_uuid_buffer_index;
  char title[256];
  bool moving;
} simui_window_t;

void simui_window_create(struct simui_context_t *context, vec2f pos,
                         vec2f size);
void simui_window_set_title(struct simui_context_t *context, const char *title);

#endif // SIMUI_WINDOW_H_
