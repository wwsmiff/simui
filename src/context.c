#include "context.h"
#include "font_data.h"
#include "helper.h"
#include "text.h"
#include "window.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#define HIGHEST_PRIORITY (WINDOW_LIMIT + 1)

typedef struct {
  simui_text_t *text;
  vec2f offset;
} font_offset_t;

// Hepler functions
void simui_window_sort(simui_context_t *context) {
  // TODO: implement better sorting algorithm
  for (size_t i = 0; i < context->window_buffer_index - 1; ++i) {
    for (size_t j = 0; j < context->window_buffer_index - i - 1; ++j) {
      simui_window_t *a = context->window_buffer[j];
      simui_window_t *b = context->window_buffer[j + 1];
      if (a->priority > b->priority) {
        simui_window_t tmp = *a;
        *a = *b;
        *b = tmp;
      }
    }
  }
}

void regenerate_fonts(simui_context_t *context) {
  for (size_t i = 0; i < context->text_buffer_index; ++i) {
    SDL_DestroyTexture(context->text_buffer[i]->texture);
  }
  for (size_t i = 0; i < context->text_buffer_index; ++i) {
    // TODO: implement caching and checking.
    simui_text_t *text = context->text_buffer[i];
    SDL_Surface *font_surface = TTF_RenderText_Solid(
        context->font, text->data,
        (SDL_Color){.r = 255, .g = 255, .b = 255, .a = 255});
    text->pos = (vec2f){.x = text->pos.x, .y = text->pos.y};
    text->texture =
        SDL_CreateTextureFromSurface(context->renderer, font_surface);
    SDL_FreeSurface(font_surface);
    SDL_QueryTexture(text->texture, NULL, NULL, &text->size.x, &text->size.y);
  }
}

// simui context implementation
simui_context_t simui_context_create(SDL_Window *target_window,
                                     SDL_Renderer *renderer) {
  simui_context_t new_context;
  if (target_window == NULL || renderer == NULL) {
    fprintf(stderr, "Need to specify window and renderer.\n");
    exit(1);
  } else {
    new_context.target_window = target_window;
    new_context.renderer = renderer;
  }

  new_context.window_buffer_index = 0;
  new_context.text_buffer_index = 0;
  new_context.dynamic_text_buffer_index = 0;
  new_context.focused_uuid = 0;
  new_context.sort_window_buffer = false;
  SDL_RWops *font_data_rwops = SDL_RWFromMem(
      proggy_clean_font_data, proggy_clean_font_data_len * sizeof(uint32_t));
  new_context.font = TTF_OpenFontRW(font_data_rwops, 0, 16);
  SDL_RWclose(font_data_rwops);
  return new_context;
}

void simui_context_handle_event(simui_context_t *context, SDL_Event *event) {
  static vec2i offset = {.x = 0, .y = 0};
  static font_offset_t font_offsets[256] = {0};
  static size_t font_offset_index = 0;
  for (size_t i = 0; i < context->window_buffer_index; ++i) {
    simui_window_t *current_window = context->window_buffer[i];
    if (event->motion.x > current_window->pos.x &&
        event->motion.x < (current_window->pos.x + current_window->size.x) &&
        event->motion.y > current_window->pos.y &&
        event->motion.y < (current_window->pos.y + current_window->size.y)) {
      if (event->type == SDL_MOUSEBUTTONDOWN &&
          event->button.button == SDL_BUTTON_LEFT) {
        font_offset_index = 0;
        memset(font_offsets, 0, sizeof(font_offsets));
        current_window->priority = HIGHEST_PRIORITY;
        context->focused_uuid = current_window->uuid;
        context->sort_window_buffer = true;
        offset.x = event->motion.x - current_window->pos.x;
        offset.y = event->motion.y - current_window->pos.y;
        if (event->motion.y < (current_window->pos.y + 30)) {
          current_window->moving = true;
          for (size_t i = 0; i < current_window->font_uuid_buffer_index; ++i) {
            simui_text_t *text =
                get_text(context, current_window->font_uuid_buffer[i]);
            font_offsets[font_offset_index++] = (font_offset_t){
                .text = text,
                .offset = (vec2f){.x = text->pos.x - current_window->pos.x,
                                  .y = text->pos.y - current_window->pos.y}};
          }
        }
      }
    }
  }

  for (size_t i = 0; i < context->window_buffer_index; ++i) {
    if (context->window_buffer[i]->uuid == context->focused_uuid) {
      continue;
    } else {
      context->window_buffer[i]->priority = i;
      context->window_buffer[i]->moving = false;
    }
  }

  if (context->sort_window_buffer) {
    simui_window_sort(context);
    context->sort_window_buffer = false;
  }

  if (event->type == SDL_MOUSEBUTTONUP &&
      event->button.button == SDL_BUTTON_LEFT) {
    context->window_buffer[context->window_buffer_index - 1]->moving = false;
  }

  if (context->window_buffer[context->window_buffer_index - 1]->moving) {
    context->window_buffer[context->window_buffer_index - 1]->pos.x =
        event->motion.x - offset.x;
    context->window_buffer[context->window_buffer_index - 1]->pos.y =
        event->motion.y - offset.y;
    for (size_t i = 0; i < font_offset_index; ++i) {
      font_offsets[i].text->pos.x =
          context->window_buffer[context->window_buffer_index - 1]->pos.x +
          font_offsets[i].offset.x;
      font_offsets[i].text->pos.y =
          context->window_buffer[context->window_buffer_index - 1]->pos.y +
          font_offsets[i].offset.y;
    }
  }
}

void simui_context_render(simui_context_t *context) {
  regenerate_fonts(context);
  for (size_t i = 0; i < context->window_buffer_index; ++i) {
    SDL_FRect background = {.x = context->window_buffer[i]->pos.x,
                            .y = context->window_buffer[i]->pos.y,
                            .w = context->window_buffer[i]->size.x,
                            .h = context->window_buffer[i]->size.y};

    SDL_FRect titlebar = {.x = context->window_buffer[i]->pos.x,
                          .y = context->window_buffer[i]->pos.y,
                          .w = context->window_buffer[i]->size.x,
                          .h = 30.0f};

    SDL_SetRenderDrawColor(context->renderer, 30, 30, 30, 255);
    SDL_RenderFillRectF(context->renderer, &background);

    SDL_SetRenderDrawColor(context->renderer, 20, 20, 20, 255);
    SDL_RenderFillRectF(context->renderer, &titlebar);

    SDL_SetRenderDrawColor(context->renderer, 150, 150, 150, 255);
    SDL_RenderDrawRectF(context->renderer, &background);

    for (size_t j = 0; j < context->window_buffer[i]->font_uuid_buffer_index;
         ++j) {
      uint64_t text_uuid = context->window_buffer[i]->font_uuid_buffer[j];
      simui_text_t *text = get_text(context, text_uuid);
      SDL_Rect font_rect = {.x = text->pos.x,
                            .y = text->pos.y,
                            .w = text->size.x,
                            .h = text->size.y};
      SDL_RenderCopy(context->renderer, text->texture, NULL, &font_rect);
    }
  }
}

void simui_context_destroy(simui_context_t *context) {
  for (size_t i = 0; i < context->window_buffer_index; ++i) {
    free(context->window_buffer[i]);
  }
  for (size_t i = 0; i < context->text_buffer_index; ++i) {
    SDL_DestroyTexture(context->text_buffer[i]->texture);
    free(context->text_buffer[i]);
  }

  TTF_CloseFont(context->font);
}