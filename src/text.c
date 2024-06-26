#include "simui/text.h"
#include "simui/context.h"
#include "simui/font_data.h"
#include "simui/helper.h"
#include "simui/window.h"
#include <SDL2/SDL_ttf.h>
#include <string.h>
#include <time.h>

uint64_t simui_window_text_create(struct simui_context_t *context,
                                  uint64_t window_uuid, const char *str,
                                  vec2f pos) {
  if (context->window_buffer_index < 1) {
    error("Must have at least one window to create text.\n");
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
