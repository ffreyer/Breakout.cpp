:: Note that this expects GLFW to be compiled already

@ECHO OFF
SetLocal EnableDelayedExpansion

ECHO "Building %PWD%"

mkdir bin

:: Compile GLFW
IF NOT EXIST "bin/glfw3.dll" (
    :: compile glfw
    mkdir "dependencies/glfw/build"
    mkdir "dependencies/include"
    mkdir "dependencies/lib"
    "C:\Program Files\CMake\bin\cmake" -S "dependencies/glfw" -B "dependencies/glfw/build" -D BUILD_SHARED_LIBS=ON -D GLFW_BUILD_EXAMPLES=FALSE -D GLFW_BUILD_TESTS=FALSE GLFW_BUILD_DOCS=FALSE
    "C:\Program Files\CMake\bin\cmake" --build "dependencies/glfw/build"
    move /y "%~dp0\dependencies\glfw\build\src\Debug\glfw3.dll" "%~dp0\bin"
    move /y "%~dp0\dependencies\glfw\build\src\Debug\glfw3dll.lib" "%~dp0\dependencies\lib"
    copy /y "%~dp0\dependencies\glfw\include\GLFW\glfw3.h" "%~dp0\dependencies\include"
    copy /y "%~dp0\dependencies\glfw\include\GLFW\glfw3native.h" "%~dp0\dependencies\include"
    rmdir "dependencies/glfw/build" /s /q
)


:: Get a list of all the .c and .cpp files.
cd src
SET cFilenames="dependencies/glad/src/gl.c"
FOR /R %%f in (*.cpp) do (
    SET cFilenames=!cFilenames! "%%f"
)
cd ..

ECHO "Files:" !cFilenames!

SET assembly=main
SET compilerFlags=-g -Wall
:: Werror - this causes errors for included c files
SET includeFlags=-Isrc -Idependencies/entt/src -Idependencies/glad/include -Idependencies/include -Idependencies/glm -Idependencies/stb
SET linkerDirs=-Ldependencies/lib
SET linkerFlags=-lglfw3dll -lopengl32 
REM SET defines=

ECHO "Building $assembly..."
:: TODO: check if this works w/o full path
"C:\\Program Files\\LLVM\\bin\\clang++.exe" %cFilenames%  %compilerFlags% -o bin/%assembly%.exe -std=c++20 %includeFlags% %linkerDirs% %linkerFlags%
REM %defines% 


ECHO "Done."
