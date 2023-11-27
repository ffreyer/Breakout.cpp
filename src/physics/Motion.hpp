#pragma once

#include "../Scene/Components.hpp"

namespace Component {
    // This enables movement simulation
    struct Motion {
        glm::vec3 velocity;
        glm::vec3 acceleration;
        
        Motion(glm::vec3 v, glm::vec3 a = glm::vec3(0))
            : velocity(v), acceleration(a)
        {}
        Motion(glm::vec2 v, glm::vec2 a = glm::vec2(0))
            : velocity(glm::vec3(v, 0)), acceleration(glm::vec3(a, 0))
        {}

        glm::vec3 update(float dt) {
            glm::vec3 shift = dt * velocity + 0.5f * dt * dt * acceleration;
            velocity = velocity + dt * acceleration;
            return shift;
        }
    };
}