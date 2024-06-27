#ifndef SIMUI_WIDGET_H_
#define SIMUI_WIDGET_H_

#include "vec2.h"
#include <stdbool.h>

struct simui_context_t;

typedef enum simui_widget_type_t {
  SIMUI_BUTTON,
} simui_widget_type_t;

typedef struct simui_widget_t {
  const char *label;
  uint64_t uuid;
  simui_widget_type_t type;
  vec2f pos;
  vec2f size;
  bool clicked;
} simui_widget_t;

uint64_t simui_window_widget_create(struct simui_context_t *context,
                                    simui_widget_type_t type, const char *label,
                                    vec2f position, vec2f size);

#endif // SIMUI_WIDGET_H_
