#pragma once

#include <glm/glm.hpp>

#include "Renderer.hpp"
#include "components.hpp"
#include "../camera/OrthographicCamera.hpp"

#include "../../opengl/Shader.hpp"
#include "../../opengl/VertexArray.hpp"

#define RENDERER2D_MAX_QUADS 20000
#define RENDERER2D_MAX_VERTICES RENDERER2D_MAX_QUADS * 4 
#define RENDERER2D_MAX_INDICES RENDERER2D_MAX_QUADS * 6

struct QuadVertex {
    glm::vec3 position;

    QuadVertex() = default;
    QuadVertex(glm::vec3 p) { position = p; } 
};

struct CircleData {
    glm::vec3 position;
    float radius;

    CircleData() = default;
    CircleData(Component::Circle& circle, Component::Position& p) {
        position = p.position;
        radius = circle.radius;
    }
};

struct Renderer2DData {
    // constants
    static const unsigned int max_quads = RENDERER2D_MAX_QUADS;
    static const unsigned int max_vertices = RENDERER2D_MAX_VERTICES;
    static const unsigned int max_indices = RENDERER2D_MAX_INDICES;

    // render data
    std::shared_ptr<GLVertexBuffer> quad_vertex_buffer;
    std::shared_ptr<GLVertexArray>  quad_vertex_array;
    std::shared_ptr<GLShader>       quad_shader;

    std::shared_ptr<GLVertexBuffer> circle_vertex_buffer;
    std::shared_ptr<GLVertexArray>  circle_vertex_array;
    std::shared_ptr<GLShader>       circle_shader;

    // object related
    // maybe pointers to last added?
    unsigned int quad_count = 0;
    unsigned int quad_index = 0;
    QuadVertex* quad_buffer = nullptr;
    // QuadVertex quad_buffer[RENDERER2D_MAX_VERTICES]; // No?

    unsigned int circle_count = 0;
    unsigned int circle_index = 0;
    CircleData* circle_buffer = nullptr;
    // CircleData circle_buffer[RENDERER2D_MAX_VERTICES];
};

class Renderer2D : public Renderer {
private:
    OrthographicCamera m_camera;
    Renderer2DData m_data;
    
public:
    Renderer2D();
    ~Renderer2D();

    void init();

    Entity& create_circle();
    Entity& create_quad();
    Entity& create_quad(glm::vec3 position, glm::vec2 size);

    void begin();
    void render();
    void end();

private:
    void render_circles();
    void render_quads();
};