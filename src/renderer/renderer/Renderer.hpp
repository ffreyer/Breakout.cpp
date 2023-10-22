#pragma once

#include <vector>

#include <entt/entt.hpp>

#include "entity.hpp"

class Renderer {
public:
    entt::registry m_registry;
    std::vector<Entity> m_entities;

    virtual void render() = 0;
};