#include "simui/helper.h"
#include "simui/context.h"
#include "simui/text.h"
#include "simui/widget.h"
#include "simui/window.h"

#include <stdio.h>

void error(const char *fmt, ...) {
  va_list vargs;
  va_start(vargs, fmt);
  fprintf(stderr, fmt, vargs);
  va_end(vargs);
  exit(1);
}

// TODO: implement better searches.

simui_text_t *get_text(simui_context_t *context, uint64_t uuid) {
  for (size_t i = 0; i < context->text_buffer_index; ++i) {
    if (context->text_buffer[i]->uuid == uuid) {
      return context->text_buffer[i];
    }
  }
  return NULL;
}

simui_widget_t *get_widget(simui_context_t *context, uint64_t uuid) {
  for (size_t i = 0; i < context->widget_buffer_index; ++i) {
    if (context->widget_buffer[i]->uuid == uuid) {
      return context->widget_buffer[i];
    }
  }
  return NULL;
}

simui_window_t *get_window(simui_context_t *context, uint64_t uuid) {
  for (size_t i = 0; i < context->window_buffer_index; ++i) {
    if (context->window_buffer[i]->uuid == uuid) {
      return context->window_buffer[i];
    }
  }

  return NULL;
}