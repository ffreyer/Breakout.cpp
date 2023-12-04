#version 330 core

in vec3 texture_dir;
out vec4 FragColor;

uniform samplerCube cubemap;

void main()
{
    FragColor = texture(cubemap, texture_dir);
}

