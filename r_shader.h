
#ifndef R_SHADER_H_
#define R_SHADER_H_

#include <stdint.h>

typedef uint32_t r_shader_t;

r_shader_t r_create_render_program(const char *vert, const char *frag);

#endif // R_SHADER_H_
