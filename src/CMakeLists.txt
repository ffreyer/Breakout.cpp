
# get all *.cpp files recursively
file(GLOB_RECURSE SRC_FILES ${PROJECT_SOURCE_DIR}/src/*.cpp)

include(CMakePrintHelpers)
cmake_print_variables(SRC_FILES)

add_executable(
    ${PROJECT_NAME} 
    ${SRC_FILES}
    ${PROJECT_SOURCE_DIR}/dependencies/glad/src/gl.c
    ${PROJECT_SOURCE_DIR}/dependencies/stb/stb_image.cpp
    ${IMGUI_SOURCES}
)