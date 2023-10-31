#pragma once

#include <string>

#include <glm/glm.hpp>

// With
// operator const (type)&() { return fieldname; }
// we can set up automatic conversions of Component structs 
// to the data they contain/represent
// This simplifies usage quite a bit

// Should also define constructors
// empty: Component() = default;
// copy: Component(const Component& ) = default;
// data: Component(... data) ...

namespace Component {

    // Core
    struct Name {
        std::string name;

        Name() = default;
        Name(const Name&) = default;
        Name(std::string& _name)
            : name(_name) {}

        operator const std::string&() { return name; }
    };

    // Geometries
    struct Circle {
        float radius;

        operator const float() { return radius; }
    };

    struct Quad {
        glm::vec2 size;

        operator const glm::vec2() { return size; }
    };

    // Generic

    struct Position {
        glm::vec3 position;

        operator const glm::vec3() { return position; }
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