#pragma once

#include "camera_core.hpp"

// TODO: keep normalized matrices around
class FirstPersonCamera : public PerspectiveCamera {
private:
    glm::vec3 m_eyeposition;
    glm::vec3 m_viewdirection;
    glm::vec3 m_up;

public:
    FirstPersonCamera() :
        m_eyeposition(glm::vec3(3.0f)),
        m_viewdirection(glm::vec3(-3.0f)),
        m_up(glm::vec3(0.0f, 1.0f, 0.0))
    {
        recalculate_view();
    }

    // generic transformations
    void translate(glm::vec3 v);
    void rotate(float angle, glm::vec3 axis);

    // direct setters
    void eyeposition(glm::vec3 v);
    void lookat(glm::vec3 v);
    void up(glm::vec3 v);
    
    // getters
    glm::vec3 eyeposition() const { return m_eyeposition; }
    glm::vec3 viewdirection() const { return m_viewdirection; }
    glm::vec3 lookat() const { return m_eyeposition + m_viewdirection; }
    glm::vec3 up() const { return m_up; }
    
    // camera motion names
    void pan(float angle); // left - right rotation (shake head)
    void tilt(float angle); // up - down rotation (nod)
    void roll(float angle); // rotate screen (head tilt)

    void dolly(float distance); // forward - backwards translation
    void pedestal(float distance); // up - down translation
    void truck(float distance); // left - right translation

    void recalculate_view() override;
};