#pragma once

#include <vector>

#include <entt/entt.hpp>

#include "entity.hpp"

class Renderer {
protected:
    entt::registry m_registry;
    std::vector<Entity> m_entities;

public:
    virtual void render() = 0;
};