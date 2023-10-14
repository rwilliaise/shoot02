#version 330 core
layout (location = 0) in vec3 vert_pos;
layout (location = 1) in vec3 vert_normal;
layout (location = 2) in vec2 vert_uv;

out vec2 texcoord;
out float darkness;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main() {
    vec3 normal_localspace = normalize((model * vec4(vert_normal, 1.0)).xyz);
    gl_Position = projection * view * model * vec4(vert_pos, 1.0);
    texcoord = vert_uv;
    darkness = dot(vec3(0, 0, 1), normal_localspace) * 0.5 + 0.5;
}

