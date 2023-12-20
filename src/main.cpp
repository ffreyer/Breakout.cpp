#include "Breakout.hpp"
#include "Example3D.hpp"

int main() {
    Application main;
    main.init("GLPlayground", 800, 600);
    main.push_back<Example3D>();
    main.push_back<Breakout>();
    main.run();

    return 0;
}