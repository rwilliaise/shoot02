
#include "shoot02.h"

#include "r_model.h"
#include "r_shader.h"
#include "r_camera.h"
#include "glad/gl.h"
#include "stb_image.h"

#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <stdio.h>

GLFWwindow *r_window = NULL;

uint32_t program = 0;
r_camera_t camera = {
    .projection_view = GLM_MAT4_IDENTITY_INIT,
    .projection = GLM_MAT4_IDENTITY_INIT,
    .fovy = 90,
    .pos = { 0, 0, 1.5 },
    .rot = GLM_QUAT_IDENTITY_INIT,
};

static void r_print_error(int error_code, const char *desc) {
    fprintf(stderr, "GLFW ERROR (0x%.04x): %s\n", error_code, desc);
}

static void r_resize(GLFWwindow *win, int w, int h) {
    r_camera_update_projection(&camera, (float) w / h);
    if (program != 0)
        r_camera_upload(&camera, program);
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

    GLFWimage icon;
    icon.pixels = stbi_load("res/textures/eyes.png", &icon.width, &icon.height, NULL, 0);
    glfwSetWindowIcon(r_window, 1, &icon);
    stbi_image_free(icon.pixels);

    program = r_create_render_program("res/shaders/color.vert", "res/shaders/color.frag");

    if (!program) {
        glfwTerminate();
        return 1;
    }

    glUseProgram(program);

    uint32_t texture;
    int w, h, channels;
    unsigned char *data = stbi_load("res/textures/eyes.png", &w, &h, &channels, 3);

    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);

    r_model_t *cube_model = r_model_load("res/models/monkey.obj");

    glUniform1i(glGetUniformLocation(program, "tex"), 0);

    r_camera_update_projection(&camera, 4.f / 3.f);
    r_camera_upload(&camera, program);

    mat4 model = GLM_MAT4_IDENTITY_INIT;
    int model_location = glGetUniformLocation(program, "model");

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(r_window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm_rotate(model, 0.01f, (vec3) { 0, 1, 0 });
        glUniformMatrix4fv(model_location, 1, GL_FALSE, model[0]);
        glBindTexture(GL_TEXTURE_2D, texture);

        r_model_draw(cube_model);

        glfwSwapBuffers(r_window);
        glfwPollEvents();
    }

    r_model_free(cube_model);
    glfwTerminate();

    return 0;
}

