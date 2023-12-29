#version 330 core

flat in vec3 f_normal;
in vec3 f_uvw;

out vec4 FragColor;

uniform isampler3D block_id;


uniform vec3 light_direction;
uniform vec3 light_color;
uniform vec3 ambient_color;

vec3 illuminate(vec3 normal, vec3 color) {
    float diffuse_coefficient = max(0.0, dot(light_direction, -normal));
    return (ambient_color + diffuse_coefficient * light_color) * color;
}

void main()
{
    uint id = uint(texture(block_id, f_uvw).x);
    if (id == uint(0)) {
        discard;
    }
    
    vec3 color = vec3(
        float((id & uint(225)) >> uint(5)) / 5.0,
        float((id & uint(25)) >> uint(3)) / 3.0,
        float((id & uint(7)) >> uint(1)) / 3.0
    );

    // FragColor = vec4(color, 1.0);
    FragColor = vec4(illuminate(f_normal, color), 1.0);
    // FragColor = vec4(0.5 * f_normal + 0.5, 1.0);
}