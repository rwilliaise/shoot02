
#ifndef R_RES_H_
#define R_RES_H_

#include "shoot02.h"

#define R_RES_TEXTURES_MAX 64

typedef struct {
    uint32_t id;
    uint32_t rc;
    char *name;
} r_texture_t;

r_texture_t *r_res_texture_lookup(const char *name);
r_texture_t *r_res_texture_from_name(const char *name);

r_texture_t *r_res_texture_ref(r_texture_t *T);
void r_res_texture_unref(r_texture_t *T);
void r_res_texture_bind(r_texture_t *T);

#endif // R_RES_H_
