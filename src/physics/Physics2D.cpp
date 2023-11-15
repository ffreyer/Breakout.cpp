#include "Physics2D.hpp"

void PhysicsEngine2D::delete_entity(entt::entity entity) {
    for (std::vector<HitData>::iterator it = m_hitlist.begin(); it != m_hitlist.end(); it++) {
        HitData& hit = *it;
        if (hit.entity1 == entity || hit.entity2 == entity) {
            m_hitlist.erase(it);
            it--;
        }
    }
}


void PhysicsEngine2D::construct() {
    clear();

    auto outer_view = m_registry->view<Component::Transform, Component::Motion2D, Component::Collision2D>();
    auto inner_view = m_registry->view<Component::Transform, Component::Motion2D, Component::Collision2D>();
    size_t counter = 1;

    // Keep track of earliest collision times for each entity for cleaning up invalid collisions
    for (entt::entity e : outer_view)
        m_earliest[e] = INVALID_TIME;
        
    for (entt::entity moving : outer_view) {
        HitData temp = HitData();

        // Check each moving collider that all_ofn't been in the first loop
        temp.dynamic = true;
        for (auto it = std::next(inner_view.begin(), counter); it != inner_view.end(); it++) {
            collision(temp, moving, *it);
            if (temp.valid && (temp.time < m_earliest[moving])) {
                push_to_hitlist(temp);
                m_earliest[moving] = std::min(m_earliest[moving], temp.time);
                m_earliest[*it]    = std::min(m_earliest[*it],    temp.time);
            }
        }

        // And also every static collider
        auto static_view = m_registry->view<Component::Transform, Component::Collision2D>(entt::exclude<Component::Motion2D>);
        temp.dynamic = false;
        for (auto other : static_view) {
            collision(temp, moving, other);
            if (temp.valid && (temp.time < m_earliest[moving])) {
                push_to_hitlist(temp);
                m_earliest[moving] = std::min(m_earliest[moving], temp.time);
            }
        }

        // Don't repeat checks
        counter++;
    }

    // Cleanup invalid collisions (keep only the earliest valid collision for each entitity)
    for (auto it = m_hitlist.begin(); it != m_hitlist.end(); ) {
        float t = it->time - 0.000001; // offset for float precision just in case
        if (it->dynamic) {
            if ((t <= m_earliest[it->entity1]) && (t <= m_earliest[it->entity2]))
                ++it;
            else
                m_hitlist.erase(it);
        } else {
            if (t <= m_earliest[it->entity1])
                ++it;
            else
                m_hitlist.erase(it);
        }
    }
}

// run simulation for delta_time
void PhysicsEngine2D::process(float delta_time) {

    float local_delta_time = delta_time;
    while (!m_hitlist.empty()) {
        // move newest to back
        std::pop_heap(m_hitlist.begin(), m_hitlist.end(), std::greater<HitData>{});

        // if newest is past delta time revert and exit
        HitData& hit = m_hitlist.back();
        if (hit.time > local_delta_time) {
            std::push_heap(m_hitlist.begin(), m_hitlist.end(), std::greater<HitData>{});
            break;
        }
        // otherwise remove the hit from hitlist
        m_hitlist.pop_back();
        local_delta_time = local_delta_time - hit.time;

        // and process it
        resolve_hit(hit);

        // Calculate next hit for the entites of the resolved hit
        calculate_collision(hit.entity1);
        if (hit.dynamic)
            calculate_collision(hit.entity2);
    }

    // Advance all particles to pos(delta_time)
    auto movables = m_registry->view<Component::Transform, Component::Motion2D>();
    for (entt::entity e : movables) {
        auto [transform, movement] = movables.get(e);
        transform.translate_by(local_delta_time * movement.velocity);
    }

    // Advance time
    for (HitData& hit : m_hitlist)
        hit.time -= local_delta_time; 
}


