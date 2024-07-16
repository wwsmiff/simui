#include "simui/context.h"
#include "simui/font_data.h"
#include "simui/helper.h"
#include "simui/text.h"
#include "simui/widget.h"
#include "simui/window.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <time.h>

#define HIGHEST_PRIORITY (WINDOW_LIMIT + 1)

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

typedef struct {
  simui_text_t *text;
  vec2f offset;
} font_offset_t;

typedef struct {
  simui_widget_t *widget;
  vec2f offset;
} widget_offset_t;

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
        char tmp_buffer[256] = {0};
        strcpy(tmp_buffer, a->title);
        strcpy(a->title, b->title);
        strcpy(b->title, tmp_buffer);
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

// simui context functions implementation
simui_context_t simui_context_create(SDL_Window *target_window,
                                     SDL_Renderer *renderer) {
  srand(time(NULL));
  simui_context_t context;

  TTF_Init();

  if (target_window == NULL || renderer == NULL) {
    error("Need to specify window and renderer.\n");
  } else {
    context.target_window = target_window;
    context.renderer = renderer;
  }

  context.window_buffer_index = 0;
  context.text_buffer_index = 0;
  context.widget_buffer_index = 0;
  context.focused_uuid = 0;
  context.sort_window_buffer = false;
  SDL_RWops *font_data_rwops =
      SDL_RWFromMem(proggy_clean_font_data, proggy_clean_font_data_len);
  context.font = TTF_OpenFontRW(font_data_rwops, 1, 32);
  return context;
}

void simui_context_handle_event(simui_context_t *context, SDL_Event *event) {
  if (context->window_buffer_index < 1) {
    return;
  }

  static vec2i offset = {.x = 0, .y = 0};

  static font_offset_t font_offsets[256] = {0};
  static size_t font_offset_index = 0;

  static widget_offset_t widget_offsets[256] = {0};
  static size_t widget_offset_index = 0;

  static bool move_scrollbar_thumb = false;
  static vec2f thumb_offset = {.x = 0, .y = 0};

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
        widget_offset_index = 0;
        memset(widget_offsets, 0, sizeof(widget_offsets));

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

          for (size_t i = 0; i < current_window->widget_uuid_buffer_index;
               ++i) {
            simui_widget_t *widget =
                get_widget(context, current_window->widget_uuid_buffer[i]);
            widget_offsets[widget_offset_index++] = (widget_offset_t){
                .widget = widget,
                .offset = (vec2f){.x = widget->pos.x - current_window->pos.x,
                                  .y = widget->pos.y - current_window->pos.y}};
          }

          thumb_offset.x =
              current_window->scrollbar_thumb.x - current_window->pos.x;
          thumb_offset.y =
              current_window->scrollbar_thumb.y - current_window->pos.y;
        }

        if (event->motion.x > (current_window->scrollbar_thumb.x) &&
            event->motion.x < (current_window->scrollbar_thumb.x + 20.0f) &&
            event->motion.y > (current_window->scrollbar_thumb.y) &&
            event->motion.y < (current_window->scrollbar_thumb.y + 20.0f) &&
            current_window->scroll) {
          move_scrollbar_thumb = true;
          thumb_offset.x = event->motion.x - current_window->scrollbar_thumb.x;
          thumb_offset.y = event->motion.y - current_window->scrollbar_thumb.y;
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
    move_scrollbar_thumb = false;
  }

  for (size_t i = 0;
       i < context->window_buffer[context->window_buffer_index - 1]
               ->widget_uuid_buffer_index;
       ++i) {
    uint64_t uuid = context->window_buffer[context->window_buffer_index - 1]
                        ->widget_uuid_buffer[i];
    simui_widget_t *widget = get_widget(context, uuid);

    if (event->motion.x > widget->pos.x &&
        event->motion.x < widget->pos.x + widget->size.x &&
        event->motion.y > widget->pos.y &&
        event->motion.y < widget->pos.y + widget->size.y) {
      if (widget->type == SIMUI_BUTTON) {
        if (event->type == SDL_MOUSEBUTTONDOWN &&
            event->button.button == SDL_BUTTON_LEFT) {
          widget->button.clicked = 1;
        }
        if (event->type == SDL_MOUSEBUTTONUP &&
            event->button.button == SDL_BUTTON_LEFT) {
          widget->button.clicked = 0;
        }
      } else if (widget->type == SIMUI_CHECKBOX) {
        if (event->type == SDL_MOUSEBUTTONDOWN &&
            event->button.button == SDL_BUTTON_LEFT) {
          widget->checkbox.active = !widget->checkbox.active;
        }
      }
    }
  }

  if (context->window_buffer[context->window_buffer_index - 1]->moving) {
    simui_window_t *current_window =
        context->window_buffer[context->window_buffer_index - 1];
    current_window->pos.x = event->motion.x - offset.x;
    current_window->pos.y = event->motion.y - offset.y;
    for (size_t i = 0; i < font_offset_index; ++i) {
      font_offsets[i].text->pos.x =
          current_window->pos.x + font_offsets[i].offset.x;
      font_offsets[i].text->pos.y =
          current_window->pos.y + font_offsets[i].offset.y;
    }
    for (size_t i = 0; i < widget_offset_index; ++i) {
      widget_offsets[i].widget->pos.x =
          current_window->pos.x + widget_offsets[i].offset.x;
      widget_offsets[i].widget->pos.y =
          current_window->pos.y + widget_offsets[i].offset.y;
    }
    current_window->scrollbar_thumb.x = current_window->pos.x + thumb_offset.x;
    current_window->scrollbar_thumb.y = current_window->pos.y + thumb_offset.y;
  }

  if (move_scrollbar_thumb) {
    simui_window_t *current_window =
        context->window_buffer[context->window_buffer_index - 1];

    vec2f initial_pos = (vec2f){.x = current_window->scrollbar_thumb.x,
                                .y = current_window->scrollbar_thumb.y};

    current_window->scrollbar_thumb.y = MIN(
        current_window->pos.y + current_window->size.y - 21.0f,
        MAX(event->motion.y - thumb_offset.y, current_window->pos.y + 30.0f));
    if (current_window->scrollbar_thumb.y > current_window->pos.y + 30 &&
        current_window->scrollbar_thumb.y <
            current_window->pos.y + current_window->size.y - 21.0f) {
      for (size_t i = 0; i < current_window->font_uuid_buffer_index; ++i) {
        simui_text_t *text =
            get_text(context, current_window->font_uuid_buffer[i]);
        if (text->uuid != current_window->title_uuid) {
          text->pos.y += initial_pos.y - current_window->scrollbar_thumb.y;
        }
      }
      for (size_t i = 0; i < current_window->widget_uuid_buffer_index; ++i) {
        simui_widget_t *widget =
            get_widget(context, current_window->widget_uuid_buffer[i]);
        widget->pos.y += (initial_pos.y - current_window->scrollbar_thumb.y);
      }
    }
  }
}

