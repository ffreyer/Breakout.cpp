#pragma once

#include "Scene/camera/camera_core.hpp"
#include "Scene/Entity.hpp"

class AbstractRenderer {
public:
    AbstractRenderer() = default;
    virtual ~AbstractRenderer() = default;
    virtual void init() = 0;

    virtual void begin(Camera& cam) = 0;
    virtual void render(Entity& e) = 0; // maybe no ref?
    virtual void end() = 0;
};