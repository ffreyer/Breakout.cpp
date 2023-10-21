#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in float radius;

out float g_radius;

// uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec2 resolution;

void main() {
    // vec4 pos = projection * view * model * vec4(position, 1);
    // gl_Position = vec4(0.5 * resolution, 1, 1) * pos / pos.w + vec4(0.5 * resolution, 0, 0);
    g_radius = radius;
    // gl_Position = vec4(400, 300, 0, 1);
    vec4 pos = projection * view  * vec4(position, 1);
    gl_Position = vec4(0.5 * resolution, 1, 1) * pos / pos.w + vec4(0.5 * resolution, 0, 0);
    // g_radius = 50.0f;
}

