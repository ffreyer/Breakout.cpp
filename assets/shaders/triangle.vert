#version 330 core

layout (location = 0) in vec3 v_pos;
layout (location = 1) in vec3 v_normal;

out vec3 f_normal;

uniform mat4 model;
uniform mat4 projectionview;
// uniform mat4 projection;
// uniform mat4 view;

void main()
{
    gl_Position = projectionview * model * vec4(v_pos, 1.0);
    f_normal = v_normal;
}

