:: Note that this expects GLFW to be compiled already

@ECHO OFF
SetLocal EnableDelayedExpansion

ECHO "Building ${PWD}"

mkdir bin

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
SET includeFlags=-Isrc -Idependencies/entt/src -Idependencies/glad/include -Idependencies/glfw/include -Idependencies/glm -Idependencies/stb
SET linkerDirs=-Ldependencies/glfw/lib
SET linkerFlags=-lglfw3dll -lglew32 -lopengl32
REM SET defines=

ECHO "Building $assembly..."
:: TODO: check if this works w/o full path
"C:\\Program Files\\LLVM\\bin\\clang++.exe" %cFilenames%  %compilerFlags% -o bin/%assembly%.exe -std=c++20 %includeFlags% %linkerDirs% %linkerFlags%
REM %defines% 


ECHO "Done."

:: OK, let's try to actually understand this
:: up to the for loop everything's easy enough
::
:: FOR /R %%f in (*.cpp) do (
::     SET cFilenames=!cFilenames! "%%f"
:: )
::
:: %%f is the variable (full path)
:: %% makes the loop run in file, % in cmd prompt
:: (*.cpp) is the set, regex matching cpp files I suppose
:: /R runs the loop in every recursive directory
:: !cFilenames! expands at run time rather than parse time like %cFilenames% would