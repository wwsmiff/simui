#include "helper.h"
#include "context.h"
#include "text.h"

simui_text_t *get_text(simui_context_t *context, uint64_t uuid) {
  for (size_t i = 0; i < context->text_buffer_index; ++i) {
    if (context->text_buffer[i]->uuid == uuid) {
      return context->text_buffer[i];
    }
  }
  return NULL;
}