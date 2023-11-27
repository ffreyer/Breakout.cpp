#pragma once

#include <limits>
#include <vector>
#include <set>
#include <algorithm>
#include <tuple>
#include <functional>

#include <glm/glm.hpp>

#include "../Scene/Components.hpp"
#include "../Scene/callbacks.hpp"
#include "../Scene/Entity.hpp"
#include "../core/print.hpp"

namespace Component {
    // This enables movement simulation
    struct Motion2D {
        glm::vec2 velocity;
    };

    // This enables collision simulation
    // allows circular or rectangular bounding boxes
    struct Collision2D {
        enum {Rect2D, Circle};

        struct BoundingShape {
            uint8_t shape;
            union {
                struct {
                    glm::vec2 origin;
                    float radius;
                    float _;
                };
                struct {
                    float left;
                    float right;
                    float bottom;
                    float top;
                };
            };

            BoundingShape() = delete;
            BoundingShape(uint8_t s) {
                shape = s; 
                if (s == Rect2D) {
                    left = 0.0f; right = 1.0f; bottom = 0.0f; top = 1.0f;
                } else if (s == Circle) {
                    origin = glm::vec2(0.0f, 0.0f); radius = 1.0f;
                } else {
                    throw std::invalid_argument("Shape %s is not a valid Shape.");
                }
            };
            BoundingShape(glm::vec2 o) : shape(Circle), origin(o), radius(1.0f) {};
            BoundingShape(glm::vec2 o, float r) : shape(Circle), origin(o), radius(r) {};
            BoundingShape(float l, float r, float b, float t) : shape(Rect2D), left(l), right(r), bottom(b), top(t) {}

            friend std::ostream& operator<<(std::ostream& stream, BoundingShape& bb) {
                if (bb.shape == Collision2D::Rect2D) {
                    stream << "BoundingBox(" << bb.left << ".." << bb.right << 
                        ", " << bb.bottom << ".." << bb.top << ")";
                } else {
                    stream << "BoundingCircle(" << bb.origin << ", " << bb.radius << ")";

                }
                return stream;
            }
        };

        BoundingShape bbox;
        float mass = 1.0f;
        Callback::Function2 on_collision = Callback::do_nothing2;

        Collision2D(float l, float r, float b, float t, float m = 1.0f, Callback::Function2 cb = Callback::do_nothing2)
            : bbox(BoundingShape(l, r, b, t)), mass(m), on_collision(cb) 
        {}
        Collision2D(glm::vec2 p, float r, float m = 1.0f, Callback::Function2 cb = Callback::do_nothing2)
            : bbox(BoundingShape(p, r)), mass(m), on_collision(cb)
        {}
        Collision2D(uint8_t shape, float m = 1.0f, Callback::Function2 cb = Callback::do_nothing2)
            : bbox(BoundingShape(shape)), mass(m), on_collision(cb)
        {}
        Collision2D(uint8_t shape, Callback::Function2 cb)
            : bbox(BoundingShape(shape)), on_collision(cb)
        {}
    };

}

/*
This engine continuously simulates the world, making it indepedent of how long
a frame actually takes. I.e. even if a frame takes a minute the simulation 
would run normally.

Idea/Setup:
- construct()
    - searches for all (direct) collisions between any dynamic and (dynamic || static) entity
    - it then sorts them by time and removes any collision whose entities are part of another
        collision
    - the remaining time-ordered collisions are saved in `m_hitlist`
    - the time of the earliest collision are saved in `m_earliest` by entity 
- process(delta_time, callbacl)
    - simulates world for `delta_time` seconds
    - processes each collision in hitlist that occurs before delta_time
        - resolve_hit(hit) to advance the world to the collision time
        - callback(entity) to process user action on collision
        - calculate_collision(entity) to search for the next collisions of the colliding entities
- resolve_hit(hit)
    - simulate motion until hit.time
    - update velocity according to collision
    - update time (hits, m_earliest) to be relative to hit.time
    - invalidate collision times of colliding entities in m_earliest
- calculate_collision(entity)
    - search for earliest collision between entity and other that is also earlier than the earliest of other
    - if such a collision exists, add it to hitlist and update m_earliest for the involved entities
    - if a previous collision (other, x) exists and x is dynamic (moving), invalidate m_earliest for x and
        call calculate_collision(x)

*/

class PhysicsEngine2D {

private:
    static constexpr float INVALID_TIME = std::numeric_limits<float>::max();

