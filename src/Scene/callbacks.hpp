#pragma once

#include <iostream>

#include <entt/entt.hpp>

#include "Components.hpp"
#include "Entity.hpp"

namespace Callback {
    typedef std::function<void(Entity)> Function1;

    [[maybe_unused]] static const void do_nothing(Entity e) { return; }
    [[maybe_unused]] static const void destroy(Entity e) { e.schedule_delete(); }
    [[maybe_unused]] static const void log(Entity e) {
        std::cout << "[LOG] Entity " << (uint32_t) e.get_entity();
        if (e.has<Component::Name>())
            std::cout << " " << e.get<Component::Name>();
        std::cout << std::endl;
    }

    typedef std::function<void(Entity, Entity)> Function2;

    [[maybe_unused]] static const void do_nothing2(Entity e, Entity other) { return; }
    [[maybe_unused]] static const void destroy2(Entity e, Entity other) { e.schedule_delete(); }
    [[maybe_unused]] static const void log2(Entity e, Entity other) { log(e); }
}

namespace Component {
    struct OnUpdate {
        Callback::Function1 callback;
        // void operator() (Entity& e) { callback(e); }
    };
}
