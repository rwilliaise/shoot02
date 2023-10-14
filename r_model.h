
#ifndef R_MODEL_H_
#define R_MODEL_H_

#include "shoot02.h"

typedef struct {
    uint32_t vao;
    uint32_t vbo;
    uint32_t ebo;
    uint32_t vertices;
} r_mesh_t;

typedef struct {
    uint32_t mesh_count;
    r_mesh_t meshes[0];
} r_model_t;

r_model_t *r_model_load(const char *path);
void r_model_draw(r_model_t *M);
void r_model_free(r_model_t *M);

#endif // R_MODEL_H_
