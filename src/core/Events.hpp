#pragma once

#include <functional>
#include <iostream>
#include <utility>

#include <glm/glm.hpp> 
#include <glm/gtx/io.hpp>

#include "EventEnums.hpp"

struct AbstractEvent {
    EventType type;
};

// Mouse

struct MouseClickEvent : AbstractEvent {
    MouseCode button;
    MouseCode mod;
};

struct MouseButtonDownEvent : MouseClickEvent {
    static EventType static_event_type() { return EventType::MouseButtonPressed; } 
};

struct MouseButtonUpEvent : MouseClickEvent {
    static EventType static_event_type() { return EventType::MouseButtonReleased; } 
};

struct MouseMoveEvent : AbstractEvent {
    glm::vec2 last_position;
    glm::vec2 position;
    glm::vec2 delta;

    static EventType static_event_type() { return EventType::MouseMoved; } 
};


struct MouseScrolledEvent : AbstractEvent {
    glm::vec2 scroll;

    static EventType static_event_type() { return EventType::MouseScrolled; } 
};


// Keyboard

struct KeyEvent : AbstractEvent {
    KeyCode button;
    KeyCode mod;
};

struct KeyDownEvent : KeyEvent {
    static EventType static_event_type() { return EventType::KeyPressed; } 
};

struct KeyUpEvent : KeyEvent {
    static EventType static_event_type() { return EventType::KeyReleased; } 
};


// Window

// WindowClose, WindowFocus, WindowLostFocus
struct WindowEvent : AbstractEvent { };

struct WindowResizeEvent : WindowEvent {
    glm::ivec2 last_size;
    glm::ivec2 size;

    static EventType static_event_type() { return EventType::WindowResize; } 
};


struct WindowMoveEvent : WindowEvent {
    glm::ivec2 last_position;
    glm::ivec2 position;

    static EventType static_event_type() { return EventType::WindowMoved; } 
};

// Dispatching to function

// #define BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }
#define BIND_EVENT_FN(fn) [this](auto&& event) { return this->fn(std::forward<decltype(event)>(event)); }

// This needs to be implemented here cause templates
template <typename T, typename F>
void dispatch(const F& callback, AbstractEvent& event) {
    if (T::static_event_type() == event.type)
        callback(static_cast<T&>(event));
}

std::ostream& operator<<(std::ostream& stream, AbstractEvent& event);
std::ostream& operator<<(std::ostream& stream, MouseClickEvent& event);
std::ostream& operator<<(std::ostream& stream, MouseMoveEvent& event);
std::ostream& operator<<(std::ostream& stream, MouseScrolledEvent& event);
std::ostream& operator<<(std::ostream& stream, KeyEvent& event);
std::ostream& operator<<(std::ostream& stream, WindowEvent& event);
std::ostream& operator<<(std::ostream& stream, WindowResizeEvent& event);
std::ostream& operator<<(std::ostream& stream, WindowMoveEvent& event);