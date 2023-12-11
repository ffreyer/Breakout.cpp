#version 330 core

in vec3 f_normal;
in vec2 f_uv;
in vec4 f_shadow_pos;

out vec4 FragColor;

uniform sampler2D texture1;

vec3 illuminate(vec4 shadow_pos, vec3 normal, vec3 color);

void main()
{
    vec3 color = texture(texture1, f_uv).rgb;
    FragColor = vec4(illuminate(f_shadow_pos, f_normal, color), 1);
}