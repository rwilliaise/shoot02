
#include "shoot02.h"

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
    .pos = { 0, 0, 1 },
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

    const struct vertex { vec3 pos; vec2 uv; } cube[] = {
        { { 0.5, 0.5, 0 }, { 1.0, 0.0 } }, // top right
        { { 0.5, -0.5, 0 }, { 1.0, 1.0 } }, // bottom right
        { { -0.5, -0.5, 0 }, { 0.0, 1.0 } }, // bottom left
        { { -0.5, 0.5, 0 }, { 0.0, 0.0 } }, // top left
    };

    const uint32_t indices[] = {
        0, 1, 3,
        1, 2, 3,
    };

    GLFWimage icon;
    icon.pixels = stbi_load("res/textures/eyes.png", &icon.width, &icon.height, NULL, 0);
    glfwSetWindowIcon(r_window, 1, &icon);
    stbi_image_free(icon.pixels);

    program = r_create_render_program("res/shaders/color.vert", "res/shaders/color.frag");

    if (!program) {
        glfwTerminate();
        return 1;
    }

    uint32_t vao;
    uint32_t vbo;
    uint32_t ebo;

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(struct vertex), NULL);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(struct vertex), (void *) offsetof(struct vertex, uv));

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

    glUniform1i(glGetUniformLocation(program, "tex"), 0);

    r_camera_update_projection(&camera, 4.f / 3.f);
    r_camera_upload(&camera, program);

    mat4 model = GLM_MAT4_IDENTITY_INIT;
    int model_location = glGetUniformLocation(program, "model");

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    while (!glfwWindowShouldClose(r_window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        glm_rotate(model, 0.01f, (vec3) { 0, 1, 0 });
        glUniformMatrix4fv(model_location, 1, GL_FALSE, model[0]);
        glBindTexture(GL_TEXTURE_2D, texture);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
        glfwSwapBuffers(r_window);
        glfwPollEvents();
    }
    glfwTerminate();

    return 0;
}

