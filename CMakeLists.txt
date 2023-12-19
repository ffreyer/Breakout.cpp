cmake_minimum_required(VERSION 3.15.7)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(GLPlayground)

# download all submodules
find_package(git QUIET)
if(GIT_FOUND AND EXISTS "${PROJECT_SRC_DIR}/.git")
	# update submodules
	message(STATUS "Submodule update")
	execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		RESULT_VARIABLE GIT_SUBMOD_RESULT
	)
	if(NOT GIUT_SUBMOD_RESULT EQUAL "0")
		message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}.")
	endif()
endif()

# submodules with CMakeLists
add_subdirectory(dependencies/entt)
add_subdirectory(dependencies/glfw)
add_subdirectory(dependencies/glm)

# imgui doesn't have a CMakeList
# maybe treat this with add_library later?

set(IMGUI_SOURCES 
    ${PROJECT_SOURCE_DIR}/dependencies/imgui/imgui_demo.cpp
    ${PROJECT_SOURCE_DIR}/dependencies/imgui/imgui_draw.cpp
    ${PROJECT_SOURCE_DIR}/dependencies/imgui/imgui_tables.cpp
    ${PROJECT_SOURCE_DIR}/dependencies/imgui/imgui_widgets.cpp
    ${PROJECT_SOURCE_DIR}/dependencies/imgui/imgui.cpp
    ${PROJECT_SOURCE_DIR}/dependencies/imgui/backends/imgui_impl_glfw.cpp
    ${PROJECT_SOURCE_DIR}/dependencies/imgui/backends/imgui_impl_opengl3.cpp
    ${PROJECT_SOURCE_DIR}/dependencies/imgui/misc/cpp/imgui_stdlib.cpp
) 

# main build information in here:
add_subdirectory(src)


target_include_directories(
    ${PROJECT_NAME}
    PUBLIC dependencies/entt/src
    PUBLIC dependencies/glfw/include
    PUBLIC dependencies/glad/include
    PUBLIC dependencies/stb
    PUBLIC dependencies/imgui
    PUBLIC src
)

target_link_directories(
    ${PROJECT_NAME}
    PUBLIC dependencies/glfw/src
    PUBLIC dependencies/glm
    PUBLIC dependencies/entt
)

target_link_libraries(
    ${PROJECT_NAME}
    glfw 
    glm::glm 
    EnTT::EnTT
)