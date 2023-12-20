#include "Application.hpp"

Application::Application() {};

Application::~Application() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    delete m_window;
}

Window* Application::get_window() const {
    return m_window;
}

bool Application::init(const char* name, int width, int height) {
    m_window = new Window(name, width, height);
    if (m_window->init()) {
        m_window->connect_events(this);

        // imgui
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        
        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForOpenGL(m_window->m_window, true);
        ImGui_ImplOpenGL3_Init();

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
        // handle events
        m_window->poll_events();

        delta_time = (float)(glfwGetTime() - last_time);
        last_time = glfwGetTime();

        // imgui
        {
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            ImGui::Begin("Examples");
            {
                static int tab = 0;
                if (ImGui::Button("Example 1", ImVec2(100, 25)))
                    tab = 0;

                if (ImGui::Button("Example 2", ImVec2(100, 25)))
                    tab = 1;

                if (ImGui::Button("Example 3", ImVec2(100, 25)))
                    tab = 2;
            }
            ImGui::End();
        }

        update(delta_time);

        {
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }

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
            if (action == GLFW_PRESS)
                event.type = EventType::KeyPressed;
            else if (action == GLFW_RELEASE)
                event.type = EventType::KeyReleased;

            win->m_app->dispatch_event(event);
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

            win->m_app->dispatch_event(event);
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

            win->m_app->dispatch_event(event);
        });

        // Scroll
        glfwSetScrollCallback(m_window, [](GLFWwindow* window, double dx, double dy){
            Window* win = (Window*) glfwGetWindowUserPointer(window);

            MouseScrolledEvent event;
            event.type = EventType::MouseScrolled;
            event.scroll = glm::vec2(dx, dy);

            win->m_app->dispatch_event(event);
        });

        glfwSetWindowPosCallback(m_window, [](GLFWwindow* window, int x, int y){
            Window* win = (Window*) glfwGetWindowUserPointer(window);

            WindowMoveEvent event;
            event.type = EventType::WindowMoved;
            event.last_position = win->m_window_position;
            win->m_window_position = glm::ivec2(x, y);
            event.position = win->m_window_position;

            win->m_app->dispatch_event(event);
        });

        glfwSetWindowSizeCallback(m_window, [](GLFWwindow* window, int x, int y){
            Window* win = (Window*) glfwGetWindowUserPointer(window);

            WindowResizeEvent event;
            event.type = EventType::WindowResize;
            event.last_size = win->m_window_size;
            win->m_window_size = glm::ivec2(x, y);
            event.size = win->m_window_size;

            win->m_app->dispatch_event(event);
        });

        glfwSetWindowCloseCallback(m_window, [](GLFWwindow* window){
            Window* win = (Window*) glfwGetWindowUserPointer(window);

            WindowEvent event;
            event.type = EventType::WindowClose;

            win->m_app->dispatch_event(event);
        });

        glfwSetWindowFocusCallback(m_window, [](GLFWwindow* window, int focused){
            Window* win = (Window*) glfwGetWindowUserPointer(window);

            WindowEvent event;
            if (focused == GLFW_TRUE)
                event.type = EventType::WindowFocus;
            else
                event.type = EventType::WindowLostFocus;

            win->m_app->dispatch_event(event);
        });

        return true;
    }
    return false;
}

void Application::dispatch_event(AbstractEvent& event) {
    // TODO: maybe reorganize this away?
    ImGuiIO& io = ImGui::GetIO();
    bool handled = false;
    switch (event.type) {
    case EventType::MouseButtonPressed:
    case EventType::MouseButtonReleased:
    case EventType::MouseMoved:
    case EventType::MouseScrolled:
        handled |= io.WantCaptureMouse;
        break;
    case EventType::KeyPressed:
    case EventType::KeyReleased:
    case EventType::KeyTyped:
        handled |= io.WantCaptureKeyboard;
        break;
    }

    if (!handled)
        on_event(event);
}