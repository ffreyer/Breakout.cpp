#version 330 core

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

in float[] g_radius;

out vec2 f_sdf;
flat out float f_radius;

uniform vec2 resolution;

#define AA_pad 4

void generate_vertex( vec2 dir) {
    float r = g_radius[0] + AA_pad;
    vec4 px_pos = gl_in[0].gl_Position + vec4(r * dir.x, r * dir.y, 0, 0);
    vec4 clip_pos = vec4(2 * px_pos.x / resolution.x - 1, 2 * px_pos.y / resolution.y - 1, px_pos.z, px_pos.w);
    // vec4 clip_pos = vec4(2 * px_pos.x / 800 - 1, 2 * px_pos.y / 600 - 1, px_pos.z, px_pos.w);
    gl_Position = clip_pos;
    f_sdf = px_pos.xy - gl_in[0].gl_Position.xy;
    f_radius = g_radius[0];
    EmitVertex();
}

void main() {
    generate_vertex(vec2(-1, -1));
    generate_vertex(vec2(-1,  1));
    generate_vertex(vec2( 1, -1));
    generate_vertex(vec2( 1,  1));
    EndPrimitive();
}