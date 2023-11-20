This is a little C++/game (engine) dev learning project. 

## Dependencies and Compilation

The project depends on

- glfw via git submodule
- entt via git submodule
- glm via git submodule
- glad explicitly
- stb explicitly

and can be compiled with the build script `build.bat` using clang. This may require adjusting the path to clang++. Note that this script also compiles glfw if necessary.

## What's implemented

- Wrappers for opengl shaders, vertexbuffers and vertexarrays, GLFW windows
- Shaders for circle and rectangle rendering (via geometry shaders)
- A 2D Physics engine for handling collisions exactly (for circles and rectangles)
- A 2D batch renderer
- A 2D Scene class which keeps track of game objects via entt (Entity Component System)
- Simple screen shake
