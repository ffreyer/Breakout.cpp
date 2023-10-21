#pragma once

#include <glm/glm.hpp>

namespace Component {

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

    struct Radius {
        float radius;
    };

    struct Circle {};

    // trying out something else here
    struct Quad {
        glm::vec3 position;
        glm::vec2 size;
    };

}