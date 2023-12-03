#version 330 core

in vec3 f_normal;

out vec4 FragColor;

vec3 illuminate(vec3 normal, vec3 color);

void main()
{
    FragColor = vec4(illuminate(f_normal, 0.5 + 0.5 * f_normal), 1);
}