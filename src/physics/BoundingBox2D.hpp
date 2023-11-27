#pragma once

#include "../Scene/Components.hpp"
#include "../Scene/callbacks.hpp"
#include "../core/print.hpp"

namespace Component {
    struct Boundingbox2D {
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
                    float x;
                    float y;
                    float r;
                    float __;
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
                if (bb.shape == Boundingbox2D::Rect2D)
                    stream << "BoundingBox(" << bb.left << ".." << bb.right << 
                        ", " << bb.bottom << ".." << bb.top << ")";
                else
                    stream << "BoundingCircle(" << bb.origin << ", " << bb.radius << ")";
                return stream;
            }
        };

        BoundingShape bbox;
        Callback::Function2 on_collision = Callback::do_nothing2;

        Boundingbox2D(float l, float r, float b, float t, Callback::Function2 cb = Callback::do_nothing2)
            : bbox(BoundingShape(l, r, b, t)), on_collision(cb) 
        {}
        Boundingbox2D(glm::vec2 p, float r, Callback::Function2 cb = Callback::do_nothing2)
            : bbox(BoundingShape(p, r)), on_collision(cb)
        {}
        Boundingbox2D(uint8_t shape, Callback::Function2 cb = Callback::do_nothing2)
            : bbox(BoundingShape(shape)), on_collision(cb)
        {}

        glm::vec4 get_raw_lrbt() {
            switch (bbox.shape) {
            case Component::Boundingbox2D::Rect2D:
                return glm::vec4(bbox.left, bbox.right, bbox.bottom, bbox.top);
            case Component::Boundingbox2D::Circle:
                return glm::vec4(bbox.x - bbox.r, bbox.x + bbox.r, bbox.y - bbox.r, bbox.y + bbox.r);
            default:
                std::cout << "This should be an error..." << std::endl;
                return glm::vec4(0);
            }
        }
        glm::vec4 get_lrbt(Component::Transform& transform) {
            glm::vec4 lrbt = get_raw_lrbt();
            lrbt.x = transform.scale.x * lrbt.x + transform.position.x;
            lrbt.y = transform.scale.x * lrbt.y + transform.position.x;
            lrbt.z = transform.scale.y * lrbt.z + transform.position.y;
            lrbt.w = transform.scale.y * lrbt.w + transform.position.y;
            return lrbt;
        }
    };
}