void simui_context_render(simui_context_t *context) {
  if (context->window_buffer_index < 1) {
    return;
  }
  regenerate_fonts(context);
  for (size_t i = 0; i < context->window_buffer_index; ++i) {
    simui_window_t *current_window = context->window_buffer[i];
    SDL_FRect background = {.x = current_window->pos.x,
                            .y = current_window->pos.y,
                            .w = current_window->size.x,
                            .h = current_window->size.y};

    SDL_FRect titlebar = {.x = current_window->pos.x,
                          .y = current_window->pos.y,
                          .w = current_window->size.x,
                          .h = 30.0f};

    SDL_SetRenderDrawColor(context->renderer, 30, 30, 30, 255);
    SDL_RenderFillRectF(context->renderer, &background);

    SDL_SetRenderDrawColor(context->renderer, 20, 20, 20, 255);
    SDL_RenderFillRectF(context->renderer, &titlebar);

    SDL_SetRenderDrawColor(context->renderer, 150, 150, 150, 255);
    SDL_RenderDrawRectF(context->renderer, &background);

    if (current_window->scroll) {
      SDL_FRect scrollbar = {
          .x = (current_window->pos.x + current_window->size.x) - 21.0f,
          .y = current_window->pos.y + 30.0f,
          .w = 20.0f,
          .h = current_window->size.y - 31.0f};
      SDL_SetRenderDrawColor(context->renderer, 70, 70, 70, 255);
      SDL_RenderFillRectF(context->renderer, &scrollbar);
      SDL_FRect scrollbar_thumb = {.x = current_window->scrollbar_thumb.x,
                                   .y = current_window->scrollbar_thumb.y,
                                   .w = 20.0f,
                                   .h = 20.0f};

      SDL_SetRenderDrawColor(context->renderer, 50, 50, 50, 255);
      SDL_RenderFillRectF(context->renderer, &scrollbar_thumb);
    }

    for (size_t j = 0; j < current_window->widget_uuid_buffer_index; ++j) {
      uint64_t widget_uuid = current_window->widget_uuid_buffer[j];
      simui_widget_t *widget = get_widget(context, widget_uuid);

      switch (widget->type) {
      case SIMUI_BUTTON: {
        SDL_Rect button_rect = {.x = widget->pos.x,
                                .y = widget->pos.y,
                                .w = widget->size.x,
                                .h = widget->size.y};
        if (widget->size.x == 0 || widget->size.y == 0) {
          continue;
        }
        SDL_SetRenderDrawColor(context->renderer, 50, 50, 50, 255);
        SDL_RenderFillRect(context->renderer, &button_rect);
        SDL_SetRenderDrawColor(context->renderer, 255, 255, 255, 255);
        SDL_RenderDrawRect(context->renderer, &button_rect);
        break;
      }
      case SIMUI_CHECKBOX: {
        SDL_FRect checkbox_outline = {.x = widget->pos.x,
                                      .y = widget->pos.y,
                                      .w = widget->size.x,
                                      .h = widget->size.y};

        SDL_FRect checkbox_fill = {.x = widget->pos.x + 3.0f,
                                   .y = widget->pos.y + 3.0f,
                                   .w = widget->size.x - 6.0f,
                                   .h = widget->size.y - 6.0f};
        SDL_SetRenderDrawColor(context->renderer, 200, 200, 200, 255);
        if (widget->checkbox.active) {
          SDL_RenderFillRectF(context->renderer, &checkbox_fill);
        }
        SDL_RenderDrawRectF(context->renderer, &checkbox_outline);
        break;
      }
      }
    }

    for (size_t j = 0; j < current_window->font_uuid_buffer_index; ++j) {
      uint64_t text_uuid = current_window->font_uuid_buffer[j];
      simui_text_t *text = get_text(context, text_uuid);
      if (text_uuid != current_window->title_uuid &&
              text->pos.y < (current_window->pos.y + 30.0f) ||
          text->pos.y >
              (current_window->pos.y + current_window->size.y - 30.0f)) {
        current_window->scroll = true;
        continue;
      }
      SDL_Rect font_rect = {.x = text->pos.x,
                            .y = text->pos.y,
                            .w = text->size.x,
                            .h = text->size.y};
      SDL_RenderCopy(context->renderer, text->texture, NULL, &font_rect);
    }
  }
}

