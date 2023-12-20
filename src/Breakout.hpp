#pragma once

#include <iostream>
#include <vector>
#include <cstdlib>

#include <glm/glm.hpp>
#include <glm/gtx/io.hpp>

#include "Scene/Scene2D.hpp"
#include "physics/Physics.hpp"
#include "core/Application.hpp"
#include "core/logging.hpp"

namespace Component {
    // TODO probably squash these into a tag component and just if-else?
    struct PlayBall {};
    struct PowerUp {};
    struct Brick {};
    struct Paddle {};
}

class Breakout : public SubApp {
private:
    Scene2D m_scene;
    Physics2D m_physics;
    uint16_t m_score = 0;
    bool m_paused = false;

public:
    Entity m_paddle;
    int m_ball_count = 0;

    Breakout(Window* window);

    void update(float delta_time) override;
    void on_event(AbstractEvent& event) override;

    void create_ball(glm::vec2 pos = glm::vec2(0), glm::vec2 vel = glm::vec2(0.1f, 1.0f));
    void screen_shake() { m_scene.screen_shake(); }
    void maybe_spawn_powerup();
    void reset();

private:
    void update_paddle_position(MouseMoveEvent& e) { return update_paddle_position(); }
    void update_paddle_position(WindowResizeEvent& e) { return update_paddle_position(); }
    void update_paddle_position();
};