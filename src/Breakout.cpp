#include "Breakout.hpp"

float randf() { return (float) std::rand() / (float) RAND_MAX; }

Breakout::Breakout() {
    if (init("Test Window", 800, 600)) {
        std::srand(glfwGetTime());
        m_scene.init();
        m_physics.init(m_scene.get_registry());
        reset();
    }
}

void Breakout::update(float delta_time) {
    if (m_paused)
        return;

    // Simulate physics world
    m_physics.resolve_motion(delta_time);
    m_physics.resolve_collisions();
    m_scene.update(delta_time);

    // Check gameover
    if (m_ball_count < 1) {
        std::cout << "Gameover" << std::endl;
        m_paused = true;
    }

    // Render
    m_scene.render(m_window->get_window_size());
}

void Breakout::on_event(AbstractEvent& event) {
    dispatch<MouseMoveEvent>(BIND_EVENT_FN(update_paddle_position), event);
    dispatch<WindowResizeEvent>(BIND_EVENT_FN(update_paddle_position), event);

    // Update Paddle position
    if (event.type == EventType::KeyReleased) {
        KeyEvent& ke = static_cast<KeyEvent&>(event);
        if (ke.button == Key::R)
            reset();
        m_paused = false;

        if (ke.button == Key::Escape)
            m_running = false;

        return;
    }
}

void Breakout::create_ball(glm::vec2 pos, glm::vec2 vel) {
    Entity ball = m_scene.create_circle("Ball", glm::vec3(pos, 0), 0.02f);
    ball.add<Component::Boundingbox2D>(glm::vec2(0.0f), 1.0f, (Callback::Function2) Physics2D::resolve_reflection);
    ball.add<Component::Motion>(vel);
    ball.add<Component::OnUpdate>([this](Entity e){ 
        auto pos = e.get<Component::Transform>().position;
        if ((abs(pos.x) > 1.1) || (abs(pos.y) > 1.1)) {
            e.schedule_delete();
            this->m_ball_count--;
        }
    });
    m_ball_count++;
    ball.add<Component::PlayBall>();
}

void Breakout::maybe_spawn_powerup() {
    const float drop_chance = 0.2;

    if (randf() < drop_chance) {
        float x = 2.0f * randf() - 1.0f;

        std::cout << "Created powerup at " << x << std::endl;

        Entity powerup = m_scene.create_circle("Powerup", glm::vec3(x, 0.5, 0), 0.02f, glm::vec4(0.1, 0.6, 0.2, 1.0));
        powerup.add<Component::Boundingbox2D>(glm::vec2(0.0f), 1.0f, [this](Entity powerup, Entity other){
            if (other.has<Component::Paddle>()) {
                powerup.schedule_delete();

                auto& reg = this->m_scene.get_registry();
                auto view = reg.view<Component::PlayBall>();

                for (entt::entity e : view) {
                    auto motion = reg.get<Component::Motion>(e);
                    glm::vec3 v = motion.velocity;
                    glm::vec3 perp = glm::vec3(v.y, -v.x, v.z);
                    motion.velocity = glm::length(v) * glm::normalize(v - 0.2f * perp);
                    auto transform = reg.get<Component::Transform>(e);
                    create_ball(transform.position - 0.1f * v, glm::length(v) * glm::normalize(v + 0.2f * perp));
                }
            }
        });

        powerup.add<Component::Motion>(glm::vec2(0, -0.5));
        powerup.add<Component::PowerUp>();
        powerup.add<Component::OnUpdate>([](Entity e){ 
            auto pos = e.get<Component::Transform>().position;
            if ((abs(pos.x) > 1.1f) || (abs(pos.y) > 1.1f)) 
                e.schedule_delete();
        });
    }
}

void Breakout::reset() {
    m_score = 0;
    m_scene.clear();

    // Add Ball
    create_ball();

    // Add Bricks
    int columns = 10, rows = 6;
    float aspect = 600.0f / 800.0f;
    glm::vec2 brick_scale = glm::vec2(0.19, 0.04);

    Callback::Function2 on_brick_collision = [this](Entity brick, Entity other){
        if (other.has<Component::PlayBall>()) {
            brick.schedule_delete();
            this->maybe_spawn_powerup();
            this->screen_shake();
        }
    };

    for (int i = 0; i < columns; i++) {
        float x = (brick_scale.x + 0.01f) * i - 0.995f;
        for (int j = 0; j < rows; j++) {
            // aspect rescaling only works on init
            float y =  1.0f - (brick_scale.y + 0.01f / aspect) * (j + 1.0f); 
            char name[16];
            sprintf_s(name, 16, "Brick[%i, %i]", i, j);
            Entity e = m_scene.create_quad(name, glm::vec3(x, y, 0.0f), brick_scale);
            e.add<Component::Boundingbox2D>(Component::Boundingbox2D::Rect2D, on_brick_collision);
        }
    }

    // Add paddle
    m_paddle = m_scene.create_quad("Paddle", glm::vec3(0.0f, -0.96f, 0.0f), brick_scale);
    m_paddle.add<Component::Boundingbox2D>(Component::Boundingbox2D::Rect2D);
    m_paddle.add<Component::Paddle>();

    // Add wall colliders
    Entity wall_l = m_scene.create_entity("Wall left");
    wall_l.add<Component::Boundingbox2D>(Component::Boundingbox2D::Rect2D);
    wall_l.add<Component::Transform>(glm::vec3(-2.0f, -2.0f, 0.0f), glm::vec3(1.0f, 4.0f, 1.0f));
    wall_l.add<Component::Quad>(glm::vec3(0, 0, 0));
        
    Entity wall_r = m_scene.create_entity("Wall right");
    wall_r.add<Component::Boundingbox2D>(Component::Boundingbox2D::Rect2D);
    wall_r.add<Component::Transform>(glm::vec3(1.0f, -2.0f, 0.0f), glm::vec3(1.0f, 4.0f, 1.0f));
    wall_r.add<Component::Quad>(glm::vec3(0, 0, 0));

    Entity wall_top = m_scene.create_entity("Wall top");
    wall_top.add<Component::Boundingbox2D>(Component::Boundingbox2D::Rect2D);
    wall_top.add<Component::Transform>(glm::vec3(-2.0f, 1.0f, 0.0f), glm::vec3(4.0f, 1.0f, 1.0f));
    wall_top.add<Component::Quad>(glm::vec3(0, 0, 0));
}

void Breakout::update_paddle_position() {
    Component::Transform& transform = m_paddle.get<Component::Transform>();

    float x = get_mouse_position().x;
    float w = (float) get_window_size().x;
    float aspect = (float) w / get_window_size().y;
    float sx = 0.5f * transform.scale.x;
    float cam_x = (x / w  * 2.0f - 1.0f);
    if (aspect > 1)
        cam_x = aspect * cam_x;

    transform.position.x = glm::clamp(cam_x, sx - 1.0f, 1.0f - sx) - sx;
}