bool simui_button_clicked(simui_context_t *context, uint64_t uuid) {
  simui_widget_t *button_widget = get_widget(context, uuid);
  if (button_widget == NULL) {
    error("button [uuid: %lx] not found.\n", uuid);
  }
  if (button_widget->type != SIMUI_BUTTON) {
    error("widget [uuid: %lx] is not a button.\n", uuid);
  }
  return button_widget->button.clicked;
}

bool simui_checkbox_active(simui_context_t *context, uint64_t uuid) {
  simui_widget_t *checkbox_widget = get_widget(context, uuid);
  if (checkbox_widget == NULL) {
    error("checkbox [uuid: %lx] not found.\n", uuid);
  }
  if (checkbox_widget->type != SIMUI_CHECKBOX) {
    error("widget [uuid: %lx] is not a checkbox.\n", uuid);
  }
  return checkbox_widget->checkbox.active;
}

void simui_context_destroy(simui_context_t *context) {
  for (size_t i = 0; i < context->window_buffer_index; ++i) {
    free(context->window_buffer[i]);
  }
  for (size_t i = 0; i < context->text_buffer_index; ++i) {
    SDL_DestroyTexture(context->text_buffer[i]->texture);
    free(context->text_buffer[i]);
  }

  for (size_t i = 0; i < context->widget_buffer_index; ++i) {
    free(context->widget_buffer[i]);
  }
  TTF_CloseFont(context->font);
  TTF_Quit();
}

#undef MIN
#undef MAX