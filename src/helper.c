#include "helper.h"
#include "context.h"
#include "text.h"
#include "widget.h"

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