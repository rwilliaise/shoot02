
#ifndef R_MAP_H_
#define R_MAP_H_

#include "shoot02.h"

#include "r_model.h"
#include "r_res.h"

typedef struct {
    uint32_t mesh_count;
    uint32_t texture_count;
    r_texture_t **textures;
    r_mesh_t meshes[0];
} r_map_t;

r_map_t *r_map_load(const char *path);
void r_map_free(r_map_t *M);

#endif // R_MAP_H_

