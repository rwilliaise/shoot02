
#ifndef R_CAMERA_H_
#define R_CAMERA_H_

#include "cglm/cglm.h"

typedef struct {
    mat4 projection_view;
    mat4 projection;
    float fovy;
    vec3 pos;
    versor rot;
} r_camera_t;

void r_camera_upload(r_camera_t *C, uint32_t program);

void r_camera_update_projection(r_camera_t *C, float aspect);
void r_camera_update(r_camera_t *C);

#endif // R_CAMERA_H_

