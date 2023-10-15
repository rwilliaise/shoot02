
#ifndef R_RES_H_
#define R_RES_H_

#include "shoot02.h"

#define R_RES_TEXTURES_MAX 64
#define r_res_texture_from_namec(name) r_res_texture_from_name(strdup(name))

typedef struct {
    uint32_t id;
    uint32_t rc;
    uint32_t w;
    uint32_t h;
    char *name;
} r_texture_t;

r_texture_t *r_res_texture_lookup(const char *name);
r_texture_t *r_res_texture_from_name(char *name);

r_texture_t *r_res_texture_ref(r_texture_t *T);
void r_res_texture_unref(r_texture_t *T);
void r_res_texture_bind(r_texture_t *T);

#endif // R_RES_H_
