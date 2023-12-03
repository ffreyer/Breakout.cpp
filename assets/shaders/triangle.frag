#version 330 core

in vec3 f_normal;
in vec2 f_uv;

out vec4 FragColor;

uniform sampler2D texture0;

vec3 illuminate(vec3 normal, vec3 color);

void main()
{
    vec3 color = texture(texture0, f_uv).rgb;
    FragColor = vec4(illuminate(f_normal, color), 1);
    // FragColor = vec4(illuminate(f_normal, 0.5 + 0.5 * f_normal), 1);
}