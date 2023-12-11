#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "camera_core.hpp"


class Camera2D : public Camera {
private:
    float m_left, m_right, m_bottom, m_top;
    glm::vec3 m_position = {0.0f, 0.0f, 0.0f};
    float m_rotation = 0.0f;

public:
    Camera2D(float left, float right, float bottom, float top)
        : m_left(left), m_right(right), m_bottom(bottom), m_top(top)    
    {
        recalculate_projection();
    }

    void set_bounds(float left, float right, float bottom, float top) {
        m_left = left;
        m_right = right;
        m_bottom = bottom;
        m_top = top;
        recalculate_projection();
    }

    void set_position(glm::vec3 position) {
        m_position = position;
        recalculate_view();
    }

    void set_rotation(float rotation) {
        m_rotation = rotation;
        recalculate_view();
    }

    void translate_by(glm::vec3 direction) {
        m_position = m_position + direction;
        recalculate_view();
    }

    void rotate_by(float angle) {
        m_rotation = m_rotation + angle;
        recalculate_view();
    }

// From parent
    void recalculate_view() override {
        // This includes an inverison
        m_view = glm::rotate(glm::mat4(1.0f), glm::radians(-m_rotation), glm::vec3(0, 0, 1)) *
            glm::translate(glm::mat4(1.0f), -m_position);
        m_projectionview = m_projection * m_view;
    }

    void recalculate_projection() override {
        // This includes an inverison
        m_projection = glm::ortho(m_left, m_right, m_bottom, m_top, -1.0f, 1.0f);
        m_projectionview = m_projection * m_view;
    }
};