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
        m_window->set_vsync(false);

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

        // Setup Metrics
        m_stats.push_back(OnlineStatistics());
        m_stats.push_back(OnlineStatistics());
        m_stats.push_back(OnlineStatistics());
        m_stats.push_back(OnlineStatistics());

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
    double frame_time, temp_time, imgui_delta_time;
    float delta_time;
    char buffer[128];

    m_running = true;
    while (m_running) {
        frame_time = glfwGetTime();
        
        temp_time = glfwGetTime();
        // handle events
        m_window->poll_events();
        // polling time stats
        m_stats[1].push(glfwGetTime() - temp_time);

        delta_time = (float)(glfwGetTime() - last_time);
        last_time = glfwGetTime();

        // imgui
        {
            temp_time = glfwGetTime();
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            ImGui::Begin("Examples");
            
            for (uint32_t i = 0; i < m_apps.size(); i++) {
                SubApp* app = m_apps[i].get();
                if (ImGui::Button(app->m_name.c_str(), ImVec2(100, 25))) {
                    m_current_app = i;
                    app->m_running = true;
                } else {
                    app->m_running = false;
                }
            }
            ImGui::End();

            ImGui::Begin("Frame Statistics");
            sprintf_s(buffer, "Frame: %0.1f fps", 1.0f / m_stats[0].mean());
            ImGui::Text(buffer);
            sprintf_s(buffer, "Frame: %0.3fms", 1000.0f * m_stats[0].mean());
            ImGui::Text(buffer);
            sprintf_s(buffer, "Poll:  %0.3fms", 1000.0f * m_stats[1].mean());
            ImGui::Text(buffer);
            sprintf_s(buffer, "Main:  %0.3fms", 1000.0f * m_stats[2].mean());
            ImGui::Text(buffer);
            sprintf_s(buffer, "ImGui: %0.3fms", 1000.0f * m_stats[3].mean());
            ImGui::Text(buffer);
            ImGui::End();

            imgui_delta_time = glfwGetTime() - temp_time;
        }

        temp_time = glfwGetTime();
        if (m_current_app < m_apps.size())
            m_apps[m_current_app]->update(delta_time);
        m_stats[2].push(glfwGetTime() - temp_time);

        {
            temp_time = glfwGetTime();
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            m_stats[3].push(glfwGetTime() - temp_time + imgui_delta_time);
        }

        m_window->swap_buffers();

        if (m_window->should_close()) {
            m_running = false;
            for (uint32_t j = 0; j < m_apps.size(); j++)
                m_apps[j]->m_running = false;
        }
        m_stats[0].push(glfwGetTime() - frame_time);
    }
};

void Application::on_event(AbstractEvent& event) {
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

    if (!handled && (m_apps.size() > m_current_app))
        m_apps[m_current_app]->on_event(event);
}

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
            win->m_window_position = glm::ivec2(x, y);
            event.position = win->m_window_position;

            win->m_app->on_event(event);
        });

        glfwSetWindowSizeCallback(m_window, [](GLFWwindow* window, int x, int y){
            Window* win = (Window*) glfwGetWindowUserPointer(window);

            WindowResizeEvent event;
            event.type = EventType::WindowResize;
            event.last_size = win->m_window_size;
            win->m_window_size = glm::ivec2(x, y);
            event.size = win->m_window_size;

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
