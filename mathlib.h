
#ifndef MATHLIB_H_
#define MATHLIB_H_

typedef float vec3_t[3];
typedef float vec4_t[4];

typedef vec4_t mat4_t[4];

#define MAT4_IDENTITY { \
    { 1, 0, 0, 0 }, \
    { 0, 1, 0, 0 }, \
    { 0, 0, 1, 0 }, \
    { 0, 0, 0, 1 }, \
}

void mat4_print(mat4_t M);
void mat4_perspective(mat4_t M, float fov, float far, float near);

#endif // MATHLIB_H_
