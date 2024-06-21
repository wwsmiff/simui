#include "window.h"
#include "context.h"
#include <stdio.h>
#include <stdlib.h>

void simui_window_new(struct simui_context_t *context, vec2f pos, vec2f size) {
  simui_window_t *window = (simui_window_t *)(malloc(sizeof(simui_window_t)));
  window->pos = pos;
  window->size = size;
  window->priority = context->window_buffer_index;
  window->moving = false;
  window->color = (SDL_Color){30 + (window->priority * 10), 30, 30, 255};
  window->uuid = ((uint64_t)(rand() % UINT64_MAX) << 16) ^ window->priority;
  context->window_buffer[context->window_buffer_index++] = window;
  context->sort_window_buffer = true;
  context->focused_uuid = window->uuid;
}