void PhysicsEngine2D::resolve_hit(HitData& hit) {
    // Update positions
    auto view = m_registry->view<Component::Transform, Component::Motion2D>();
    for (entt::entity e : view) {
        auto [transform, motion] = view.get(e);
        transform.translate_by(hit.time * motion.velocity);
    }

    // Resolve Collision
    if (hit.dynamic) { // both entities move - do kinematics
        Component::Collision2D& collision1 = m_registry->get<Component::Collision2D>(hit.entity1);
        Component::Motion2D&    motion1    = m_registry->get<Component::Motion2D>(hit.entity1);
            
        Component::Collision2D& collision2 = m_registry->get<Component::Collision2D>(hit.entity2);
        Component::Motion2D&    motion2    = m_registry->get<Component::Motion2D>(hit.entity2);

        // calculate new velocities for elastic collision
        float m1 = collision1.mass, m2 = collision2.mass;
        float mi = 1.0 / (m1 + m2);
        glm::vec2 v1 = (m1 - m2) * mi * motion1.velocity + 2 * m2 * mi * motion2.velocity;
        motion2.velocity = 2 * m1 * mi * motion1.velocity + (m2 - m1) * mi * motion2.velocity;
        motion1.velocity = v1;

    } else { // second entity is stationary -> do reflection
        Component::Motion2D& motion1 = m_registry->get<Component::Motion2D>(hit.entity1);
        motion1.velocity = motion1.velocity - 2 * glm::dot(hit.normal, motion1.velocity) * hit.normal; // reflection
    }
    // Note: 
    // Moving past collision should not be necessary because the velocity changes put
    // the collision start at t < 0 and end at t = 0 (+- float precision). So the next
    // start > 0 is not going to be this collision

    // Advance time (by decreasing the time to future events/collisions)
    for (HitData& _hit : m_hitlist)
        _hit.time = _hit.time - hit.time;
    for (auto& [key, time] : m_earliest)
        time = time - hit.time;

    // Mark future collision times of entities involved in this hit as invalid
    m_earliest[hit.entity1] = INVALID_TIME;
    if (hit.dynamic)
        m_earliest[hit.entity2] = INVALID_TIME;

    // Resolve callbacks
    // TODO: should/can we do this earlier and potnetially avoid doing some calculations?
    {
        Entity e1 = Entity(m_registry, hit.entity1);
        Entity e2 = Entity(m_registry, hit.entity2);
        e1.get<Component::Collision2D>().on_collision(e1, e2);
        e2.get<Component::Collision2D>().on_collision(e2, e1);
    }
}


// Update after "entity" collision has been processed
// O(N entities)
void PhysicsEngine2D::calculate_collision(entt::entity entity) {
    // if the entity is not valid (i.e. it has been deleted) we exit immediately
    if (!m_registry->valid(entity))
        return;

    HitData selected = HitData();
    HitData temp = HitData();

    // Check for collisions with dynamic entities.
    // A collision is valid if the other entity does not have a prior collision.
    // It is selected if it is earlier than the current selection
    auto moving_objects = m_registry->view<Component::Transform, Component::Collision2D, Component::Motion2D>();
    temp.dynamic = true;
    for (const entt::entity other : moving_objects) {
        if (other == entity) continue; // skip self-collision
        collision(temp, entity, other);
        if (temp.valid && (temp.time < m_earliest[other]) && (temp < selected))
            selected.copy_from(temp);
    }

    // Check with static entities
    if (m_registry->all_of<Component::Motion2D>(entity)) { // if this is a dynamic entity
        auto static_objects = m_registry->view<Component::Transform, Component::Collision2D>(entt::exclude<Component::Motion2D>);
        temp.dynamic = false;
        for (const entt::entity other : static_objects) {
            collision(temp, entity, other);
            if (temp.valid && (temp < selected))
                selected.copy_from(temp);
        }
    }

    // If we have found a valid collision, add it to the hitlist, update times and
    // cleanup invalidated hits
    if (selected.valid) {
        // Swap entities if given entity is not dynamic (entity1 must always be dynamic)
        if (!m_registry->all_of<Component::Motion2D>(entity)) {
            selected.entity1 = selected.entity2;
            selected.entity2 = entity;
            selected.dynamic = false;
        }

        // add to hitlist, update earliest
        m_hitlist.push_back(selected);
        std::push_heap(m_hitlist.begin(), m_hitlist.end(), std::greater<HitData>{});

        m_earliest[selected.entity1] = selected.time;
        if (selected.dynamic)
            m_earliest[selected.entity2] = selected.time;

        // If there exists a later hit involving the "other" entity it is now invalid and
        // needs to be removed. This may orphan another dynamic entity, which needs to update
        if (selected.dynamic) {
            for (auto it = m_hitlist.begin(); it != m_hitlist.end(); it++) {
                if (it->entity1 == selected.entity2) {
                    m_hitlist.erase(it);
                    if (it->dynamic) {
                        m_earliest[it->entity2] = INVALID_TIME;
                        calculate_collision(it->entity2);
                    }
                } else if (it->entity2 == selected.entity2) {
                    if (it->entity1 == selected.entity1)
                        continue; // it's the hit we just added
                    m_hitlist.erase(it);
                    m_earliest[it->entity1] = INVALID_TIME;
                    calculate_collision(it->entity1);
                }
            }   
        }
    }
}

