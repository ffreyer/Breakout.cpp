#include "apps/Breakout.hpp"
#include "apps/Example3D.hpp"
#include "apps/VoxelExample.hpp"

int main() {
    Application main;
    main.init("GLPlayground", 800, 600);
    main.push_back<Example3D>();
    main.push_back<VoxelExample>();
    main.push_back<Breakout>();
    main.run();

    return 0;
}