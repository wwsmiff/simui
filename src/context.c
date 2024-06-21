#include "context.h"
#include "window.h"
#include <SDL2/SDL.h>

#define HIGHEST_PRIORITY (WINDOW_LIMIT + 1)

simui_context_t simui_context_create(SDL_Window *target_window,
                                     SDL_Renderer *renderer) {

  simui_context_t new_context;

  if (target_window == NULL && renderer != NULL) {
    fprintf(stderr, "Only renderer specified.\n");
    exit(1);
  } else if (target_window != NULL && renderer == NULL) {
    fprintf(stderr, "Only window specified.\n");
    exit(1);
  } else if (target_window == NULL && renderer == NULL) {
    fprintf(stderr, "Need to specify window and renderer.\n");
    exit(1);
  } else {
    new_context.target_window = target_window;
    new_context.renderer = renderer;
  }

  new_context.window_buffer_index = 0;
  new_context.focused_uuid = 0;
  new_context.sort_window_buffer = false;

  return new_context;
}

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

void simui_context_handle_event(simui_context_t *context, SDL_Event *event) {
  static vec2i offset = {.x = 0, .y = 0};

  for (size_t i = 0; i < context->window_buffer_index; ++i) {
    simui_window_t *current_window = context->window_buffer[i];
    if (event->motion.x > current_window->pos.x &&
        event->motion.x < (current_window->pos.x + current_window->size.x) &&
        event->motion.y > current_window->pos.y &&
        event->motion.y < (current_window->pos.y + current_window->size.y)) {
      if (event->type == SDL_MOUSEBUTTONDOWN &&
          event->button.button == SDL_BUTTON_LEFT) {
        current_window->priority = HIGHEST_PRIORITY;
        context->focused_uuid = current_window->uuid;
        context->sort_window_buffer = true;
        offset.x = event->motion.x - current_window->pos.x;
        offset.y = event->motion.y - current_window->pos.y;
        if (event->motion.y < (current_window->pos.y + 30.0f)) {
          current_window->moving = true;
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
  }
}

void simui_context_render(simui_context_t *context) {
  for (size_t i = 0; i < context->window_buffer_index; ++i) {
    SDL_Color bg_color = context->window_buffer[i]->color;
    SDL_FRect background = {.x = context->window_buffer[i]->pos.x,
                            .y = context->window_buffer[i]->pos.y,
                            .w = context->window_buffer[i]->size.x,
                            .h = context->window_buffer[i]->size.y};

    SDL_FRect titlebar = {.x = context->window_buffer[i]->pos.x,
                          .y = context->window_buffer[i]->pos.y,
                          .w = context->window_buffer[i]->size.x,
                          .h = 30.0f};

    SDL_SetRenderDrawColor(context->renderer, bg_color.r, bg_color.g,
                           bg_color.b, 255);
    SDL_RenderFillRectF(context->renderer, &background);

    SDL_SetRenderDrawColor(context->renderer, 20, 20, 20, 255);
    SDL_RenderFillRectF(context->renderer, &titlebar);

    SDL_SetRenderDrawColor(context->renderer, 150, 150, 150, 255);
    SDL_RenderDrawRectF(context->renderer, &background);
  }
}

void simui_context_destroy(simui_context_t *context) {
  for (size_t i = 0; i < context->window_buffer_index; ++i) {
    free(context->window_buffer[i]);
  }
}