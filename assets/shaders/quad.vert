#version 330 core

layout (location = 0) in vec3 v_pos;
layout (location = 1) in vec2 v_size;
layout (location = 2) in vec4 v_color;

flat out vec2 g_size;
flat out vec4 g_color;

void main() {
    gl_Position = vec4(v_pos, 1);
    g_size = v_size;
    g_color = v_color;
}