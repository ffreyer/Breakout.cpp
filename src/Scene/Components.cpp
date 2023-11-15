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
    stream << "Circle(color = " << comp.color << ")";
    return stream;
}

std::ostream& operator<<(std::ostream& stream, Component::Quad comp) {
    stream << "Quad(color = " << comp.color << ")";
    return stream;
}



std::ostream& operator<<(std::ostream& stream, Component::CameraData comp) {
    stream << "CameraData(...)";
    return stream;
}