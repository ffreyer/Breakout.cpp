#version 330 core

uniform vec3 light_direction;
uniform vec3 light_color;
uniform vec3 ambient_color;

// TODO camdir
// vec3 blinn_phong(vec3 light_direction, vec3 light_color, vec3 normal, vec3 color) {

// }

vec3 illuminate(vec3 normal, vec3 color) {
    float diffuse_coefficient = max(0.0, dot(light_direction, -normal));
    return (ambient_color + diffuse_coefficient * light_color) * color;
}