#version 330 core
out vec4 frag_color;

in vec2 texcoord;
in float darkness;

uniform sampler2D tex;

void main() {
    frag_color = darkness * texture(tex, texcoord);
}
