#version 330 core

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

flat in vec2[] g_size;
flat in vec4[] g_color;

flat out vec4 f_color;

// uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void generate_vertex(mat4 pv, vec2 dir) {
    vec2 offset = g_size[0] * dir;
    gl_Position = pv * (gl_in[0].gl_Position + vec4(offset, 0, 0));
    f_color = g_color[0];
    EmitVertex();
}

void main() {
    mat4 pv = projection * view;
    generate_vertex(pv, vec2(0, 0));
    generate_vertex(pv, vec2(0, 1));
    generate_vertex(pv, vec2(1, 0));
    generate_vertex(pv, vec2(1, 1));
    EndPrimitive();
}