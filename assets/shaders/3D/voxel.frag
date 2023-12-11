#version 330 core

flat in vec3 f_normal;
in vec2 f_uv;

out vec4 FragColor;

uniform sampler2D texture_map;

vec3 illuminate(vec3 normal, vec3 color);

void main()
{
    vec3 color = texture(texture_map, f_uv).rgb;
    FragColor = vec4(illuminate(f_normal, color), 1);
}