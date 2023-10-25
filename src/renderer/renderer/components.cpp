#include "components.hpp"
#include "../../core/print.hpp"

std::ostream& operator<<(std::ostream& stream, Component::Circle comp) {
    stream << "Circle(radius = " << comp.radius << ")";
    return stream;
}

std::ostream& operator<<(std::ostream& stream, Component::Quad comp) {
    stream << "Quad(size = " << comp.size << ")";
    return stream;
}

std::ostream& operator<<(std::ostream& stream, Component::Name& comp) {
    stream << "Name(" << comp.name << ")";
    return stream;
}

std::ostream& operator<<(std::ostream& stream, Component::Position comp) {
    stream << "Position(" << comp.position << ")";
    return stream;
}

std::ostream& operator<<(std::ostream& stream, Component::CameraData comp) {
    stream << "CameraData(...)";
    return stream;
}

std::ostream& operator<<(std::ostream& stream, Component::Color comp) {
    stream << "Color(" << comp.color << ")";
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