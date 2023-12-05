#version 330 core

flat in vec3 f_normal;
flat in int f_id;

out vec4 FragColor;

vec3 illuminate(vec3 normal, vec3 color);

void main()
{
    vec3 color = vec3(0.4, 0.1, 0.8);
    FragColor = vec4(illuminate(f_normal, color), 1);
}