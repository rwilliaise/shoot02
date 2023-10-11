#version 330 core
layout (location = 0) in vec3 vert_pos;

uniform mat4 perspective;

void main() {
    gl_Position = vec4(vert_pos, 1.0);
}

