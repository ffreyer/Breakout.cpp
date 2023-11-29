#version 330 core

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

flat in float[] g_radius;
flat in vec4[] g_color;

out vec2 f_sdf;
flat out float f_radius;
flat out vec4 f_color;

// uniform mat4 model;
uniform mat4 projectionview;
uniform vec2 resolution;

#define AA_pad 4

void generate_vertex(vec4 origin, vec2 dir) {
    vec4 clip_pos = origin + projectionview * vec4(g_radius[0] * dir, 0, 0);
    clip_pos.xy = clip_pos.xy + AA_pad * dir / resolution;
    gl_Position = clip_pos;

    f_sdf = resolution * (clip_pos.xy - origin.xy);
    f_radius = min(resolution.x, resolution.y) * g_radius[0];
    f_color = g_color[0];
    EmitVertex();
}

void main() {
    vec4 origin = projectionview * gl_in[0].gl_Position;
    generate_vertex(origin, vec2(-1, -1));
    generate_vertex(origin, vec2(-1,  1));
    generate_vertex(origin, vec2( 1, -1));
    generate_vertex(origin, vec2( 1,  1));
    EndPrimitive();
}