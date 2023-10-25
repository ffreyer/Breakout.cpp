#pragma once

#include <string>

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
    struct Name {
        std::string name;
    };

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

    struct Velocity {
        glm::vec2 velocity;
    };
    
    struct Destructable {
    };
}

std::ostream& operator<<(std::ostream& stream, Component::Circle comp);
std::ostream& operator<<(std::ostream& stream, Component::Quad comp);
std::ostream& operator<<(std::ostream& stream, Component::Name& comp);
std::ostream& operator<<(std::ostream& stream, Component::Position comp);
std::ostream& operator<<(std::ostream& stream, Component::CameraData comp);
std::ostream& operator<<(std::ostream& stream, Component::Color comp);
std::ostream& operator<<(std::ostream& stream, Component::Velocity comp);
std::ostream& operator<<(std::ostream& stream, Component::Destructable comp);