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

bool Component::BoundingBox2D::collides(BoundingBox2D& other) const {
    return (left < other.right && other.left < right) && 
        (bottom < other.top && other.bottom < top);
}

bool Component::BoundingBox2D::collides(glm::vec2 point) const {
    return (left < point.x && point.x < right) && 
        (bottom < point.y && point.y < top);
}

glm::vec2 Component::BoundingBox2D::distance(glm::vec2 point) const {
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

HitResult Component::BoundingBox2D::collision_parameter(glm::vec2 origin, glm::vec2 dir) const {
    // See distance()
    glm::bvec4 b_lrbt = glm::bvec4(
        origin.x < left,   right < origin.x, 
        origin.y < bottom, top < origin.y
    );

    HitResult result = HitResult();

    // object inside bbox
    if (internal_collision(result, b_lrbt, origin, dir))
        return result;

    // line check
    // a circle hitting a line is like a point hitting a translated line
    // unless it hits the start or end of it, which is a corner of the bbox
    glm::vec2 edge = glm::vec2(
        b_lrbt[0] * left   + b_lrbt[1] * right,
        b_lrbt[2] * bottom + b_lrbt[3] * top
    );

    // if this doesn't update result there is no collision
    line_collision(result, b_lrbt, edge, origin, dir);

    return result;
}

HitResult Component::BoundingBox2D::collision_parameter(glm::vec2 origin, float radius, glm::vec2 dir) const {
    // Circle - bbox intersect
    // - for line checks consider line move by radius outwards
    // - for corners solve (center + t * dir - corner)^2 = radius^2
    // I think that should be all?

    // See distance()
    glm::bvec4 b_lrbt = glm::bvec4(
        origin.x < left,   right < origin.x, 
        origin.y < bottom, top < origin.y
    );

    HitResult result = HitResult();

    // object inside bbox
    if (internal_collision(result, b_lrbt, origin, dir))
        return result;

    // line check
    // a circle hitting a line is like a point hitting a translated line
    // unless it hits the start or end of it, which is a corner of the bbox
    glm::vec2 edge = glm::vec2(
        b_lrbt[0] * left   + b_lrbt[1] * right,
        b_lrbt[2] * bottom + b_lrbt[3] * top
    );

    glm::vec2 offset = glm::vec2((b_lrbt[1] - b_lrbt[0]) * radius, (b_lrbt[3] - b_lrbt[2]) * radius);
    if (line_collision(result, b_lrbt, edge + offset, origin, dir))
        return result;

    // if these have produced no results we either hit a corner
    // or miss completely
    // check corner
    glm::vec2& corner = edge;

    // if the radius has uneven scaling in x and y direction we transform
    // to a space where it's evenly scaled by transforming origin, corner and dir
    if (corner_collision(result, corner, origin, radius, dir)) {
        return result;
    }

    // Can't/Didn't hit anything
    return HitResult();
}

HitResult Component::BoundingBox2D::collision_parameter(glm::vec2 origin, glm::vec2 radius, glm::vec2 dir) const {
    // Circle - bbox intersect
    // - for line checks consider line move by radius outwards
    // - for corners solve (center + t * dir - corner)^2 = radius^2
    // I think that should be all?

    // See distance()
    glm::bvec4 b_lrbt = glm::bvec4(
        origin.x < left,   right < origin.x, 
        origin.y < bottom, top < origin.y
    );

    HitResult result = HitResult();

    // object inside bbox
    if (internal_collision(result, b_lrbt, origin, dir))
        return result;

    // line check
    // a circle hitting a line is like a point hitting a translated line
    // unless it hits the start or end of it, which is a corner of the bbox
    glm::vec2 edge = glm::vec2(
        b_lrbt[0] * left   + b_lrbt[1] * right,
        b_lrbt[2] * bottom + b_lrbt[3] * top
    );

    glm::vec2 offset = glm::vec2((b_lrbt[1] - b_lrbt[0]) * radius.x, (b_lrbt[3] - b_lrbt[2]) * radius.y);
    if (line_collision(result, b_lrbt, edge + offset, origin, dir))
        return result;

    // if these have produced no results we either hit a corner
    // or miss completely
    // check corner
    glm::vec2& corner = edge;

    // if the radius has uneven scaling in x and y direction we transform
    // to a space where it's evenly scaled by transforming origin, corner and dir
    glm::vec2 inv_aspect = radius.y / radius;
    if (corner_collision(result, inv_aspect * corner, inv_aspect * origin, radius.y, inv_aspect * dir)) {
        return result;
    }

    // Can't/Didn't hit anything
    return HitResult();
}

// Internal collision code

bool Component::BoundingBox2D::internal_collision(
        HitResult& result,
        glm::bvec4 b_lrbt, glm::vec2 origin, glm::vec2 dir
    ) const
{
    if (!b_lrbt[0] && !b_lrbt[1] && !b_lrbt[2] && !b_lrbt[3]) {
        result.update(origin, glm::normalize(-dir), 0.0f, true);
        return true;
    }
    return false;
}

bool Component::BoundingBox2D::line_collision(
        HitResult& result,
        glm::bvec4 b_lrbt, glm::vec2 edge, glm::vec2 origin, glm::vec2 dir
    ) const
{
    // could hit bottom or top
    if ((b_lrbt[2] || b_lrbt[3]) && (std::abs(dir.y) > epsilon)) { 
        // point + t * dir = edge +- radius
        float t = (edge.y - origin.y) / dir.y;
        float x = origin.x + t * dir.x;
        if (left < x && x < right) {
            result.update(glm::vec2(x, edge.y), glm::vec2(0.0, b_lrbt[3] - b_lrbt[2]), t, true);
            return true;
        }
    }

    // could hit left or right
    if ((b_lrbt[0] || b_lrbt[1]) && (std::abs(dir.x) > epsilon)) { 
        float t = (edge.x - origin.x) / dir.x;
        float y = origin.y + t * dir.y;
        if (bottom < y && y < top) {
            result.update(glm::vec2(edge.x, y), glm::vec2(b_lrbt[1] - b_lrbt[0], 0.0f), t, true);
            return true;
        }
    }

    return false;
}

bool Component::BoundingBox2D::corner_collision(
        HitResult& result, 
        glm::vec2 corner, glm::vec2 origin, float radius, glm::vec2 dir
    ) const
{
    // ||point + t * dir - corner||^2 = radius^2
    // || p + t v ||^2 - r^2 = 0      w/ p = (point - corner), v = dir, r = radius
    // = (p.x + t v.x)^2 + (p.y + t v.y)^2 - r^2
    // = p.x^2 + 2 t p.x v.x + t^2 v.x^2 + p.y^2 + 2 t p.y v.y + t^2 v.y^2 - r^2
    // = t^2 (v.x^2 + v.y^2) + t 2(p.x v.x + p.y v.y) + (p.x^2 + p.y^2 - r^2)
    glm::vec2 p = origin - corner;

    float a = glm::dot(dir, dir);
    float b = 2 * glm::dot(p, dir);
    float c = glm::dot(p, p) - radius * radius;
    float root_arg = b * b - 4 * a * c;

    if (root_arg >= 0.0f && a > epsilon && b < 0.0f) {
        float root_val = sqrtf(root_arg);
        float t = (-b - root_val) / (2 * a);
        p = origin + t * dir;
        result.update(p, glm::normalize(p - corner), t, true);
        return true;
    }

    return false;
}