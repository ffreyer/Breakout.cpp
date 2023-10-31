#include "Components.hpp"
#include "../core/print.hpp"

std::ostream& operator<<(std::ostream& stream, Component::Name& comp) {
    stream << "Name(" << comp.name << ")";
    return stream;
}

std::ostream& operator<<(std::ostream& stream, Component::Transform& comp) {
    stream << "Transform(position = " << comp.position << ", rotation = " << 
        comp.rotation << ", scale = " << comp.scale << ")";
    return stream;
}


std::ostream& operator<<(std::ostream& stream, Component::Circle comp) {
    stream << "Circle()";
    return stream;
}

std::ostream& operator<<(std::ostream& stream, Component::Quad comp) {
    stream << "Quad()";
    return stream;
}



std::ostream& operator<<(std::ostream& stream, Component::CameraData comp) {
    stream << "CameraData(...)";
    return stream;
}

std::ostream& operator<<(std::ostream& stream, Component::Velocity comp) {
    stream << "Velocity(" << comp.velocity << ")";
    return stream;
}

std::ostream& operator<<(std::ostream& stream, Component::Destructable comp) {
    stream << "Destructable()";
    return stream;
}