// TODO: should we remove this?
// Adds a new hit to the hitlist. 
void PhysicsEngine2D::push_to_hitlist(HitData& hit) {
    // Skip if invalid (i.e. no collision occurs)
    if (!hit.valid)
        return;

    // Search for involved object
    for (HitData& previous : m_hitlist) {
        // if the reverse hit exists, skip
        if (hit.entity1 == previous.entity2 && hit.entity2 == previous.entity1)
            return;
    }

    // If it's not a dublicate add it
    m_hitlist.push_back(hit);
    std::push_heap(m_hitlist.begin(), m_hitlist.end(), std::greater<HitData>{});
}


// TODO: acceleration? that would lead to quartic equation?
void PhysicsEngine2D::collision(HitData& hit, entt::entity e1, entt::entity e2) {
    // Fetch Data
    Component::Transform& transform1 = m_registry->get<Component::Transform>(e1);
    Component::Transform& transform2 = m_registry->get<Component::Transform>(e2);
    Component::Collision2D::BoundingShape& bb1 = m_registry->get<Component::Collision2D>(e1).bbox;
    Component::Collision2D::BoundingShape& bb2 = m_registry->get<Component::Collision2D>(e2).bbox;

    glm::vec2 v1 = glm::vec2(0), v2 = glm::vec2(0);
    if (m_registry->all_of<Component::Motion2D>(e1))
        v1 = m_registry->get<Component::Motion2D>(e1).velocity;
    if (m_registry->all_of<Component::Motion2D>(e2))
        v2 = m_registry->get<Component::Motion2D>(e2).velocity;

    // Update hit
    hit.entity1 = e1;
    hit.entity2 = e2;
    hit.valid = false;
    hit.time = INVALID_TIME;

    // dispatch to correct methods
    if (bb1.shape == Component::Collision2D::Rect2D && bb2.shape == Component::Collision2D::Rect2D) {
        auto [p1, wh1] = convert_as_rect(transform1, bb1);
        auto [p2, wh2] = convert_as_rect(transform2, bb2);
        collision(p1, v1, wh1, p2, v2, wh2, hit);
    } else if (bb1.shape == Component::Collision2D::Circle && bb2.shape == Component::Collision2D::Circle) {
        auto [p1, r1] = convert_as_circle(transform1, bb1);
        auto [p2, r2] = convert_as_circle(transform2, bb2);
        collision(p1, v1, r1, p2, v2, r2, hit);
    } else if (bb1.shape == Component::Collision2D::Rect2D && bb2.shape == Component::Collision2D::Circle) {
        auto [p1, wh1] = convert_as_rect(transform1, bb1);
        auto [p2, r2] = convert_as_circle(transform2, bb2);
        collision(p2, v2, r2, p1, v1, wh1, hit);
        hit.normal = -hit.normal;
    } else if (bb1.shape == Component::Collision2D::Circle && bb2.shape == Component::Collision2D::Rect2D){
        auto [p1, r1] = convert_as_circle(transform1, bb1);
        auto [p2, wh2] = convert_as_rect(transform2, bb2);
        collision(p1, v1, r1, p2, v2, wh2, hit);
    }
}

// Circle Circle
void PhysicsEngine2D::collision(glm::vec2 p1, glm::vec2 v1, float r1, glm::vec2 p2, glm::vec2 v2, float r2, HitData& hit) {
    // circle
    // overlapping if: norm((p2 - p1) + t * (v2 - v1)) < r1 + r2 
    // <(p2 - p1) + t * (v2 - v1), (p2 - p1) + t * (v2 - v1)> = (r1 + r2)^2
    // t^2 ||v2-v1||^2 + t * 2<p2-p1, v2-v1> + ||p2-p1||^2 - (r1+r2)^2 = 0
    glm::vec2 dv = v2 - v1;
    glm::vec2 dp = p2 - p1;
    float a = glm::dot(dv, dv);
    float b = 2.0f * dot(dp, dv);
    float c = dot(dp, dp) - (r1 + r2) * (r1 + r2);
    // TODO: check when this becomes negative
    float root = sqrtf(b * b - 4.0f * a * c);
        
    float t1 = (-b - root) / (2.0f * a); // start of collision
    // float t2 = (-b + root) / (2.0f * a); // end of collision

    if (t1 >= 0.0f) {
        hit.time = t1;
        hit.normal = glm::normalize(dp + t1 * dv);
        hit.valid = true;
    }
}

