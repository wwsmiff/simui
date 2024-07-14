#include "simui/window.h"
#include "simui/context.h"
#include "simui/helper.h"
#include "simui/text.h"
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

uint64_t simui_window_create(struct simui_context_t *context, vec2f pos,
                             vec2f size) {
  if (context == NULL) {
    error("Context cannot be null.\n");
  }

  simui_window_t *window = (simui_window_t *)(malloc(sizeof(simui_window_t)));
  window->pos = pos;
  window->size = size;
  window->priority = context->window_buffer_index;
  window->moving = false;
  window->scroll = false;
  window->scrollbar_thumb.x = (window->pos.x + window->size.x) - 21.0f;
  window->scrollbar_thumb.y = window->pos.y + 30.0f;

  window->uuid = ((uint64_t)(rand() % UINT64_MAX) << 16) ^ window->priority;
  window->font_uuid_buffer_index = 0;
  window->widget_uuid_buffer_index = 0;
  sprintf(window->title, "window uuid: %lx", window->uuid);

  context->window_buffer[context->window_buffer_index++] = window;
  context->focused_uuid = window->uuid;
  context->sort_window_buffer = true;

  window->title_uuid = simui_window_text_create(context, 0, window->title,
                                                (vec2f){.x = 0.0f, .y = 0.0f});
  simui_text_t *title_text =
      context->text_buffer[context->text_buffer_index - 1];
  title_text->pos.x = ((window->pos.x) / 2.0f) +
                      ((window->pos.x + window->size.x) / 2.0f) -
                      (title_text->size.x / 2.0f);
  title_text->pos.y = (window->pos.y + 15.0f) - (title_text->size.y / 2.0f);

  return window->uuid;
}

void simui_window_set_title(struct simui_context_t *context,
                            uint64_t window_uuid, const char *title) {
  if (context->window_buffer_index < 1) {
    error("Must have at least one window to set title.\n");
  }

  simui_window_t *current_window = NULL;
  if (window_uuid == 0) {
    current_window = context->window_buffer[context->window_buffer_index - 1];
  } else {
    current_window = get_window(context, window_uuid);
    if (current_window == NULL) {
      error("window [uuid: %lx] does not exist.\n", window_uuid);
    }
  }

  strncpy(current_window->title, title, 256);
  simui_text_t *title_text = get_text(context, current_window->title_uuid);
  SDL_DestroyTexture(title_text->texture);
  SDL_Surface *font_surface =
      TTF_RenderText_Solid(context->font, title_text->data,
                           (SDL_Color){.r = 255, .g = 255, .b = 255, .a = 255});
  title_text->texture =
      SDL_CreateTextureFromSurface(context->renderer, font_surface);
  SDL_FreeSurface(font_surface);
  SDL_QueryTexture(title_text->texture, NULL, NULL, &title_text->size.x,
                   &title_text->size.y);
  title_text->pos.x =
      ((current_window->pos.x) / 2.0f) +
      ((current_window->pos.x + current_window->size.x) / 2.0f) -
      (title_text->size.x / 2.0f);
  title_text->pos.y =
      (current_window->pos.y + 15.0f) - (title_text->size.y / 2.0f);
}
