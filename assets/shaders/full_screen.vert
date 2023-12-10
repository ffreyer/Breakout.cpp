#version 330 core

layout (location = 0) in vec2 v_pos;

out vec2 f_uv;

void main() {
    gl_Position = vec4(v_pos, 0, 1);
    f_uv = 0.5 + 0.5 * v_pos;
}