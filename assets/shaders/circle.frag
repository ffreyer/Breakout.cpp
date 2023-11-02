#version 330 core

in vec2 f_sdf;
flat in float f_radius;
flat in vec4 f_color;

out vec4 FragColor;

void main() {
    float weight = f_radius - sqrt(f_sdf.x * f_sdf.x + f_sdf.y * f_sdf.y);
    weight = smoothstep(-0.9, 0.9, weight);
    FragColor = vec4(f_color.rgb, f_color.a * weight);
    // FragColor = vec4(0.5 + weight, 0, 0, 1);
    // FragColor = vec4(0.9, 0.4, 0.0, 1.0);
}

