#version 330 core

layout (location = 0) in vec3 pos;

flat out vec3 f_normal;
out vec3 f_uvw;

uniform mat4 projectionview;
uniform vec3 eyeposition;
uniform int dim;
uniform isampler3D block_id;

const vec3 unit_vecs[3] = vec3[]( vec3(1, 0, 0), vec3(0, 1, 0), vec3(0, 0, 1) );
const mat3 rotation_matrix = mat3(
    1, 0, 0,
    0, 0, 1,
    0, -1, 0
);

void main() {
    ivec3 size = textureSize(block_id, 0);
    ivec3 offset = size / 2;
    // int id = gl_InstanceID <= (size[dim] >> 1) ? gl_InstanceID : (size[dim] >> 1) - gl_InstanceID;
    int id = gl_InstanceID <= (size[dim] >> 1) ? -gl_InstanceID : gl_InstanceID - (size[dim] >> 1);
    vec3 displacement = id * unit_vecs[dim];
    vec3 voxel_pos = pos + displacement; 
    vec3 world_pos = 0.1 * voxel_pos;
    gl_Position = projectionview * vec4(world_pos, 1.0f);

    // normal is always +- unit_vecs[dim], pointing towards player
    f_normal = sign((eyeposition - world_pos) * unit_vecs[dim]);

    // texture coordinates should be normalized world pos
    f_uvw = (voxel_pos - 0.5 * f_normal) / size + 0.5;
}

/*
Performance Testing Notes:

1024 x 1024 x 128 voxels at 800x600 pixels with camera at 50, 10, 50
1. rendering back to font: ~780fps
2. alternating front and back starting from center: ~920fps
3. center to front, then center to back: ~990fps
4. center to back, then center to front: ~1020fps (will depend on view direction I'd assume)
5. no shading, using (4): ~1030fps
6. render xz planes first (shallow direction), using (4): ~1520fps

Ideas
- use two-stage rendering process
    - in main pass derive and write:
        - block id or position
        - maybe uv and side within block
        - normal
    - in second pass render everything with sort-of-deferred shading
        - could add in-voxel uv, side for textured voxels
    - position output could allow skipping block_id lookup for a binary air vs block lookup
        - would still need block lookup in second shader though
- split rendering into large chunks 
    - increases number of quads but may reduce overdraw
*/