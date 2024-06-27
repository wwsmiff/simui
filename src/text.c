#include "text.h"
#include "context.h"
#include "font_data.h"
#include "window.h"
#include <SDL2/SDL_ttf.h>
#include <string.h>
#include <time.h>

uint64_t simui_window_text_create(struct simui_context_t *context,
                                  const char *str, vec2f pos) {
  simui_window_t *current_window =
      context->window_buffer[context->window_buffer_index - 1];
  simui_text_t *text = (simui_text_t *)(malloc(sizeof(simui_text_t)));
  SDL_Surface *font_surface = TTF_RenderText_Solid(
      context->font, str, (SDL_Color){.r = 255, .g = 255, .b = 255, .a = 255});
  text->pos = (vec2f){.x = pos.x + current_window->pos.x,
                      .y = pos.y + current_window->pos.y + 30.0f};
  text->texture = SDL_CreateTextureFromSurface(context->renderer, font_surface);
  SDL_FreeSurface(font_surface);
  SDL_QueryTexture(text->texture, NULL, NULL, &text->size.x, &text->size.y);
  text->uuid =
      ((uint64_t)(rand() % UINT64_MAX) << 16) ^ context->text_buffer_index;
  text->data = str;
  context->text_buffer[context->text_buffer_index++] = text;
  current_window->font_uuid_buffer[current_window->font_uuid_buffer_index++] =
      text->uuid;

  return text->uuid;
}
