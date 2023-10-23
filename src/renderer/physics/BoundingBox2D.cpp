#include "BoundingBox2D.hpp"
  
void Component::BoundingBox2D::translate_by(glm::vec2 v) {
    left = left + v.x;
    right = right + v.x;
    bottom = bottom + v.y;
    top = top + v.y;
}

void Component::BoundingBox2D::translate_center_to(glm::vec2 p) {
    float w = right - left, h = top - bottom;
    left   = p.x - 0.5f * w;
    right  = p.x + 0.5f * w;
    bottom = p.y - 0.5f * h;
    top    = p.y + 0.5f * h;
}
void Component::BoundingBox2D::translate_lb_to(glm::vec2 p) {
    right  = p.x + right - left;
    top    = p.y + top - bottom;
    left   = p.x;
    bottom = p.y;
}

void Component::BoundingBox2D::print() {
    std::cout << "BBox(" << left << " .. " << right << 
        ", " << bottom << " .. " << top << ")" << std::endl;
}

bool Component::BoundingBox2D::collides(BoundingBox2D& other) {
    return (left < other.right && other.left < right) && 
        (bottom < other.top && other.bottom < top);
}

bool Component::BoundingBox2D::collides(glm::vec2 point) {
    return (left < point.x && point.x < right) && 
        (bottom < point.y && point.y < top);
}

glm::vec2 Component::BoundingBox2D::distance(glm::vec2 point) {
    // b_left is 
    //  true |  false     false
    //       .---------.
    //       |         |
    //       '---------'
    // false    false  | true
    // b_right is
    // etc

    bool b_left = left > point.x;
    bool b_right = point.x > right;
    bool b_below = bottom > point.y;
    bool b_above = point.y > top;

    return glm::vec2(
        // left if on the left
        // right if on the right            1 if left or right
        // 0 if inside                      0 if inside
        b_left * left + b_right * right  - (b_left || b_right) * point.x,
        b_below * bottom + b_above * top - (b_below || b_above) * point.y
    );
}

HitResult Component::BoundingBox2D::collision_parameter(glm::vec2 point, glm::vec2 dir) {
    const float epsilon = 0.000001;

    // See distance()
    bool b_left = left > point.x;
    bool b_right = point.x > right;
    bool b_below = bottom > point.y;
    bool b_above = point.y > top;

    // inside (not left, right, above or below bbox)
    if (!b_left && !b_right && !b_below && !b_above)
        return HitResult(point, glm::vec2(0.0f, 0.0f), 0.0f, true);

    glm::vec2 edge = glm::vec2(
        b_left * left    + b_right * right,
        b_below * bottom + b_above * top
    );

    // could hit bottom or top
    if ((b_above || b_below) && (std::abs(dir.y) > epsilon)) { 
        float ty = (edge.y - point.y) / dir.y;
        float x = point.x + ty * dir.x;
        if (left < x && x < right)
            return HitResult(glm::vec2(x, edge.y), glm::vec2(0.0, b_above - b_below), ty, true);
    }

    // could hit left or right
    if ((b_left || b_right) && (std::abs(dir.x) > epsilon)) { 
        float tx = (edge.x - point.x) / dir.x;
        float y = point.y + tx * dir.y;
        if (bottom < y && y < top)
            return HitResult(glm::vec2(edge.x, y), glm::vec2(b_right - b_left, 0.0f), tx, true);
    }

    // Can't hit anything
    return HitResult();
}