#include "window.h"
#include "context.h"
#include "helper.h"
#include "text.h"
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void simui_window_create(struct simui_context_t *context, vec2f pos,
                         vec2f size) {
  simui_window_t *window = (simui_window_t *)(malloc(sizeof(simui_window_t)));
  window->pos = pos;
  window->size = size;
  window->priority = context->window_buffer_index;
  window->moving = false;
  window->uuid = ((uint64_t)(rand() % UINT64_MAX) << 16) ^ window->priority;
  window->font_uuid_buffer_index = 0;
  char window_title[256] = {0};
  sprintf(window_title, "window uuid: %llx", window->uuid);

  context->window_buffer[context->window_buffer_index++] = window;
  context->focused_uuid = window->uuid;
  context->sort_window_buffer = true;

  window->title_uuid = simui_window_text_create(context, window_title,
                                                (vec2f){.x = 0.0f, .y = 0.0f});

  simui_text_t *title_text =
      context->text_buffer[context->text_buffer_index - 1];
  title_text->pos.x = ((window->pos.x) / 2.0f) +
                      ((window->pos.x + window->size.x) / 2.0f) -
                      (title_text->size.x / 2.0f);
  title_text->pos.y = (window->pos.y + 15.0f) - (title_text->size.y / 2.0f);
  strncpy(title_text->data, window_title, CHAR_LIMIT);
}

void simui_window_set_title(struct simui_context_t *context,
                            const char *title) {
  simui_window_t *current_window =
      context->window_buffer[context->window_buffer_index - 1];
  simui_text_t *title_text = get_text(context, current_window->title_uuid);
  strncpy(title_text->data, title, CHAR_LIMIT);
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