#include "Events.hpp"


std::ostream& operator<<(std::ostream& stream, EventType type) {
	stream << enum_to_string(type);
    return stream;
}


std::ostream& operator<<(std::ostream& stream, AbstractEvent& event) {
    stream << "AbstractEvent(" << event.type << ")";
    return stream;
}

std::ostream& operator<<(std::ostream& stream, MouseClickEvent& event) {
    stream << "MouseClickEvent(" << event.button << ", " << event.mod << ")";
    return stream;
}

std::ostream& operator<<(std::ostream& stream, MouseMoveEvent& event) {
    stream << "MouseMoveEvent(" << event.last_position << " -> " << event.position << ")";
    return stream;
}

std::ostream& operator<<(std::ostream& stream, MouseScrolledEvent& event) {
    stream << "MouseScrollEvent(" << event.scroll  << ")";
    return stream;
}

std::ostream& operator<<(std::ostream& stream, KeyEvent& event) {
    stream << "KeyEvent(" << event.button << ", " << event.mod << ")";
    return stream;
}

std::ostream& operator<<(std::ostream& stream, WindowEvent& event) {
    stream << "WindowEvent(" << event.type << ")";
    return stream;
}

std::ostream& operator<<(std::ostream& stream, WindowResizeEvent& event) {
    stream << "WindowResizeEvent(" << event.last_size << " -> " << event.size << ")";
    return stream;
}

std::ostream& operator<<(std::ostream& stream, WindowMoveEvent& event) {
    stream << "WindowMoveEvent(" << event.last_position << " -> " << event.position << ")";
    return stream;
}