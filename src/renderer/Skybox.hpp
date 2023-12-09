#pragma once

#include <array>

#include "opengl/GLTexture.hpp"
#include "opengl/GLTexture.hpp"
#include "opengl/GLVertexArray.hpp"
#include "opengl/GLShader.hpp"

// TODO: rework to use ECS
// TODO: rework to fit a general interface

class SkyBox {
private:
    GLVertexArray va;
    GLShader shader;
    GLCubeMap cubemap;

public:
    SkyBox(std::array<std::string, 6> filepaths);
    void render(glm::mat4& view, glm::mat4& projection);
};