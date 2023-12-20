:: Note that this expects GLFW to be compiled already

@ECHO OFF
SetLocal EnableDelayedExpansion

ECHO "Building %PWD%"

mkdir bin

IF NOT EXIST "bin/glfw3.dll" GOTO :COMPILE_GLFW
IF NOT EXIST "dependencies/glfw/include/GLFW/glfw3.h" GOTO :COMPILE_GLFW
IF NOT EXIST "dependencies/glfw/include/GLFW/glfw3native.h" GOTO :COMPILE_GLFW
IF NOT EXIST "dependencies/glfw/build/src/Debug/glfw3dll.lib" GOTO :COMPILE_GLFW

GOTO GLFW_VALID

:COMPILE_GLFW
ECHO "Compiling GLFW"
    :: Compile GLFW
    mkdir "dependencies/glfw/build"
    "C:\Program Files\CMake\bin\cmake" -S "dependencies/glfw" -B "dependencies/glfw/build" -D BUILD_SHARED_LIBS=ON -D GLFW_BUILD_EXAMPLES=FALSE -D GLFW_BUILD_TESTS=FALSE GLFW_BUILD_DOCS=FALSE
    "C:\Program Files\CMake\bin\cmake" --build "dependencies/glfw/build"

:GLFW_VALID


:: Get a list of all the .c and .cpp files.
SET cFilenames="dependencies/glad/src/gl.c"
cd src
FOR /R %%f in (*.cpp) do (
    SET cFilenames=!cFilenames! "%%f"
)

:: Imgui
cd ../dependencies/imgui
FOR %%f in (*.cpp) do (
    SET cFilenames=!cFilenames! "%%~ff"
)
cd ../..
SET cFilenames=!cFilenames! "dependencies/imgui/backends/imgui_impl_glfw.cpp"
SET cFilenames=!cFilenames! "dependencies/imgui/backends/imgui_impl_opengl3.cpp"
SET cFilenames=!cFilenames! "dependencies/imgui/misc/cpp/imgui_stdlib.cpp"
SET cFilenames=!cFilenames! "dependencies/stb/stb_image.cpp"

ECHO "Files:" !cFilenames!

SET assembly=main
SET compilerFlags=-g -Wall
:: Werror - this causes errors for included c files
SET includeFlags=-Isrc -Idependencies/entt/src -Idependencies/glad/include -Idependencies/glfw/include -Idependencies/glm -Idependencies/imgui -Idependencies/stb
SET linkerDirs=-Ldependencies/glfw/build/src/Debug
SET linkerFlags=-lglfw3dll -lopengl32 
REM SET defines=

ECHO "Building $assembly..."
:: TODO: check if this works w/o full path
"C:\\Program Files\\LLVM\\bin\\clang++.exe" %cFilenames%  %compilerFlags% -o bin/%assembly%.exe -std=c++20 %includeFlags% %linkerDirs% %linkerFlags%
REM %defines% 


ECHO "Done."
