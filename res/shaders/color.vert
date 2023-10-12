#version 330 core
layout (location = 0) in vec3 vert_pos;
layout (location = 2) in vec2 vert_uv;

out vec2 texcoord;

uniform mat4 projection_view;
uniform mat4 model;

void main() {
    gl_Position = projection_view * model * vec4(vert_pos, 1.0);
    texcoord = vert_uv;
}
