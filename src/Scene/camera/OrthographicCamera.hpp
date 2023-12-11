#pragma once

#include "camera_core.hpp"

// TODO: keep normalized matrices around
class OrthographicCamera : public Camera {
private:
    float m_left = -1.0f, m_right = 1.0f, m_bottom = -1.0f, m_top = 1.0f, m_near = 0.01f, m_far = 1.0f;
    glm::vec3 m_eyeposition;
    glm::vec3 m_viewdirection;
    glm::vec3 m_up;

public:
    OrthographicCamera() :
        m_eyeposition(glm::vec3(3.0f)),
        m_viewdirection(glm::vec3(-3.0f)),
        m_up(glm::vec3(0.0f, 1.0f, 0.0))
    {
        recalculate_view();
    }

    // direct setters
    void eyeposition(glm::vec3 v) { m_eyeposition = v; }
    void lookat(glm::vec3 v) { m_viewdirection = v - m_eyeposition; }
    void viewdirection(glm::vec3 v) { m_viewdirection = v; }
    void up(glm::vec3 v) { m_up = v; }
    void lrbt(float l, float r, float b, float t) { m_left = l; m_right = r; m_bottom = b; m_top = t; }
    void lrbt(glm::vec4 v) { m_left = v[0]; m_right = v[1]; m_bottom = v[2]; m_top = v[3]; }
    void near(float v) { m_near = v; }
    void far(float v) { m_far = v; }
    
    // getters
    glm::vec3 eyeposition() const { return m_eyeposition; }
    glm::vec3 viewdirection() const { return m_viewdirection; }
    glm::vec3 lookat() const { return m_eyeposition + m_viewdirection; }
    glm::vec3 up() const { return m_up; }
    glm::vec4 lrbt() { return glm::vec4(m_left, m_right, m_bottom, m_top); }
    float near() { return m_near; }
    float far() {return  m_far; }

    void recalculate_view() override {
        m_view = glm::lookAt(m_eyeposition, m_eyeposition + m_viewdirection, m_up);
        m_projectionview = m_projection * m_view;
    };

    void recalculate_projection() override {
        // This includes an inverison
        m_projection = glm::ortho(m_left, m_right, m_bottom, m_top, m_near, m_far);
        m_projectionview = m_projection * m_view;
    }
};