
#ifndef R_CAMERA_H_
#define R_CAMERA_H_

#include "cglm/cglm.h"

typedef struct {
    mat4 projection;
    vec3 position;
    vec3 forward;
    float fovy;
    float yaw;
    float pitch;
} r_camera_t;

void r_camera_update(r_camera_t *C, uint32_t program);
void r_camera_update_projection(r_camera_t *C, float aspect);

#endif // R_CAMERA_H_

