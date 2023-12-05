A little C++/game (engine) learning project.


## What's implemented

- Simple Breakout Game
- Wrappers for opengl shaders, vertexbuffers, vertexarrays, textures, GLFW windows
- A 2D Physics engine for handling collisions (between circles and rectangles)
- A 2D batch renderer
- A 2D Scene class which keeps track of game objects via entt (Entity Component System)
- 3D mesh rendering
- basic voxel rendering
- very basic lighting
- skybox

![Screenshot 2023-12-05 163730](https://github.com/ffreyer/GLPlayground.cpp/assets/10947937/b7bc242a-70ad-4dee-9dee-a3f6219b52ea)

## Dependencies and Compilation

The project depends on

- glfw via git submodule
- entt via git submodule
- glm via git submodule
- glad explicitly
- stb explicitly

and can be compiled with the build script `build.bat` using clang. This may require adjusting the path to clang++. Note that this script also compiles glfw if necessary.