// BBox - BBox
void PhysicsEngine2D::collision(glm::vec2 p1, glm::vec2 v1, glm::vec2 wh1, glm::vec2 p2, glm::vec2 v2, glm::vec2 wh2, HitData& hit) {
    // touching or inside condition:
    // abs(bb2.center - bb1.center) <= bb2.widths + bb1.widths
    // abs(c1 + t * v1 - c2 - t * v2) <= bb2.width + bb1.widths
    // square to resolve abs
    // (c1 - c2 + t (v1 - v2)).^2 <= ((w1 + w2)^2, (h1 + h2)^2)
    // t^2 (v1 - v2).^2 + t 2(c1-c2) .* (v1-v2) + (c1 - c2).^2 - (ws1 + ws2).^2 <= 0
    glm::vec2 dp = p2 - p1;
    glm::vec2 dv = v2 - v1;
    glm::vec2 wh = 0.5f * (wh1 + wh2);

    glm::vec2 a = dv * dv;
    glm::vec2 b = 2.0f* dp * dv;
    glm::vec2 c = dp * dp - wh * wh;
    glm::vec2 root = glm::sqrt(b * b - 4.0f * a * c);
        
    // 1/2 is close/far solution, x/y is x/y
    glm::vec2 t1 = (-b - root) / (2.0f * a);
    glm::vec2 t2 = (-b + root) / (2.0f * a);

    // Events/time order for a valid collision:
    // float first  = std::min(t1.x, t1.y); // collision possible (1 dim entering, other outside)
    // float second = std::max(t1.x, t1.y); // collision start (1 dim inside/touching, other entering)
    // float third  = std::min(t2.x, t2.y); // collision end (1 dim inside/touching, other exiting)
    // float fourth = std::max(t2.x, t2.y); // (1 dim exiting, other outside)

    float stop = std::min(t2.x, t2.y);

    if ((t1.x >= t1.y) && (t1.x < stop) && (t1.x >= 0.0f)) {
        hit.valid = true;
        hit.time = t1.x;
        hit.normal = glm::vec2(glm::sign(dv.x), 0);
    } else if ((t1.y >= t1.x) && (t1.y < stop) && (t1.y >= 0.0f)) {
        hit.valid = true;
        hit.time = t1.y;
        hit.normal = glm::vec2(0, glm::sign(dv.y));
    }
}

// Circle Point (see circle-circle, this is just slightly simplified and specialized for circle-rect collision)
float PhysicsEngine2D::collision(glm::vec2 p1, glm::vec2 v1, float r1, glm::vec2 p2, glm::vec2 v2) {
    glm::vec2 dv = v2 - v1;
    glm::vec2 dp = p2 - p1;
    float a = glm::dot(dv, dv);
    float b = 2.0f * dot(dp, dv);
    float c = dot(dp, dp) - r1 * r1;
    float root = sqrtf(b * b - 4.0f * a * c);
    return (-b - root) / (2.0f * a);
}

// Circle Rect2
void PhysicsEngine2D::collision(glm::vec2 p1, glm::vec2 v1, float r1, glm::vec2 p2, glm::vec2 v2, glm::vec2 wh2, HitData& hit) {
    // When the circle collides with an edge of the Rect2 bbox it acts
    // the same as a 2R x 2R bbox. Resolve collision for this case:
    collision(p1, v1, glm::vec2(2.0f * r1, 2.0f * r1), p2, v2, wh2, hit);

    // When it collides with a corner that corner can be viewed as a
    // point (or R = 0 circle). Resolve those collisions here.
    // They replace the existing hit if they are earlier in time
    for (float w : {-0.5f * wh2.x, 0.5f * wh2.x}) {
        for (float h : {-0.5f * wh2.y, 0.5f * wh2.y}) {
            float t = collision(p1, v1, r1, p2 + glm::vec2(w, h), v2);
            if ((t < hit.time) && (t > 0.0f)) {
                hit.time = t;
                hit.valid = true;
                hit.normal = glm::normalize((p2 + glm::vec2(w, h) - p1) + t * (v2 - v1));
            }
        }
    }
}