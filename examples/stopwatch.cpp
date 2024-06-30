#include "simui/simui.h"
#include <array>
#include <chrono>
#include <format>
#include <functional>
#include <memory>
#include <string>

int main() {
  SDL_Init(SDL_INIT_VIDEO);
  std::unique_ptr<SDL_Window, std::function<void(SDL_Window *)>> window{
      SDL_CreateWindow("SIMUI Stopwatch", SDL_WINDOWPOS_CENTERED,
                       SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN),
      SDL_DestroyWindow};

  std::unique_ptr<SDL_Renderer, std::function<void(SDL_Renderer *)>> renderer{
      SDL_CreateRenderer(window.get(), -1,
                         SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC),
      SDL_DestroyRenderer};

  bool running{true};
  bool paused{false};
  SDL_Event event;

  std::string stopwatch_string{};

  int hours{};
  int minutes{};
  int seconds{};
  int milliseconds{};

  simui_context_t context = simui_context_create(window.get(), renderer.get());
  simui_window_create(&context, (vec2f){50.0f, 50.0f}, (vec2f){300.0f, 400.0f});
  simui_window_set_title(&context, 0, "Stopwatch");
  simui_window_text_create(&context, 0, stopwatch_string.data(),
                           (vec2f){110.0f, 100.0f});
  uint64_t pause_checkbox = simui_window_widget_create(
      &context, 0, SIMUI_CHECKBOX, "Pause", (vec2f){115.0f, 200.0f},
      (vec2f){20.0f, 20.0f});
  uint64_t reset_button = simui_window_widget_create(
      &context, 0, SIMUI_BUTTON, "Reset", (vec2f){30.0f, 250.0f},
      (vec2f){100.0f, 25.0f});
  uint64_t quit_button = simui_window_widget_create(
      &context, 0, SIMUI_BUTTON, "Quit", (vec2f){150.0f, 250.0f},
      (vec2f){100.0f, 25.0f});

  auto start{std::chrono::high_resolution_clock::now()};

  while (running) {
    stopwatch_string = std::format("{:02d}:{:02d}:{:02d}.{:04d}", hours,
                                   minutes, seconds, milliseconds);

    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        running = false;
      }

      simui_context_handle_event(&context, &event);
      paused = simui_checkbox_active(&context, pause_checkbox);

      if (simui_button_clicked(&context, reset_button)) {
        hours = 0;
        minutes = 0;
        seconds = 0;
        milliseconds = 0;
      }
      if (simui_button_clicked(&context, quit_button)) {
        running = false;
      }
    }

    auto current{std::chrono::high_resolution_clock::now()};
    auto delta{
        std::chrono::duration_cast<std::chrono::milliseconds>(current - start)
            .count()};

    if (!paused) {
      milliseconds += delta;
    }

    if (milliseconds >= 1000) {
      seconds++;
      milliseconds = 0;
    }
    if (seconds >= 60) {
      minutes++;
      seconds = 0;
    }
    if (minutes >= 60) {
      hours++;
      minutes = 0;
    }
    if (hours >= 24) {
      hours = 0;
      minutes = 0;
      seconds = 0;
    }

    SDL_SetRenderDrawColor(renderer.get(), 100, 100, 255, 255);
    SDL_RenderClear(renderer.get());

    simui_context_render(&context);

    SDL_RenderPresent(renderer.get());

    start = current;
  }

  simui_context_destroy(&context);
  SDL_Quit();
}
