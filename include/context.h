#ifndef SIMUI_CONTEXT_H_
#define SIMUI_CONTEXT_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define WINDOW_LIMIT 12

struct SDL_Window;
struct SDL_Renderer;
union SDL_Event;

struct simui_window_t;

typedef struct simui_context_t {
  struct SDL_Window *target_window;
  struct SDL_Renderer *renderer;
  struct simui_window_t *window_buffer[WINDOW_LIMIT];
  size_t window_buffer_index;
  size_t focused_uuid;
  bool sort_window_buffer;
} simui_context_t;

simui_context_t simui_context_create(struct SDL_Window *window,
                                     struct SDL_Renderer *renderer);

void simui_context_render(simui_context_t *context);
void simui_context_handle_event(simui_context_t *context,
                                union SDL_Event *event);
void simui_context_destroy(simui_context_t *context);

#endif // SIMHUI_CONTEXT_H_
