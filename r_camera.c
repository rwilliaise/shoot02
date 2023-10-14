
#include "shoot02.h"
#include "r_camera.h"

void r_camera_update(r_camera_t *C, uint32_t program) {
    mat4 view = GLM_MAT4_IDENTITY_INIT;
    glm_look(C->position, C->forward, (vec3) { 0, 1, 0 }, view);

    glUseProgram(program);
    int view_location = glGetUniformLocation(program, "view");
    int projection_location = glGetUniformLocation(program, "projection");
    glUniformMatrix4fv(view_location, 1, GL_FALSE, view[0]);
    glUniformMatrix4fv(projection_location, 1, GL_FALSE, C->projection[0]);
}

void r_camera_update_projection(r_camera_t *C, float aspect) {
    glm_perspective(glm_rad(C->fovy), aspect, 0.1f, 1000.f, C->projection);
}
