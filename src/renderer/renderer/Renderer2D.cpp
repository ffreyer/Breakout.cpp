
#include "Renderer2D.hpp"

#include <iostream>

Renderer2D::Renderer2D() :
    m_camera(OrthographicCamera(-1.0f, 1.0f, -1.0f, 1.0f))
{}

void Renderer2D::init() {
    // TODO: make sure these get intialized correctly
    m_camera.recalculate_projection();
    m_camera.recalculate_view();

    // Circle rendering
    m_data.circle_vertex_buffer = std::make_shared<GLVertexBuffer>(
        m_data.max_vertices * sizeof(CircleData), GL_STREAM_DRAW
    );
    m_data.circle_vertex_buffer->set_layout(GLBufferLayout({
        GLBufferElement("Position", GLType::Float3),
        GLBufferElement("Radius", GLType::Float)
    }));

    m_data.circle_vertex_array = std::make_shared<GLVertexArray>();
    m_data.circle_vertex_array->push(m_data.circle_vertex_buffer);

    m_data.circle_shader = std::make_shared<GLShader>();
    m_data.circle_shader->add_source("../assets/shaders/circle.vert");
    m_data.circle_shader->add_source("../assets/shaders/circle.geom");
    m_data.circle_shader->add_source("../assets/shaders/circle.frag");
    m_data.circle_shader->compile();

    m_data.circle_buffer = new CircleData[m_data.max_vertices];

    // TODO: do this with a geometry shader?
    // Quad rendering
    m_data.quad_vertex_buffer = std::make_shared<GLVertexBuffer>(
        m_data.max_vertices * sizeof(QuadVertex), GL_STREAM_DRAW
    );
    m_data.quad_vertex_buffer->set_layout(GLBufferLayout({
        GLBufferElement("Position", GLType::Float3),
    }));

    m_data.quad_vertex_array = std::make_shared<GLVertexArray>();
    m_data.quad_vertex_array->push(m_data.quad_vertex_buffer);
    
    // m_data_quad_shader = GLShader({"../shaders/quad.vert", "../shaders/quad.frag"});
    m_data.quad_shader = std::make_shared<GLShader>();
    m_data.quad_shader->add_source("../assets/shaders/quad.vert");
    m_data.quad_shader->add_source("../assets/shaders/quad.frag");
    m_data.quad_shader->compile();

    m_data.quad_buffer = new QuadVertex[m_data.max_vertices];

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
}

Renderer2D::~Renderer2D() {
    m_entities.clear();    
    free(m_data.circle_buffer);
}

Entity& Renderer2D::create_circle() {
    m_entities.emplace_back(m_registry);
    Entity& entity = m_entities.back();
    
    // TODO: Is there a better solution?
    // I wanna keep position free I think? maybe not?
    entity.add<Component::Circle>(); 
    entity.add<Component::Position>(glm::vec3(0.0f, 0.0f, 0.0f));
    entity.add<Component::Radius>(10.0f);
    // entity.add<Component::CameraData>();

    return entity;
}

Entity& Renderer2D::create_quad() {
    return create_quad(glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec2(1.0f, 1.0f));
}

Entity &Renderer2D::create_quad(glm::vec3 position, glm::vec2 size)
{
    m_entities.emplace_back(m_registry);
    Entity& entity = m_entities.back();
    entity.add<Component::Quad>(position, size); 
    return entity;
}

// TODO: refactor this to work without glad
#include <glad/gl.h>

void Renderer2D::begin() {
    // TODO: do we do time stuff here?
    // TODO: events?
    // TODO: camera recalculation?
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer2D::render() {
    // TODO: some setup?
    auto view = m_registry.view<
        Component::Circle,
        Component::Position,
        Component::Radius
        // Component::CameraData
    >();

    m_data.circle_vertex_array->bind();
    m_data.circle_shader->use();
    m_data.circle_shader->set_uniform("projection", m_camera.m_projection);
    m_data.circle_shader->set_uniform("view", m_camera.m_view);
    m_data.circle_shader->set_uniform("resolution", glm::vec2(800, 600));

    for (auto entity : view) {
        Component::Position& pos = m_registry.get<Component::Position>(entity);
        Component::Radius& radius = m_registry.get<Component::Radius>(entity);

        m_data.circle_buffer[m_data.circle_index] = CircleData(pos, radius);
        m_data.circle_index = m_data.circle_index + 1;

        if (m_data.circle_index == m_data.max_vertices)
            render_circles();
    }
    render_circles();

    // and again for quads
    auto quad_view = m_registry.view<Component::Quad>();

    m_data.quad_vertex_array->bind();
    m_data.quad_shader->use();
    m_data.quad_shader->set_uniform("projection", m_camera.m_projection);
    m_data.quad_shader->set_uniform("view", m_camera.m_view);

    for (auto entity : quad_view) {
        Component::Quad& quad = m_registry.get<Component::Quad>(entity);
        glm::vec3 pos = quad.position - 0.5f * glm::vec3(quad.size, 0.0f);

        if (m_data.quad_index + 6 > m_data.max_vertices)
            render_quads();

        m_data.quad_buffer[m_data.quad_index]   = QuadVertex(pos);
        m_data.quad_buffer[m_data.quad_index+1] = QuadVertex(pos + glm::vec3(quad.size.x, 0.0f, 0.0f));
        m_data.quad_buffer[m_data.quad_index+2] = QuadVertex(pos + glm::vec3(0.0f, quad.size.y, 0.0f));

        m_data.quad_buffer[m_data.quad_index+3] = QuadVertex(pos + glm::vec3(quad.size, 0.0f));
        m_data.quad_buffer[m_data.quad_index+4] = QuadVertex(pos + glm::vec3(quad.size.x, 0.0f, 0.0f));
        m_data.quad_buffer[m_data.quad_index+5] = QuadVertex(pos + glm::vec3(0.0f, quad.size.y, 0.0f));
        
        m_data.quad_index = m_data.quad_index + 6;
    }
    render_quads();
}

void Renderer2D::end() {
    // TODO: swap buffers here?
}


void Renderer2D::render_circles() {
    if (m_data.circle_index == 0)
        return;

    m_data.circle_vertex_buffer->set_data(m_data.circle_buffer, m_data.circle_index * sizeof(CircleData));
    glDrawArrays(GL_POINTS, 0, m_data.circle_index);
    m_data.circle_index = 0;
}

void Renderer2D::render_quads() {
    if (m_data.quad_index == 0)
        return;

    m_data.quad_vertex_buffer->set_data(m_data.quad_buffer, m_data.quad_index * sizeof(QuadVertex));
    glDrawArrays(GL_TRIANGLES, 0, m_data.quad_index);
    m_data.quad_index = 0;
}