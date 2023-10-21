#include "Application.hpp"

Application::Application() {};

Application::~Application() {
    delete m_window;
}

Window* Application::get_window() {
    return m_window;
}

bool Application::init(const char* name, int width, int height) {
    m_window = new Window(name, width, height);
    if (m_window->init()) {
        m_window->connect_events(this);
        return true;
    }
        
    // Failed
    std::cout << "Failed to initialize window!" << std::endl;
    delete m_window;
    m_window = nullptr;
    return false;
};

void Application::run() {
    if (!m_window) {
        std::cout << "Window needs to be initialized first!" << std::endl;
        return;
    }

    double last_time = glfwGetTime();
    float delta_time;

    m_running = true;
    while (m_running) {
        // TODO:
        // handle events
        m_window->poll_events();

        delta_time = (float)(glfwGetTime() - last_time);
        last_time = glfwGetTime();

        update(delta_time);

        m_window->swap_buffers();

        if (m_window->should_close())
            m_running = false;
    }
};


bool Window::connect_events(Application* app) {
    m_app = app;
    if (m_window) {
        glfwSetWindowUserPointer(m_window, this);

        // Keys
        glfwSetKeyCallback(m_window, [](GLFWwindow* window, int key, int scancode, int action, int mods){
            Window* win = (Window*) glfwGetWindowUserPointer(window);

            KeyEvent event;
            event.type = EventType::None;
            event.button = key;
            event.mod = mods;
            if (action == GLFW_KEY_DOWN)
                event.type = EventType::KeyPressed;
            else if (action == GLFW_KEY_UP)
                event.type = EventType::KeyReleased;

            win->m_app->on_event(event);
        });

        // Mouse Button
        glfwSetMouseButtonCallback(m_window, [](GLFWwindow* window, int button, int action, int mods){
            Window* win = (Window*) glfwGetWindowUserPointer(window);

            MouseClickEvent event;
            event.type = EventType::None;
            event.button = button;
            event.mod = mods;
            if (action == GLFW_KEY_DOWN)
                event.type = EventType::MouseButtonPressed;
            else if (action == GLFW_KEY_UP)
                event.type = EventType::MouseButtonReleased;

            win->m_app->on_event(event);
        });

        // Mouse Position
        glfwSetCursorPosCallback(m_window, [](GLFWwindow* window, double x, double y){
            Window* win = (Window*) glfwGetWindowUserPointer(window);

            MouseMoveEvent event;
            event.type = EventType::MouseMoved;
            event.last_position = win->m_mouse_position;
            win->m_mouse_position = glm::vec2(x, y);
            event.position = win->m_mouse_position;
            event.delta = event.position - event.last_position;

            win->m_app->on_event(event);
        });

        // Scroll
        glfwSetScrollCallback(m_window, [](GLFWwindow* window, double dx, double dy){
            Window* win = (Window*) glfwGetWindowUserPointer(window);

            MouseScrolledEvent event;
            event.type = EventType::MouseScrolled;
            event.scroll = glm::vec2(dx, dy);

            win->m_app->on_event(event);
        });

        glfwSetWindowPosCallback(m_window, [](GLFWwindow* window, int x, int y){
            Window* win = (Window*) glfwGetWindowUserPointer(window);

            WindowMoveEvent event;
            event.type = EventType::WindowMoved;
            event.last_position = win->m_window_position;
            win->m_window_position = glm::vec2(x, y);
            event.position = win->m_window_position;

            win->m_app->on_event(event);
        });

        glfwSetWindowSizeCallback(m_window, [](GLFWwindow* window, int x, int y){
            Window* win = (Window*) glfwGetWindowUserPointer(window);

            WindowMoveEvent event;
            event.type = EventType::WindowMoved;
            event.last_position = win->m_window_size;
            win->m_window_size = glm::vec2(x, y);
            event.position = win->m_window_size;

            win->m_app->on_event(event);
        });

        glfwSetWindowCloseCallback(m_window, [](GLFWwindow* window){
            Window* win = (Window*) glfwGetWindowUserPointer(window);

            WindowEvent event;
            event.type = EventType::WindowClose;

            win->m_app->on_event(event);
        });

        glfwSetWindowFocusCallback(m_window, [](GLFWwindow* window, int focused){
            Window* win = (Window*) glfwGetWindowUserPointer(window);

            WindowEvent event;
            if (focused == GLFW_TRUE)
                event.type = EventType::WindowFocus;
            else
                event.type = EventType::WindowLostFocus;

            win->m_app->on_event(event);
        });

        return true;
    }
    return false;
}