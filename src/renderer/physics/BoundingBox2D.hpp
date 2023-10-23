#pragma once

#include <iostream>

#include <glm/glm.hpp>

struct HitResult {
    glm::vec2 position;
    glm::vec2 normal;
    float parameter;
    bool hit;

    HitResult() : 
        position(glm::vec2(0.0f, 0.0f)), 
        normal(glm::vec2(0.0f, 0.0f)), 
        parameter(0.0f), hit(false) 
    {}

    HitResult(glm::vec2 pos, glm::vec2 n, float t, bool b)
        : position(pos), normal(n), parameter(t), hit(b)
    {}
};

namespace Component {

    class BoundingBox2D {
    public:
        float left = 0.0f;
        float right = 0.0f;
        float bottom = 0.0f;
        float top = 0.0f;

        BoundingBox2D() = default;
        BoundingBox2D(float l, float r, float b, float t) : left(l), right(r), bottom(b), top(t) {}
        BoundingBox2D(glm::vec2 position, glm::vec2 size)
            : left(position.x), right(position.x + size.x), bottom(position.y), top(position.y + size.y)
        {
        }

        void translate_by(glm::vec2 v);
        void translate_center_to(glm::vec2 p);
        void translate_lb_to(glm::vec2 p);

        bool collides(BoundingBox2D& other);
        bool collides(glm::vec2 point);

        glm::vec2 distance(glm::vec2 point);

        HitResult collision_parameter(glm::vec2 point, glm::vec2 dir);

        void print();
    };
    
}