    struct HitData {
        union {
            struct {
                entt::entity entity1;
                entt::entity entity2;
            };
            entt::entity entities[2];
        };
        float time = INVALID_TIME;            // time of collision relative to start of framse
        glm::vec2 normal = glm::vec2(0.0f);   // surface normal at collision point
        bool valid = false;                   // is there a collision? is it in the future? 
        bool dynamic = true;                  // does it involve two moving particles?

        HitData() : entity1(entt::null), entity2(entt::null) {};
        HitData(entt::entity a, entt::entity b, float p) : entity1(a), entity2(b), time(p), valid(true) {};
        HitData(entt::entity a, entt::entity b, float p, bool v) : entity1(a), entity2(b), time(p), valid(v) {};

        void copy_from(HitData& other) {
            entity1 = other.entity1;
            entity2 = other.entity2;
            time = other.time;
            normal = other.normal;
            valid = other.valid;
            dynamic = other.dynamic;
        }

        // simplify time comparisons
        friend bool operator<(const HitData& left, const HitData& right) {
            if (!left.valid)
                return false;
            return left.time < right.time;
        }

        friend bool operator>(const HitData& left, const HitData& right) {
            if (!left.valid)
                return false;
            return left.time > right.time;
        }
    };

    entt::registry* m_registry;
    std::vector<HitData> m_hitlist;
    std::unordered_map<entt::entity, float> m_earliest;

public:

    PhysicsEngine2D(entt::registry& registry) : m_registry(&registry) {};
    PhysicsEngine2D() : m_registry(nullptr) {};

    void init(entt::registry& registry) {
        m_registry = &registry;
        m_registry->on_destroy<Component::Collision2D>().connect<&PhysicsEngine2D::delete_entity_cb>(this);
    }

    void delete_entity_cb(entt::registry& r, entt::entity e) { return delete_entity(e); };
    void add_entity_cb(entt::registry& r, entt::entity e) { return add_entity(e); }

    void add_entity(entt::entity e) {
        update_entity(e);
    }

    // TODO: should this delete from m_earliest too?
    void delete_entity(entt::entity entity);

    void update_entity(entt::entity entity) {
        delete_entity(entity);
        calculate_collision(entity);
    }

    void clear() {
        m_hitlist.clear();
        m_earliest.clear();
    }

    // construct m_hitlist and m_earliest
    void construct();

    // run simulation for delta_time
    void process(float delta_time);

    void resolve_hit(HitData& hit);

    // keeping this public for testing purposes
    // TODO: Ignoring rotation for now
    // Rect with pos being center, size being full width and height
    static std::tuple<glm::vec2, glm::vec2> convert_as_rect(Component::Transform& transform, Component::Collision2D::BoundingShape& bb) {
        glm::vec2 scale = glm::vec2(transform.scale);
        glm::vec2 lb = scale * glm::vec2(bb.left, bb.bottom);
        glm::vec2 rt = scale * glm::vec2(bb.right, bb.top);
        glm::vec2 pos = glm::vec2(transform.position) + 0.5f * (lb + rt);
        glm::vec2 size = rt - lb;
        return std::tuple(pos, size);
    }
    // Circle with center, radius
    static std::tuple<glm::vec2, float> convert_as_circle(Component::Transform& transform, Component::Collision2D::BoundingShape& bb) {
        glm::vec2 pos = glm::vec2(transform.position) + glm::vec2(transform.scale) * bb.origin;
        float radius = transform.scale.x * bb.radius;
        return std::tuple(pos, radius);
    }

private:

    // Search for earliest collision of "entity" and update collisions invalidated as a result
    void calculate_collision(entt::entity entity);

    // Adds a new hit to the hitlist. 
    void push_to_hitlist(HitData& hit);

    // Calculates and writes collision data to "hit" if a collision between the the given entities exists
    void collision(HitData& hit, entt::entity e1, entt::entity e2);

    // Collision methods for Bounding Shapes:

    // Circle - Circle
    void collision(glm::vec2 p1, glm::vec2 v1, float r1, glm::vec2 p2, glm::vec2 v2, float r2, HitData& hit);
    // BBox - BBox
    void collision(glm::vec2 p1, glm::vec2 v1, glm::vec2 wh1, glm::vec2 p2, glm::vec2 v2, glm::vec2 wh2, HitData& hit);

    // Circle - Point
    float collision(glm::vec2 p1, glm::vec2 v1, float r1, glm::vec2 p2, glm::vec2 v2);

    // Circle - Rect2/Bbox
    void collision(glm::vec2 p1, glm::vec2 v1, float r1, glm::vec2 p2, glm::vec2 v2, glm::vec2 wh2, HitData& hit);
};