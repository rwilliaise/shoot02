
#ifndef R_MAP_H_
#define R_MAP_H_

#include "shoot02.h"

#include "r_model.h"
#include "r_texture.h"

#define R_MAP_INVERSE_SCALE_FACTOR 16 // tb units in one meter

typedef struct {
    r_texture_t *texture;
    r_mesh_t mesh;
} r_surface_t;

typedef struct {
    uint32_t surface_count;
    r_surface_t surfaces[0];
} r_map_t;

r_map_t *r_map_load(const char *path);
void r_map_draw(r_map_t *M);
void r_map_free(r_map_t *M);

#endif // R_MAP_H_

