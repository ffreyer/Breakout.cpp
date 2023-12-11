#version 330 core

uniform vec3 light_direction;
uniform vec3 light_color;
uniform vec3 ambient_color;

uniform sampler2D shadowmap;

// TODO camdir
// vec3 blinn_phong(vec3 light_direction, vec3 light_color, vec3 normal, vec3 color) {

// }

float shadow_multiplier(vec4 shadow_pos, vec3 normal) {
    const int blur = 2;
    const float divider = 1.0 / ((1 + 2 * blur) * (1 + 2 * blur));

    vec3 shadow_clip = shadow_pos.xyz / shadow_pos.w;
    shadow_clip = 0.5 * shadow_clip + 0.5;
    float bias = max(0.05 * (1.0 - dot(light_direction, normal)), 0.005);
    float shadow = 0.0;
    vec2 texel_size = 1.0 / textureSize(shadowmap, 0);
    for (int x = -blur; x <= blur; x++) {
        for (int y = -blur; y <= blur; y++) {
            float closest_depth = texture(shadowmap, shadow_clip.xy + vec2(x, y) * texel_size).r;
            shadow += shadow_clip.z - bias > closest_depth ? 0.0 : 1.0;
        }
    }
    return shadow * divider;
}

vec3 illuminate(vec4 shadow_pos, vec3 normal, vec3 color) {
    float shadow = shadow_multiplier(shadow_pos, normal);
    float diffuse_coefficient = max(0.0, dot(light_direction, -normal));
    return (ambient_color + shadow * diffuse_coefficient * light_color) * color;
}