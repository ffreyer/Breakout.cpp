#pragma once

#include <functional>

#include <glm/glm.hpp> 

#include "EventEnums.hpp"

struct AbstractEvent {
    EventType type;
};

// Mouse

struct MouseClickEvent : AbstractEvent {
    MouseCode button;
    MouseCode mod;
};

struct MouseMoveEvent : AbstractEvent {
    glm::vec2 last_position;
    glm::vec2 position;
    glm::vec2 delta;
};

struct MouseScrolledEvent : AbstractEvent {
    glm::vec2 scroll;
};

// Keyboard

struct KeyEvent : AbstractEvent {
    KeyCode button;
    KeyCode mod;
};

// Window

// WindowClose, WindowFocus, WindowLostFocus
struct WindowEvent : AbstractEvent { };

struct WindowResizeEvent : WindowEvent {
    glm::ivec2 last_size;
    glm::ivec2 size;
};

struct WindowMoveEvent : WindowEvent {
    glm::ivec2 last_position;
    glm::ivec2 position;
};