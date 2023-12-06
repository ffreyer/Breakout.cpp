#pragma once

#include <array>

#include "opengl/GLTexture.hpp"
#include "opengl/GLTexture.hpp"
#include "opengl/GLVertexArray.hpp"
#include "opengl/GLShader.hpp"

class SkyBox {
private:
    GLVertexArray va;
    GLShader shader;
    GLCubeMap cubemap;

public:
    SkyBox(std::array<std::string, 6> filepaths) {
        // load texture
        int width, height, channels;
        unsigned char* data;
        cubemap.bind();
        cubemap.set_internal_format(GLCubeMap::RGB);
        stbi_set_flip_vertically_on_load(false);
        for (int i = 0; i < 6; i++) {
            data = stbi_load(filepaths[i].c_str(), &width, &height, &channels, 0);
            cubemap.set_data(data, channels_to_gl_type(channels), width, height, GLCubeMap::RIGHT + i);
            stbi_image_free(data);
        }

        // generate vertexarray
        float vertices[] = {
            -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            -1.0f,  1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f,  1.0f
        };
        auto layout = GLBufferLayout({GLBufferElement("Position", GLType::Float3)});
        std::shared_ptr<GLVertexBuffer> buffer = std::make_shared<GLVertexBuffer>(vertices, sizeof(vertices));
        buffer->set_layout(layout);
        va.push(buffer);

        // Generate shader
        shader.add_source("../assets/shaders/skybox.vert");
        shader.add_source("../assets/shaders/skybox.frag");
        shader.compile();
    }

    void render(glm::mat4& view, glm::mat4& projection){
        glDepthFunc(GL_LEQUAL); 
        shader.bind();
        va.bind();
        cubemap.bind();
        shader.set_uniform("view", glm::mat4(glm::mat3(view)));
        shader.set_uniform("projection", projection);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glDepthFunc(GL_LESS); 
    }
};