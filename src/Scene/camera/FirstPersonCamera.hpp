#pragma once

#include "camera_core.hpp"

// TODO: keep normalized matrices around
class FirstPersonCamera : public PerspectiveCamera {
public:
    glm::vec3 m_eyeposition;
    glm::vec3 m_viewdirection;
    glm::vec3 m_up;

    FirstPersonCamera() :
        m_eyeposition(glm::vec3(0.0f, 1.7f, 0.0f)),
        m_viewdirection(glm::vec3(0.0f, 0.0f, -1.0f)),
        m_up(glm::vec3(0.0f, 1.0f, 0.0))
    {
    }

    // generic transformations
    void translate(glm::vec3 v);
    void rotate(float angle, glm::vec3 axis);
    
    // camera motion names
    void pan(float angle); // left - right rotation (shake head)
    void tilt(float angle); // up - down rotation (nod)
    void roll(float angle); // rotate screen (head tilt)

    void dolly(float distance); // forward - backwards translation
    void pedestal(float distance); // up - down translation
    void truck(float distance); // left - right translation

    void recalculate_view();
    void recalculate_projection();
};