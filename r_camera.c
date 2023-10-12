
#include "shoot02.h"
#include "r_camera.h"

void r_camera_upload(r_camera_t *C, uint32_t program) {
    glUseProgram(program);
    int projection_view_location = glGetUniformLocation(program, "projection_view");
    glUniformMatrix4fv(projection_view_location, 1, GL_FALSE, C->projection_view[0]);
}

void r_camera_update_projection(r_camera_t *C, float aspect) {
    glm_perspective(glm_rad(C->fovy), aspect, 0.1f, 1000.f, C->projection);
    r_camera_update(C);
}

void r_camera_update(r_camera_t *C) {
    mat4 view = GLM_MAT4_IDENTITY_INIT;
    vec3 neg_pos;
    glm_vec3_copy(C->pos, neg_pos);
    glm_vec3_negate(neg_pos);
    glm_translate(view, neg_pos);
    glm_quat_rotate(view, C->rot, view);
    glm_mat4_mul(C->projection, view, C->projection_view);
}

