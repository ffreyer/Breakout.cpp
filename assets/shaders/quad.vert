#version 330 core

layout (location = 0) in vec3 v_pos;
layout (location = 1) in vec4 v_color;

uniform mat4 view;
uniform mat4 projection;

flat out vec4 f_color;

void main() {
    gl_Position = projection * view * vec4(v_pos, 1.0);
    f_color = v_color;
}

