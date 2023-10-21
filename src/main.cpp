#include <iostream>

#include <glad/gl.h>

#include "opengl/Window.hpp"

int main() {
    std::cout << "Hello World" << std::endl;

    Window window = Window("Test", 800, 600);
    window.init();

    while (!window.should_close()) {
        window.poll_events();
        glClearColor(0.4f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        window.swap_buffers();
    }
}