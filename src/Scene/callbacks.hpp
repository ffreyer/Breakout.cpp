#pragma once

#include <iostream>

#include <entt/entt.hpp>

#include "Components.hpp"
#include "Entity.hpp"

namespace Callback {
    typedef std::function<void(Entity, Entity)> Function;
    
    [[maybe_unused]] static const void do_nothing(Entity e, Entity other) { return; }
    [[maybe_unused]] static const void destroy(Entity e, Entity other) { e.destroy(); }
    [[maybe_unused]] static const void log(Entity e, Entity other) {
        std::cout << "[LOG] Entity " << (uint32_t) e.get_entity();
        if (e.has<Component::Name>())
            std::cout << " " << e.get<Component::Name>();
        std::cout << std::endl;
    }
}