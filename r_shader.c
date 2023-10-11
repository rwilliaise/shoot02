
#include "r_shader.h"

#include <glad/gl.h>
#include <stdlib.h>
#include <stdio.h>
#include <shoot02.h>
#include <res.h>

static int r_check_compile(uint32_t shader, const char *shader_type) {
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);   
    if (success == GL_FALSE) {
        char info[512];
        glGetShaderInfoLog(shader, 512, NULL, info);
        printf("GL COMPILE ERROR (%s):\n%s", shader_type, info);
        return 1;
    }
    return 0;
}

uint32_t r_create_render_program(const char *vert, const char *frag) {
    const char *vert_source = res_from_path(vert);
    const char *frag_source = res_from_path(frag);
    
    if (vert_source == NULL || frag_source == NULL) { 
        free((char *) vert_source);
        free((char *) frag_source);
        return 0;
    }
    
    uint32_t vert_shader = glCreateShader(GL_VERTEX_SHADER);
    uint32_t frag_shader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(vert_shader, 1, &vert_source, NULL);
    glShaderSource(frag_shader, 1, &frag_source, NULL);

    glCompileShader(vert_shader);
    glCompileShader(frag_shader);

    if (r_check_compile(vert_shader, "vert") || r_check_compile(frag_shader, "frag")) {
        glDeleteShader(vert_shader);
        glDeleteShader(frag_shader);
        return 0;
    }
    
    uint32_t out = glCreateProgram();

    glAttachShader(out, vert_shader);
    glAttachShader(out, frag_shader);

    glLinkProgram(out);

    glDeleteShader(vert_shader);
    glDeleteShader(frag_shader);

    int success;
    glGetProgramiv(out, GL_LINK_STATUS, &success);

    if (success == GL_FALSE) {
        char info[512];
        glGetProgramInfoLog(out, 512, NULL, info);
        printf("GL LINK ERROR:\n%s", info);
        return 0;
    }

    free((char *) vert_source);
    free((char *) frag_source);
    return out;
}

