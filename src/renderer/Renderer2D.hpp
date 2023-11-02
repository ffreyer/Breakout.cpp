#pragma once

#include <string>

#include <glm/glm.hpp>

#include "camera/OrthographicCamera.hpp"

#include "../opengl/Shader.hpp"
#include "../opengl/VertexArray.hpp"

#define RENDERER2D_MAX_QUADS 20000
#define RENDERER2D_MAX_VERTICES RENDERER2D_MAX_QUADS * 4 
#define RENDERER2D_MAX_INDICES RENDERER2D_MAX_QUADS * 6

struct QuadVertex {
    glm::vec3 position;
    glm::vec4 color;

    QuadVertex() = default;
    QuadVertex(glm::vec3 p, glm::vec4 c) 
        : position(p), color(c) {}
};

struct CircleData {
    glm::vec3 position;
    float radius;
    glm::vec4 color;

    CircleData() = default;
    CircleData(glm::vec3& pos, float r, glm::vec4 c)
        : position(pos), radius(r), color(c) {}
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

class Renderer2D {
private:
    OrthographicCamera m_camera;
    Renderer2DData m_data;
    
public:
    Renderer2D();
    ~Renderer2D();

    void init();

    void begin(glm::vec2 resolution);
    void draw_quad(glm::vec3 position, glm::vec2 size, glm::vec4 color);
    void draw_circle(glm::vec3 position, float radius, glm::vec4 color);
    void end();

private:
    void render_circles();
    void render_quads();
};