#include "widget.h"
#include "context.h"
#include "text.h"
#include "window.h"

#define MAX(x, y) ((x) > (y)) ? (x) : (y)

uint64_t simui_window_widget_create(simui_context_t *context,
                                    simui_widget_type_t type, const char *label,
                                    vec2f pos, vec2f size) {

  simui_widget_t *widget = (simui_widget_t *)(malloc(sizeof(simui_widget_t)));

  simui_window_t *current_window =
      context->window_buffer[context->window_buffer_index - 1];

  widget->uuid =
      ((uint64_t)(rand() % UINT64_MAX) << 16) ^ context->widget_buffer_index;
  widget->type = type;
  widget->pos = (vec2f){.x = current_window->pos.x + pos.x,
                        .y = current_window->pos.y + pos.y};
  widget->size = size;
  widget->clicked = false;

  context->widget_buffer[context->widget_buffer_index++] = widget;
  current_window
      ->widget_uuid_buffer[current_window->widget_uuid_buffer_index++] =
      widget->uuid;

  simui_window_text_create(context, label, (vec2f){pos.x, pos.y});

  simui_text_t *label_text =
      context->text_buffer[context->text_buffer_index - 1];

  widget->size.x = MAX(size.x, label_text->size.x + 20);
  widget->size.y = MAX(size.y, label_text->size.y);

  label_text->pos.x = (widget->pos.x / 2) +
                      ((widget->pos.x + widget->size.x) / 2) -
                      (label_text->size.x / 2);
  label_text->pos.y = (widget->pos.y / 2) +
                      ((widget->pos.y + widget->size.y) / 2) -
                      (label_text->size.y / 2);

  return widget->uuid;
}