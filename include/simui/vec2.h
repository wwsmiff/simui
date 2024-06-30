#ifndef SIMUI_VEC2_H_
#define SIMUI_VEC2_H_

#include <stdint.h>

typedef struct simui_vec2i_t {
  int32_t x, y;
} simui_vec2i_t;

typedef struct simui_vec2f_t {
  float x, y;
} simui_vec2f_t;

typedef struct simui_vec2u_t {
  uint32_t x, y;
} simui_vec2u_t;

typedef simui_vec2i_t vec2i;
typedef simui_vec2f_t vec2f;
typedef simui_vec2u_t vec2u;

#endif // SIMUI_VEC2_H_
