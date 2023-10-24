#include "BoundingBox2D.hpp"

#include <iostream>
  
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

HitResult Component::BoundingBox2D::collision_parameter(glm::vec2 point, float radius, glm::vec2 dir) {
    // TODO: this doesn't work because the radius of our ball is in pixel space
    // TODO: and in world space it's not a circle anymore

    // Circle - bbox intersect
    // - for line checks consider line move by radius outwards
    // - for corners solve (center + t * dir - corner)^2 = radius^2
    // I think that should be all?

    const float epsilon = 0.000001;

    // See distance()
    bool b_left  = left > point.x;
    bool b_right = point.x > right;
    bool b_below = bottom > point.y;
    bool b_above = point.y > top;

    // inside (not left, right, above or below bbox)
    if (!b_left && !b_right && !b_below && !b_above)
        return HitResult(point, glm::vec2(0.0f, 0.0f), 0.0f, true);


    // line check
    // a circle hitting a line is like a point hitting a translated line
    // unless it hits the start or end of it, which is a corner of the bbox
    glm::vec2 edge = glm::vec2(
        b_left * left    + b_right * right,
        b_below * bottom + b_above * top
    );

    // could hit bottom or top
    if ((b_above || b_below) && (std::abs(dir.y) > epsilon)) { 
        // point + t * dir = edge +- radius
        float ty = (edge.y + (b_above - b_below) * radius - point.y) / dir.y;
        float x = point.x + ty * dir.x;
        // std::cout << "(" << edge.y << " + " << (b_above - b_below) * radius << " - " << point.y << 
            // ") / " << dir.y << " = " << ty << "  |  " << left << " < " << x << " < " << right << std::endl;
        if (left < x && x < right)
            return HitResult(glm::vec2(x, edge.y), glm::vec2(0.0, b_above - b_below), ty, true);
    }

    // could hit left or right
    if ((b_left || b_right) && (std::abs(dir.x) > epsilon)) { 
        float tx = (edge.x + (b_right - b_left) * radius - point.x) / dir.x;
        float y = point.y + tx * dir.y;
        if (bottom < y && y < top)
            return HitResult(glm::vec2(edge.x, y), glm::vec2(b_right - b_left, 0.0f), tx, true);
    }

    // if these have produced no results we either hit a corner
    // or miss completely
    // check corner
    glm::vec2& corner = edge;

    // ||point + t * dir - corner||^2 = radius^2
    // || p + t v ||^2 - r^2 = 0      w/ p = (point - corner), v = dir, r = radius
    // = (p.x + t v.x)^2 + (p.y + t v.y)^2 - r^2
    // = p.x^2 + 2 t p.x v.x + t^2 v.x^2 + p.y^2 + 2 t p.y v.y + t^2 v.y^2 - r^2
    // = t^2 (v.x^2 + v.y^2) + t 2(p.x v.x + p.y v.y) + (p.x^2 + p.y^2 - r^2)
    float a = glm::dot(dir, dir);
    glm::vec2 p = point - corner;
    float b = 2 * glm::dot(p, dir);
    float c = glm::dot(p, p) - radius * radius;
    float root_arg = b * b - 4 * a * c;
    if (root_arg >= 0.0f && a > epsilon) {
        float root_val = sqrtf(root_arg);
        float t1 = (-b + root_val) / (2 * a);
        float t2 = (-b - root_val) / (2 * a);
        p = point + t1 * dir;
        // std::cout << t1 << ", " << t2 << std::endl;
        // return HitResult(p, glm::normalize(p - corner), t1, true);
    }

    // Can't/Didn't hit anything
    return HitResult();
}

HitResult Component::BoundingBox2D::collision_parameter(glm::vec2 point, glm::vec2 radius, glm::vec2 dir) {
    // Circle - bbox intersect
    // - for line checks consider line move by radius outwards
    // - for corners solve (center + t * dir - corner)^2 = radius^2
    // I think that should be all?

    const float epsilon = 0.000001;

    // See distance()
    bool b_left  = left > point.x;
    bool b_right = point.x > right;
    bool b_below = bottom > point.y;
    bool b_above = point.y > top;

    // inside (not left, right, above or below bbox)
    if (!b_left && !b_right && !b_below && !b_above)
        return HitResult(point, glm::vec2(0.0f, 0.0f), 0.0f, true);


    // line check
    // a circle hitting a line is like a point hitting a translated line
    // unless it hits the start or end of it, which is a corner of the bbox
    glm::vec2 edge = glm::vec2(
        b_left * left    + b_right * right,
        b_below * bottom + b_above * top
    );

    // could hit bottom or top
    if ((b_above || b_below) && (std::abs(dir.y) > epsilon)) { 
        // point + t * dir = edge +- radius
        float ty = (edge.y + (b_above - b_below) * radius.y - point.y) / dir.y;
        float x = point.x + ty * dir.x;
        // std::cout << "(" << edge.y << " + " << (b_above - b_below) * radius.y << " - " << point.y << 
            // ") / " << dir.y << " = " << ty << "  |  " << left << " < " << x << " < " << right << std::endl;
        if (left < x && x < right)
            return HitResult(glm::vec2(x, edge.y), glm::vec2(0.0, b_above - b_below), ty, true);
    }

    // could hit left or right
    if ((b_left || b_right) && (std::abs(dir.x) > epsilon)) { 
        float tx = (edge.x + (b_right - b_left) * radius.x - point.x) / dir.x;
        float y = point.y + tx * dir.y;
        if (bottom < y && y < top)
            return HitResult(glm::vec2(edge.x, y), glm::vec2(b_right - b_left, 0.0f), tx, true);
    }

    // if these have produced no results we either hit a corner
    // or miss completely
    // check corner
    glm::vec2& corner = edge;

    // ||aspect * (...)|| = (aspect * radius)^2
    // ||point + t * dir - corner||^2 = radius^2
    // || p + t v ||^2 - r^2 = 0      w/ p = (point - corner), v = dir, r = radius
    // = (p.x + t v.x)^2 + (p.y + t v.y)^2 - r^2
    // = p.x^2 + 2 t p.x v.x + t^2 v.x^2 + p.y^2 + 2 t p.y v.y + t^2 v.y^2 - r^2
    // = t^2 (v.x^2 + v.y^2) + t 2(p.x v.x + p.y v.y) + (p.x^2 + p.y^2 - r^2)
    glm::vec2 inv_aspect = radius.y / radius;
    float a = glm::dot(inv_aspect * dir, inv_aspect * dir);
    glm::vec2 p = inv_aspect * (point - corner);
    float b = 2 * glm::dot(p, inv_aspect * dir);
    float c = glm::dot(p, p) - radius.y * radius.y;
    float root_arg = b * b - 4 * a * c;
    if (root_arg >= 0.0f && a > epsilon) {
        float root_val = sqrtf(root_arg);
        float t1 = (-b + root_val) / (2 * a);
        float t2 = (-b - root_val) / (2 * a);
        p = point + t1 * dir;
        // std::cout << t1 << ", " << t2 << std::endl;
        // return HitResult(p, glm::normalize(p - corner), t1, true);
    }

    // Can't/Didn't hit anything
    return HitResult();
}