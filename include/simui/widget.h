#ifndef SIMUI_WIDGET_H_
#define SIMUI_WIDGET_H_

#include "vec2.h"
#include <stdbool.h>

struct simui_context_t;

typedef enum simui_widget_type_t {
  SIMUI_BUTTON,
  SIMUI_CHECKBOX,
} simui_widget_type_t;

typedef struct simui_widget_t {
  simui_widget_type_t type;
  const char *label;
  uint64_t uuid;
  vec2f pos;
  vec2f size;
  union {
    struct {
      bool clicked;
    } button;

    struct {
      bool active;
    } checkbox;
  };
} simui_widget_t;

#ifdef __cplusplus
extern "C" {
#endif

uint64_t simui_window_widget_create(struct simui_context_t *context,
                                    uint64_t window_uuid,
                                    simui_widget_type_t type, const char *label,
                                    vec2f position, vec2f size);
#ifdef __cplusplus
}
#endif

#endif // SIMUI_WIDGET_H_
