#version 330 core

layout (location = 0) in vec3 v_position;
layout (location = 1) in float v_radius;
layout (location = 2) in vec4 v_color;

flat out float g_radius;
flat out vec4 g_color;

void main() {
    gl_Position = vec4(v_position, 1);
    g_radius = v_radius;
    g_color = v_color;
}

