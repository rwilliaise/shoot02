
#include "shoot02.h"
#include "r_map.h"
#include "r_model.h"
#include "r_texture.h"
#include "r_shader.h"
#include "r_camera.h"

#include "glad/gl.h"
#include "stb_image.h"

#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <string.h>

GLFWwindow *r_window = NULL;

uint32_t program = 0;
r_camera_t camera = {
    .projection = GLM_MAT4_IDENTITY_INIT,
    .forward = { 1, 0, 0 },
    .position = GLM_VEC3_ZERO_INIT,
    .fovy = 90.f,
    .yaw = 0,
    .pitch = 0,
};
double last_xpos = 0, last_ypos = 0;

static void r_print_error(int error_code, const char *desc) {
    fprintf(stderr, "GLFW ERROR (0x%.04x): %s\n", error_code, desc);
}

static void r_resize(GLFWwindow *win, int w, int h) {
    r_camera_update_projection(&camera, (float) w / h);
    if (program != 0)
        r_camera_update(&camera, program);
    glViewport(0, 0, w, h);
}

static void cl_freecam_move(float delta) {
    vec3 camera_right;
    vec3 freecam_direction = GLM_VEC3_ZERO_INIT;
    glm_vec3_cross(camera.forward, (vec3) { 0, 1, 0 }, camera_right);
    glm_normalize(camera_right);
    if (glfwGetKey(r_window, GLFW_KEY_W)) {
        glm_vec3_add(freecam_direction, camera.forward, freecam_direction);
    }
    if (glfwGetKey(r_window, GLFW_KEY_A)) {
        glm_vec3_sub(freecam_direction, camera_right, freecam_direction);
    }
    if (glfwGetKey(r_window, GLFW_KEY_S)) {
        glm_vec3_sub(freecam_direction, camera.forward, freecam_direction);
    }
    if (glfwGetKey(r_window, GLFW_KEY_D)) {
        glm_vec3_add(freecam_direction, camera_right, freecam_direction);
    }
    if (glfwGetKey(r_window, GLFW_KEY_ESCAPE)) {
        glfwSetInputMode(r_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }

    if (glm_vec3_dot(freecam_direction, freecam_direction) == 0) {
        return;
    }

    glm_vec3_scale(freecam_direction, delta * 15, freecam_direction);
    glm_vec3_add(camera.position, freecam_direction, camera.position);
}

static void cl_mouse_move(GLFWwindow *win, double xpos, double ypos) {
    if (glfwGetInputMode(win, GLFW_CURSOR) != GLFW_CURSOR_DISABLED)
        return;

    if (last_xpos == 0) {
        last_xpos = xpos;
        last_ypos = ypos;
        return;
    }

    float x_delta = last_xpos - xpos;
    float y_delta = last_ypos - ypos;
    last_xpos = xpos;
    last_ypos = ypos;

    x_delta *= 0.1;
    y_delta *= 0.1;

    camera.yaw -= x_delta;
    camera.pitch += y_delta;
    camera.pitch = glm_clamp(camera.pitch, -89, 89);

    camera.forward[0] = cos(glm_rad(camera.yaw)) * cos(glm_rad(camera.pitch));
    camera.forward[1] = sin(glm_rad(camera.pitch));
    camera.forward[2] = sin(glm_rad(camera.yaw)) * cos(glm_rad(camera.pitch));
}

static void cl_mouse_button(GLFWwindow *win, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        glfwSetInputMode(r_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
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
    if (type != GL_DEBUG_TYPE_ERROR) return;
    fprintf(stderr, "GL ERROR (0x%.04x): %s\n",
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

    r_texture_t *concrete = r_texture_from_namec("res/textures/conc00.png");
    r_model_t *monkey_model = r_model_load("res/models/monkey.obj");

    char *target_map;
    if (argc > 1) {
        const char *res_maps = "res/maps/";
        const char *d_map = ".map";
        char *base_name = argv[1];
        target_map = malloc(strlen(res_maps) + strlen(d_map) + strlen(base_name) + 1);
        *target_map = 0;
        strcat(target_map, res_maps);
        strcat(target_map, base_name);
        strcat(target_map, d_map);
    } else {
        target_map = "res/maps/debug00.map";
    }

    r_map_t *debug00 = r_map_load(target_map);
    if (argc > 1)
        free(target_map);

    // glUniform1i(glGetUniformLocation(program, "tex"), 0);

    r_camera_update_projection(&camera, 4.f / 3.f);
    r_camera_update(&camera, program);

    mat4 model = GLM_MAT4_IDENTITY_INIT;
    int model_location = glGetUniformLocation(program, "model");

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glfwSwapInterval(1);
    glfwSetMouseButtonCallback(r_window, cl_mouse_button);
    glfwSetCursorPosCallback(r_window, cl_mouse_move);

    double last = glfwGetTime();
    double delta;

    while (!glfwWindowShouldClose(r_window)) {
        delta = glfwGetTime() - last;
        last = glfwGetTime();

        cl_freecam_move(delta);
        r_camera_update(&camera, program);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUniformMatrix4fv(model_location, 1, GL_FALSE, GLM_MAT4_IDENTITY[0]);
        r_map_draw(debug00);

        glm_rotate(model, M_PI * delta, (vec3) { 0, 1, 0 });
        glUniformMatrix4fv(model_location, 1, GL_FALSE, model[0]);

        r_texture_bind(concrete);
        r_model_draw(monkey_model);

        glfwSwapBuffers(r_window);
        glfwPollEvents();
    }

    r_texture_unref(concrete);
    r_model_free(monkey_model);
    r_map_free(debug00);

    glfwTerminate();

    return 0;
}

