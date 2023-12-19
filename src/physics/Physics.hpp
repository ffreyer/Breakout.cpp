#pragma once

#include <limits>

#include <entt/entt.hpp>

#include "Motion.hpp"
#include "BoundingBox2D.hpp"

class Physics2D {
private:
    entt::registry* m_registry = nullptr;

public:
    void init(entt::registry& reg) {
        m_registry = &reg;
    }

// Systems

    void resolve_motion(float delta_time) const {
        auto view = m_registry->view<Component::Transform, Component::Motion>();
        for (entt::entity e : view) {
            auto [transform, motion] = view.get(e);
            transform.translate_by(motion.update(delta_time));
        }
    }

    // TODO: Optimize:
    // - double work from not advancing inner loop
    // - better filtering options? e.g. fast skip?
    void resolve_collisions() const {
        auto view = m_registry->view<Component::Motion, Component::Transform, Component::Boundingbox2D>();
        for (entt::entity main : view)
            resolve_collisions(main);
    }

    void resolve_collisions(entt::entity main) const {
        auto [m_transform, m_bbox] = m_registry->get<Component::Transform, Component::Boundingbox2D>(main);
        glm::vec4 m_lrbt = m_bbox.get_lrbt(m_transform);

        auto view = m_registry->view<Component::Transform, Component::Boundingbox2D>();
        for (entt::entity other : view) {
            if (main == other)
                continue;

            auto [o_transform, o_bbox] = view.get(other);
            glm::vec4 o_lrbt = o_bbox.get_lrbt(o_transform);

            if (intersects(m_lrbt, o_lrbt)) {
                m_bbox.on_collision(Entity(m_registry, main), Entity(m_registry, other));
                o_bbox.on_collision(Entity(m_registry, other), Entity(m_registry, main));
            }
        }
    }

    void resolve_collisions(entt::entity main, entt::entity other) const {
        if (main == other)
            return;

        auto [m_transform, m_bbox] = m_registry->get<Component::Transform, Component::Boundingbox2D>(main);
        glm::vec4 m_lrbt = m_bbox.get_lrbt(m_transform);
        
        auto [o_transform, o_bbox] = m_registry->get<Component::Transform, Component::Boundingbox2D>(other);
        glm::vec4 o_lrbt = o_bbox.get_lrbt(o_transform);

        if (intersects(m_lrbt, o_lrbt)) {
            m_bbox.on_collision(Entity(m_registry, main), Entity(m_registry, other));
            o_bbox.on_collision(Entity(m_registry, other), Entity(m_registry, main));
        }
    }

// Resolving Collisions

    static const void resolve_reflection(Entity a, Entity b) {

        auto [a_transform, a_bbox, a_motion] = a.get<Component::Transform, Component::Boundingbox2D, Component::Motion>();
        glm::vec4 a_lrbt = a_bbox.get_lrbt(a_transform);
        
        auto [b_transform, b_bbox] = b.get<Component::Transform, Component::Boundingbox2D>();
        glm::vec4 b_lrbt = b_bbox.get_lrbt(b_transform);

        glm::vec3 vel = a_motion.velocity, accel = a_motion.acceleration;
        // TODO: this isn't meant to be here but it at least gets the collision time right
        if (b.has<Component::Motion>()) {
            auto b_motion = b.get<Component::Motion>();
            vel = vel - b_motion.velocity;
            accel = accel - b_motion.acceleration;
        }

        glm::vec2 x_col = glm::vec2(
            zero_time(a_lrbt.x - b_lrbt.y, vel.x, accel.x),
            zero_time(a_lrbt.y - b_lrbt.x, vel.x, accel.x)
        );

        glm::vec2 y_col = glm::vec2(
            zero_time(a_lrbt.z - b_lrbt.w, vel.y, accel.y),
            zero_time(a_lrbt.w - b_lrbt.z, vel.y, accel.y)
        );

        // we assume there is a collision so the second earliest "inside" time
        // signifies when we have a full collision
        float t;
        glm::vec3 normal;

        {
            float txmin = std::min(x_col.x, x_col.y);
            float tymin = std::min(y_col.x, y_col.y);
            if (txmin < tymin) {
                t = tymin;
                normal = glm::vec3(0.0f, glm::sign(y_col.y - y_col.x), 0.0f);
            } else {
                t = txmin;
                normal = glm::vec3(glm::sign(x_col.y - x_col.x), 0.0f, 0.0f);        
            }
        }

        if (t > 0.0f)
            return;

        // now adjust the velocity and move the entity
        a_transform.translate_by(a_motion.update(t));
        a_motion.velocity = a_motion.velocity - 2 * glm::dot(normal, a_motion.velocity) * normal;
        a_transform.translate_by(a_motion.update(-t));
    }

// Utilities/Internals

    static bool intersects(glm::vec4 a_lrbt, glm::vec4 b_lrbt) {
        bool x_overlap = (a_lrbt.x < b_lrbt.y) && (a_lrbt.y > b_lrbt.x);
        bool y_overlap = (a_lrbt.z < b_lrbt.w) && (a_lrbt.w > b_lrbt.z);
        return x_overlap && y_overlap;
    }

    static float zero_time(float x, float vel, float accel) {
        // Solve x + vel t + 0.5 accel t^2 = 0
        if (accel == 0.0f) {
            return - x / vel;
        } else {
            float a = 0.5f * accel;
            float b = vel;
            float c = x;
            float root = sqrtf(b * b - 4.0f * a * c);
            float t1 = (-b - root) / (2.0f * a);
            float t2 = (-b + root) / (2.0f * a);
            return abs(t1) < abs(t2) ? t1 : t2;
        }
    }
};