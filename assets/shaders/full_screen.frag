#version 330 core

in vec2 f_uv;

out vec4 FragColor;

uniform sampler2D tex;

void main() {
    // vec4 color = texture(tex, f_uv);
    float depth = texture(tex, f_uv).r;
    FragColor = vec4(vec3(depth), 1.0);
}