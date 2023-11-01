#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in float radius;

out float g_radius;

void main() {
    gl_Position = vec4(position, 1);
    g_radius = radius;
}

