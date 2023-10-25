#pragma once

#include <iostream>

#include <glm/glm.hpp>

struct HitResult {
    glm::vec2 position;
    glm::mat2 reflection_matrix;
    float parameter;
    bool hit;

    HitResult() : 
        position(glm::vec2(0.0f, 0.0f)), 
        reflection_matrix(glm::mat2(1.0f)), 
        parameter(0.0f), hit(false) 
    {}

    HitResult(glm::vec2 pos, glm::mat2 m, float t, bool b)
        : position(pos), reflection_matrix(m), parameter(t), hit(b)
    {}

    HitResult(glm::vec2 pos, glm::vec2 n, float t, bool b)
        : position(pos), parameter(t), hit(b)
    {
        set_reflection_matrix(n);
    }

    void set_reflection_matrix(glm::vec2 n) {
        glm::vec2 s = glm::vec2(n.y, -n.x);
        reflection_matrix = glm::mat2();
        reflection_matrix[0][0] = s.x * s.x - n.x * n.x;
        reflection_matrix[1][0] = s.x * s.y - n.x * n.y;
        reflection_matrix[0][1] = s.x * s.y - n.x * n.y;
        reflection_matrix[1][1] = s.y * s.y - n.y * n.y;
    }

    void update(glm::vec2 pos, glm::vec2 n, float t, bool b) {
        position = pos;
        set_reflection_matrix(n);
        parameter = t;
        hit = b;
    }
};

namespace Component {

    class BoundingBox2D {
    private:
        const float epsilon = 0.000001;

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

        glm::vec2 get_center();

        bool collides(const BoundingBox2D& other) const;
        bool collides(glm::vec2 point) const;

        glm::vec2 distance(glm::vec2 point) const;

        HitResult collision_parameter(glm::vec2 point, glm::vec2 dir) const;
        HitResult collision_parameter(glm::vec2 point, float radius, glm::vec2 dir) const;
        HitResult collision_parameter(glm::vec2 point, glm::vec2 radius, glm::vec2 dir) const;

        void print();

    private:
        bool internal_collision(HitResult& result, glm::bvec4 b_lrbt, glm::vec2 origin, glm::vec2 dir) const;
        bool line_collision(HitResult& result, glm::bvec4 b_lrbt, glm::vec2 edge, glm::vec2 origin, glm::vec2 dir) const;
        bool corner_collision(HitResult& result, glm::vec2 corner, glm::vec2 origin, float radius, glm::vec2 dir) const;
    };
    
}