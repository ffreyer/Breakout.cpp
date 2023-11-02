#version 330 core

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

flat in vec2[] g_size;
flat in vec4[] g_color;

flat out vec4 f_color;

// uniform mat4 model;
uniform mat4 projectionview;

void generate_vertex(vec2 dir) {
    vec2 offset = g_size[0] * dir;
    gl_Position = projectionview * (gl_in[0].gl_Position + vec4(offset, 0, 0));
    f_color = g_color[0];
    EmitVertex();
}

void main() {
    generate_vertex(vec2(0, 0));
    generate_vertex(vec2(0, 1));
    generate_vertex(vec2(1, 0));
    generate_vertex(vec2(1, 1));
    EndPrimitive();
}