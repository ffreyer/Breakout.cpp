#include "FirstPersonCamera.hpp"

#include <glm/gtc/matrix_transform.hpp>

void FirstPersonCamera::recalculate_projection() {
    m_projection = glm::perspective(glm::radians(m_fov), m_aspect, m_near, m_far);
    m_projectionview = m_projection * m_view;
}

void FirstPersonCamera::recalculate_view() {
    // glm::vec3 ux, uy, uz;

    // m_viewdir = m_lookat - m_eyeposition; // point to lookat, -z dir
    // uz = normalize(m_viewdir);
    // ux = normalize(cross(m_up, uz));
    // uy = normalize(cross(uz, ux));

    m_view = glm::lookAt(m_eyeposition, m_eyeposition + m_viewdirection, m_up);
    m_projectionview = m_projection * m_view;
}

void FirstPersonCamera::translate(glm::vec3 v) {
    glm::mat3 transform = glm::mat3(
        normalize(m_viewdirection), 
        normalize(m_up), 
        normalize(cross(m_viewdirection, m_up))
    ); // ^ camera basis
    m_eyeposition = m_eyeposition + transform * v;
}

void FirstPersonCamera::rotate(float angle, glm::vec3 axis) {
    // TODO: no 3x3 rotate?
    glm::mat4 rot = glm::mat4(1.0f);
    rot = glm::rotate(rot, angle, axis);
    m_viewdirection = glm::mat3(rot) * m_viewdirection;
}
    
// camera motion names
void FirstPersonCamera::pan(float angle) {
    float c = cos(angle);
    float s = sin(angle);
    glm::mat3 rot = glm::mat3(glm::vec3(c, 0, -s), glm::vec3(0, 1, 0), glm::vec3(s, 0, c));
    m_viewdirection = rot * m_viewdirection;
}
void FirstPersonCamera::tilt(float angle) {
    rotate(angle, glm::cross(m_viewdirection, m_up));
}
void FirstPersonCamera::roll(float angle) {
    // TODO: do we allow this?
    rotate(angle, glm::normalize(m_viewdirection));
}

void FirstPersonCamera::dolly(float distance) {
    m_eyeposition = m_eyeposition + normalize(m_viewdirection) * distance;
}
void FirstPersonCamera::pedestal(float distance) {
    m_eyeposition = m_eyeposition + normalize(m_up) * distance;
}
void FirstPersonCamera::truck(float distance) {
    m_eyeposition = m_eyeposition + normalize(cross(m_viewdirection, m_up)) * distance;
}
