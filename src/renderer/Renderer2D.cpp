
#include "Renderer2D.hpp"

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
        GLBufferElement("Radius", GLType::Float),
        GLBufferElement("Color", GLType::Float4)
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
        GLBufferElement("Size", GLType::Float2),
        GLBufferElement("Color", GLType::Float4),
    }));

    m_data.quad_vertex_array = std::make_shared<GLVertexArray>();
    m_data.quad_vertex_array->push(m_data.quad_vertex_buffer);
    
    // m_data_quad_shader = GLShader({"../shaders/quad.vert", "../shaders/quad.frag"});
    m_data.quad_shader = std::make_shared<GLShader>();
    m_data.quad_shader->add_source("../assets/shaders/quad.vert");
    m_data.quad_shader->add_source("../assets/shaders/quad.geom");
    m_data.quad_shader->add_source("../assets/shaders/quad.frag");
    m_data.quad_shader->compile();

    m_data.quad_buffer = new QuadVertex[m_data.max_vertices];

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    // glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
}

Renderer2D::~Renderer2D() {
    delete m_data.circle_buffer;
    delete m_data.quad_buffer;
}

// TODO: refactor this to work without glad
#include <glad/gl.h>

void Renderer2D::begin(glm::vec2 resolution) {
    // This should probably be handled by Scene
    // Fixing shorter dimension here to avoid edges +-1 being outside a standard window
    float aspect = resolution.x / resolution.y;
    m_camera.set_bounds(-aspect, aspect, -1.0f, 1.0f);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer2D::draw_quad(glm::vec3 pos, glm::vec2 size, glm::vec4 color) {
    if (m_data.quad_index == m_data.max_vertices)
        render_quads();

    m_data.quad_buffer[m_data.quad_index] = QuadVertex(pos, size, color);
        
    m_data.quad_index = m_data.quad_index + 1;
}

void Renderer2D::draw_circle(glm::vec3 position, float radius, glm::vec4 color) {
    if (m_data.circle_index == m_data.max_vertices)
        render_circles();

    m_data.circle_buffer[m_data.circle_index] = CircleData(position, radius, color);

    m_data.circle_index = m_data.circle_index + 1;
}

void Renderer2D::end() {
    // TODO: swap buffers here?
    // probably not because we might have multiple renderers in the future?
    
    // flush buffers
    render_quads();
    render_circles();
}

void Renderer2D::render_circles() {
    if (m_data.circle_index == 0)
        return;

    m_data.circle_vertex_array->bind();
    m_data.circle_vertex_buffer->set_data(m_data.circle_buffer, m_data.circle_index * sizeof(CircleData));
    
    m_data.circle_shader->use();
    m_data.circle_shader->set_uniform("projection", m_camera.m_projection);
    m_data.circle_shader->set_uniform("view", m_camera.m_view);
    m_data.circle_shader->set_uniform("resolution", glm::vec2(800, 600)); // TODO:

    glDrawArrays(GL_POINTS, 0, m_data.circle_index);

    m_data.circle_index = 0;
}

void Renderer2D::render_quads() {
    if (m_data.quad_index == 0)
        return;

    m_data.quad_vertex_array->bind();
    m_data.quad_vertex_buffer->set_data(m_data.quad_buffer, m_data.quad_index * sizeof(QuadVertex));

    m_data.quad_shader->use();
    m_data.quad_shader->set_uniform("projection", m_camera.m_projection);
    m_data.quad_shader->set_uniform("view", m_camera.m_view);

    glDrawArrays(GL_POINTS, 0, m_data.quad_index);

    m_data.quad_index = 0;
}