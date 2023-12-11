#version 330 core

// TODO: 
// Use viewdir to discard invisible faces (reducing max_vertices to 12)

layout (points) in;
layout (triangle_strip, max_vertices = 24) out;

flat in ivec3[] g_idx;

uniform isampler3D block_id;

uniform mat4 projectionview;
uniform mat4 model;

// Mesh generation constants
const int MAX_INDEX = 16;

const ivec3 OFFSETS[6] = ivec3[](
    ivec3(+1, 0, 0), ivec3(-1, 0, 0),
    ivec3(0, +1, 0), ivec3(0, -1, 0), 
    ivec3(0, 0, +1), ivec3(0, 0, -1)
);

const vec3 FACES[24] = vec3[24](
    vec3(1, 0, 0), vec3(1, 0, 1), vec3(1, 1, 0), vec3(1, 1, 1),
    vec3(0, 0, 0), vec3(0, 0, 1), vec3(0, 1, 0), vec3(0, 1, 1),
    vec3(0, 1, 0), vec3(0, 1, 1), vec3(1, 1, 0), vec3(1, 1, 1),
    vec3(0, 0, 0), vec3(0, 0, 1), vec3(1, 0, 0), vec3(1, 0, 1),
    vec3(0, 0, 1), vec3(0, 1, 1), vec3(1, 0, 1), vec3(1, 1, 1),
    vec3(0, 0, 0), vec3(0, 1, 0), vec3(1, 0, 0), vec3(1, 1, 0)
);

const vec3 NORMALS[6] = vec3[6](
    vec3(+1, 0, 0), vec3(-1, 0, 0),
    vec3(0, +1, 0), vec3(0, -1, 0),
    vec3(0, 0, +1), vec3(0, 0, -1)
);

// Mesh/vertex gen

bool verify(ivec3 idx) {
    return 
        (0 <= idx.x) && (idx.x < MAX_INDEX) &&
        (0 <= idx.y) && (idx.y < MAX_INDEX) &&
        (0 <= idx.z) && (idx.z < MAX_INDEX);
}

void generate_face(mat4 pvm, int id, int neighbour_idx) {
    vec3 center = vec3(g_idx[0]);
    for (int i = 0; i < 4; i++) {
        gl_Position = pvm * vec4(center + FACES[4 * neighbour_idx + i], 1);
        EmitVertex();
    }
    EndPrimitive();
}

void maybe_render_face(mat4 pvm, int id, int neighbour_idx) {
    ivec3 idx = g_idx[0] + OFFSETS[neighbour_idx];

    // Always render Chunk edge && skip renderign faces occluded by other blocks
    int neighbor_id = 0;
    if (verify(idx)) {
        neighbor_id = texelFetch(block_id, idx, 0).r;
        if (neighbor_id != 0) return;
    }

    generate_face(pvm, id, neighbour_idx);
}

void main(){
    // Return immediately if this block is invisible (air)
    int id = texelFetch(block_id, g_idx[0], 0).r;
    if (id == 0)
        return;

    mat4 pvm = projectionview * model;

    for (int i = 0; i < 6; i++)
        maybe_render_face(pvm, id, i);
}