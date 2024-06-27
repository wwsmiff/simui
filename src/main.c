#include "simui.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <time.h>

int main(void) {
  SDL_Init(SDL_INIT_VIDEO);
  TTF_Init();

  SDL_Window *target_window =
      SDL_CreateWindow("Test Window", SDL_WINDOWPOS_CENTERED,
                       SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN);
  SDL_Renderer *renderer = SDL_CreateRenderer(
      target_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

  bool running = true;
  SDL_Event e;

  simui_context_t context = simui_context_create(target_window, renderer);

  simui_window_create(&context, (vec2f){90.0f, 90.0f}, (vec2f){300.0f, 300.0f});
  simui_window_set_title(&context, "A new title.");
  simui_window_text_create(&context, "Lorem ipsum dolor.", (vec2f){5.0f, 5.0f});

  uint64_t btn1 =
      simui_window_widget_create(&context, SIMUI_BUTTON, "Button 1",
                                 (vec2f){5.0f, 75.0f}, (vec2f){50.0f, 25.0f});
  uint64_t btn2 =
      simui_window_widget_create(&context, SIMUI_BUTTON, "Button 2",
                                 (vec2f){5.0f, 125.0f}, (vec2f){50.0f, 25.0f});
  uint64_t btn3 = simui_window_widget_create(
      &context, SIMUI_BUTTON, "Very long button label", (vec2f){5.0f, 175.0f},
      (vec2f){50.0f, 25.0f});

  simui_window_create(&context, (vec2f){30.0f, 30.0f}, (vec2f){300.0f, 300.0f});
  simui_window_text_create(&context, "The quick brown fox.",
                           (vec2f){5.0f, 5.0f});
  uint64_t checkbox_1 =
      simui_window_widget_create(&context, SIMUI_CHECKBOX, "Checkbox",
                                 (vec2f){5.0f, 75.0f}, (vec2f){20.0f, 20.0f});

  while (running) {
    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_QUIT) {
        running = false;
      }
      simui_context_handle_event(&context, &e);
    }

    SDL_SetRenderDrawColor(renderer, 100, 100, 255, 255);
    SDL_RenderClear(renderer);

    if (simui_button_clicked(&context, btn1)) {
      printf("Button 1\n");
    }
    if (simui_button_clicked(&context, btn2)) {
      printf("Button 2\n");
    }
    if (simui_button_clicked(&context, btn3)) {
      printf("Button 3\n");
    }
    if (simui_checkbox_active(&context, checkbox_1)) {
      printf("Active!\n");
    }

    simui_context_render(&context);

    SDL_RenderPresent(renderer);
  }

  simui_context_destroy(&context);

  SDL_DestroyWindow(target_window);
  SDL_DestroyRenderer(renderer);
  SDL_Quit();
  TTF_Quit();

  return 0;
}
