#version 330 core

layout (location = 0) in vec3 v_pos;

uniform mat4 model;
uniform mat4 projectionview;
// uniform mat4 projection;
// uniform mat4 view;

void main()
{
    gl_Position = projectionview * model * vec4(v_pos, 1.0);
}

