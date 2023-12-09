#include "MeshRenderer.hpp"

namespace Component {
    // TODO: check if move is the correct tool here
    SimpleMesh::SimpleMesh(std::vector<uint32_t>& idxs, std::vector<float>& data, GLBufferLayout& layout)
        : indices(idxs), vertices(data)
    {
        std::shared_ptr<GLIndexBuffer> ibuffer = std::make_shared<GLIndexBuffer>(indices);
        std::shared_ptr<GLVertexBuffer> vbuffer = std::make_shared<GLVertexBuffer>(vertices);
        vbuffer->set_layout(layout);
        va.set(ibuffer);
        va.push(vbuffer);
    }

    SimpleTexture2D::SimpleTexture2D(const char* filepath) 
        : texture(GLTexture(2))
    {
        int channels;
        stbi_set_flip_vertically_on_load(true);
        unsigned char* image = stbi_load(filepath, &width, &height, &channels, 0);
        unsigned int format = channels_to_gl_type(channels);
        texture.set_internal_format(format);
        texture.set_data(image, format, width, height);
        stbi_image_free(image);
    };

    void SimpleTexture2D::bind(GLShader& shader, unsigned int slot) const {
        char name[10];
        sprintf_s(name, 10, "texture%u", slot);
        shader.set_uniform(name, (int) slot);
        glActiveTexture(GL_TEXTURE0 + slot);
        texture.bind();
    }
}

void MeshRenderer::init() {
    m_shader = std::make_shared<GLShader>();
    m_shader->add_source("../assets/shaders/triangle.vert");
    m_shader->add_source("../assets/shaders/lighting.frag");
    m_shader->add_source("../assets/shaders/triangle.frag");
    m_shader->compile();
}

void MeshRenderer::add_cube_mesh(Entity& cube) const {
    static std::vector<float> vertices = {
        // back
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, -1.0f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 0.0f, -1.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 0.0f, -1.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f, 0.0f, -1.0f,  1.0f, 1.0f,

        // front
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f,

        // left
        -0.5f, -0.5f, -0.5f,  -1.0f, 0.0f, 0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  -1.0f, 0.0f, 0.0f,  0.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  -1.0f, 0.0f, 0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  -1.0f, 0.0f, 0.0f,  1.0f, 1.0f,

        // right
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  1.0f, 1.0f,

        // bottom
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f, 0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f, 0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f, 0.0f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f, 0.0f,  1.0f, 1.0f,

        // top
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f,
    };
    static std::vector<uint32_t> indices = {
        0, 1, 2, 1, 2, 3, // back
        4, 5, 6, 5, 6, 7, // front
        8, 9, 10, 9, 10, 11, // left
        12, 13, 14, 13, 14, 15, // right
        16, 17, 18, 17, 18, 19, // bottom
        20, 21, 22, 21, 22, 23  // top
    };
    static GLBufferLayout layout = GLBufferLayout({
        GLBufferElement("Position", GLType::Float3),
        GLBufferElement("Normal", GLType::Float3),
        GLBufferElement("Texture Coordinates", GLType::Float2),
    });

    cube.add<Component::SimpleMesh>(indices, vertices, layout);
    // cube.add<Component::SimpleTexture2D>("../assets/wood_container.jpg");
}

void MeshRenderer::begin(glm::mat4& projectionview) const {
    m_shader->bind();
    m_shader->set_uniform("projectionview", projectionview);
    // TODO:
    m_shader->set_uniform("light_direction", glm::normalize(glm::vec3(0.0f, -1.0f, 0.0)));
    m_shader->set_uniform("light_color", glm::vec3(0.8f, 0.95f, 1.0f));
    m_shader->set_uniform("ambient_color", glm::vec3(0.2f));
}

void MeshRenderer::draw_mesh(Entity e) const {
    auto& mesh = e.get<Component::SimpleMesh>();
    auto& texture = e.get<Component::SimpleTexture2D>();
    auto& transform = e.get<Component::Transform>();
    m_shader->set_uniform("model", transform.get_matrix());
    m_shader->set_uniform("normalmatrix", transform.get_normalmatrix());
    texture.bind(*m_shader, 0);
    mesh.bind();
    glDrawElements(GL_TRIANGLES, mesh.size(), GL_UNSIGNED_INT, 0);
}

void MeshRenderer::end() const {
    GLVertexArray::unbind();
}