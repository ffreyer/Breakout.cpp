#pragma once

#include <glm/glm.hpp>

namespace Component {

    // Geometries
    struct Circle {
        float radius;
    };

    struct Quad {
        glm::vec2 size;
    };

    // Generic
    struct Position {
        glm::vec3 position;
    };

    struct CameraData {
        glm::mat4 projection;
        glm::mat4 view;
        glm::mat4 projectionview;
        glm::vec2 resolution;
    };

    struct Color {
        glm::vec4 color;
    };

}