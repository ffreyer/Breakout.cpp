#pragma once

#include <iostream>

#include <entt/entt.hpp>

#include "Components.hpp"

namespace Callback {
    typedef std::function<void(entt::registry&, entt::entity)> Function;
    
    
    static const void do_nothing(entt::registry& reg, entt::entity e) { return; }
    static const void destroy(entt::registry& reg, entt::entity e) { reg.destroy(e); }
    static const void log(entt::registry& reg, entt::entity e) {
        std::cout << "[LOG] Entity " << (uint32_t) e;
        if (reg.all_of<Component::Name>(e))
            std::cout << " " << reg.get<Component::Name>(e);
        std::cout << std::endl;
    }
}