#ifndef SIMUI_HELPER_H_
#define SIMUI_HELPER_H_

#include <stdint.h>

struct simui_context_t;
struct simui_text_t;
struct simui_widget_t;

struct simui_text_t *get_text(struct simui_context_t *context, uint64_t uuid);
struct simui_widget_t *get_widget(struct simui_context_t *context,
                                  uint64_t uuid);

#endif // SIMUI_HELPER_H_