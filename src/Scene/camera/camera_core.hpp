#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Perhaps this is too fractured?

class Camera {
public:
    // calculated
    glm::mat4 m_view;
    glm::mat4 m_projection;
    glm::mat4 m_projectionview;

public:
    virtual void recalculate_view() = 0;
    virtual void recalculate_projection() = 0;
    // void recalculate_projectionview() { m_projectionview = m_projection * m_view; }
    // bool recalculate_all() {
    //     if (!recalculate_projection())
    //         return false;
    //     if (!recalculate_view())
    //         return false;
    //     recalculate_projectionview();
    //     return true;
    // }
};

class PerspectiveCamera : public Camera {
public:
    float m_fov;
    float m_near, m_far;
    float m_aspect;

public:
    PerspectiveCamera() :
        m_fov(45.0f), m_near(0.1f), m_far(100.0f), m_aspect(1.33333333f)
    {
        recalculate_projection();
    }

    void recalculate_projection() override {
        m_projection = glm::perspective(glm::radians(m_fov), m_aspect, m_near, m_far);
        m_projectionview = m_projection * m_view;
    }
};
