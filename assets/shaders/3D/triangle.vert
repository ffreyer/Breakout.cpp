#version 330 core

layout (location = 0) in vec3 v_pos;
layout (location = 1) in vec3 v_normal;
layout (location = 2) in vec2 v_uv;

flat out vec3 f_normal;
out vec2 f_uv;
out vec4 f_shadow_pos;

uniform mat4 model;
uniform mat4 projectionview;
uniform mat3 normalmatrix;
uniform mat4 lightspace;

void main()
{
    vec4 world_pos = model * vec4(v_pos, 1.0);
    gl_Position = projectionview * world_pos;
    f_shadow_pos = lightspace * world_pos;
    f_normal = normalmatrix * v_normal;
    f_uv = v_uv;
}

