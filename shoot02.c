
#include "shoot02.h"

#include "r_shader.h"
#include "r_camera.h"

#include <cglm/cglm.h>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <stdio.h>

GLFWwindow *r_window = NULL;

static void r_print_error(int error_code, const char *desc) {
    fprintf(stderr, "GLFW ERROR (0x%.04x): %s\n", error_code, desc);
}

static void r_resize(GLFWwindow *win, int w, int h) {
    glViewport(0, 0, w, h);
}

static void r_handle_gl_error(
    GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar *message,
    const void *ud
) {
    fprintf(stderr, "GL %s (0x%.04x): %s\n",
            type == GL_DEBUG_TYPE_ERROR ? "ERROR" : "MESSAGE",
            severity,
            message
    );
}

int main(int argc, char *argv[]) {
    glfwSetErrorCallback(r_print_error);
    
    if (glfwInit() != GLFW_TRUE)
        return 1;

    r_window = glfwCreateWindow(640, 480, "shoot02", NULL, NULL);
    if (r_window == NULL) {
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(r_window);
    gladLoadGL(glfwGetProcAddress);

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(r_handle_gl_error, NULL);

    glfwSetFramebufferSizeCallback(r_window, r_resize);
    r_resize(r_window, 640, 480);

    const float cube[] = {
        0, 0.5, 0,
        -0.5, -0.5, 0,
        0.5, -0.5, 0
    };

    uint32_t vao;
    uint32_t vbo;

    uint32_t program = r_create_render_program("res/shaders/color.vert", "res/shaders/color.frag");

    if (!program) {
        glfwTerminate();
        return 1;
    }

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, NULL);
    glEnableVertexAttribArray(0);

    glUseProgram(program);

    r_camera_t camera = {
        .projection_view = GLM_MAT4_IDENTITY_INIT,
        .projection = GLM_MAT4_IDENTITY_INIT,
        .fovy = 90,
        .pos = { 0, 0, 1 },
        .rot = GLM_QUAT_IDENTITY_INIT,
    };

    r_camera_update_projection(&camera, 4.f / 3.f);
    r_camera_upload(&camera, program);

    mat4 model = GLM_MAT4_IDENTITY_INIT;
    int model_location = glGetUniformLocation(program, "model");

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    while (!glfwWindowShouldClose(r_window)) {
        glm_rotate(model, 0.01f, (vec3) { 0, 1, 0 });
        glUniformMatrix4fv(model_location, 1, GL_FALSE, model[0]);

        glClear(GL_COLOR_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glfwSwapBuffers(r_window);
        glfwPollEvents();
    }
    glfwTerminate();

    return 0;
}

