#version 330 core

flat in vec3 f_normal;
in vec2 f_uv;
in vec4 f_shadow_pos;

out vec4 FragColor;

uniform sampler2D texture_map;

vec3 illuminate(vec4 shadow_pos, vec3 normal, vec3 color);

void main()
{
    vec3 color = texture(texture_map, f_uv).rgb;
    FragColor = vec4(illuminate(f_shadow_pos, f_normal, color), 1);
}