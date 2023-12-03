#pragma once

#include <string>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>


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

    struct Transform {
        glm::vec3 position = glm::vec3(0.0f);
        glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
        glm::vec3 scale = glm::vec3(1.0f);

        Transform() = default;
        Transform(const Transform&) = default;
        Transform(glm::vec3 p) { position = p; }
        Transform(glm::vec3 p, glm::vec3 s) { position = p; scale = s; }

        glm::mat4 get_matrix() {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::scale(model, scale);
            model = glm::toMat4(rotation) * model;
            model = glm::translate(model, position);
            return model;
        }

        glm::mat3 get_normalmatrix() {
            // TODO: can we skip the mat4?
            glm::mat3 model = glm::mat3(glm::scale(glm::mat4(1.0f), 1.0f / scale));
            model = glm::toMat3(rotation) * model;
            return model;
        }

        void translate_by(const glm::vec3& v) {
            position = position + v;
        }
        void translate_by(const glm::vec2& v) {
            position = position + glm::vec3(v, 0);
        }
        void scale_by(const glm::vec3& v) {
            scale = scale * v;
        }
        void rotate_by(const glm::quat& q) {
            rotation = rotation * q;
        }
        void rotate_by(const glm::vec3& axis, float angle) {
            glm::quat q = glm::angleAxis(angle, axis);
            rotation = rotation * q;
        }

        operator const glm::mat4() { return get_matrix(); }
    };

    // Geometries
    struct Circle {
        glm::vec4 color;
        // maybe texture
        // maybe edge softness?

        Circle() = default;
        Circle(const Circle&) = default;
        Circle(glm::vec3 rgb) : color(glm::vec4(rgb, 1)) {}
        Circle(glm::vec4 rgba) : color(rgba) {}
    };

    struct Quad {
        glm::vec4 color;
        // texture

        Quad() = default;
        Quad(const Quad&) = default;
        Quad(glm::vec3 rgb) : color(glm::vec4(rgb, 1)) {}
        Quad(glm::vec4 rgba) : color(rgba) {}
    };

    // Generic

    struct CameraData {
        glm::mat4 projection;
        glm::mat4 view;
        glm::mat4 projectionview;
        glm::vec2 resolution;
    };

    struct ScheduledDelete {};
}

std::ostream& operator<<(std::ostream& stream, Component::Name& comp);
std::ostream& operator<<(std::ostream& stream, Component::Transform& comp);
std::ostream& operator<<(std::ostream& stream, Component::Circle comp);
std::ostream& operator<<(std::ostream& stream, Component::Quad comp);
std::ostream& operator<<(std::ostream& stream, Component::CameraData comp);