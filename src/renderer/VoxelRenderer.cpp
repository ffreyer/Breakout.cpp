#include "VoxelRenderer.hpp"

void VoxelRenderer::init() {
    // TODO: reorganize rendering to centered between voxels:
    // --x---+---x---+--
    //       |       |
    //   1   x   2   x
    //       |       |
    // --x---+---x---+--
    // This would reduce each vertex to generating 0 or 2 triangles, rather than up to 12.
    // It would also remove the potential for doubly-rendered faces, e.g:
    // +---++---+
    // | x || x |
    // +---++---+

    // This is a basic, simple implementation
    // We consider a 16^3 chunk of voxels.
    // Voxel information (i.e. voxel block type) is placed in a 
    // 3D texture with air being 0. This should allow us to discard
    // hidden faces in a geometry shader.
    // The Vertexarray contains static position related to the values
    // in the texture. They get translated by a Transform as a chunk.

    // Vertices
    glm::ivec3 vertices[Component::Chunk::SIZE];
    for (int z = 0; z < Component::Chunk::LENGTH; z++)
        for (int y = 0; y < Component::Chunk::LENGTH; y++)
            for (int x = 0; x < Component::Chunk::LENGTH; x++)
                vertices[x + Component::Chunk::LENGTH * (y + Component::Chunk::LENGTH * z)] = 
                    glm::ivec3(x, y, z);

    GLBufferLayout layout = GLBufferLayout({ GLBufferElement("Position", GLType::Int3) });
    std::shared_ptr<GLVertexBuffer> vbuffer = std::make_shared<GLVertexBuffer>(
        vertices, sizeof(glm::ivec3) * Component::Chunk::SIZE, GLVertexBuffer::STREAM_DRAW);
    vbuffer->set_layout(layout);
    render_data.va = std::make_shared<GLVertexArray>();
    render_data.va->push(vbuffer);

    // Texture
    render_data.block_id = std::make_shared<GLTexture>(3);
    render_data.block_id->set_min_filter(GLTexture::NEAREST);
    render_data.block_id->set_mag_filter(GLTexture::NEAREST);
    render_data.block_id->set_internal_format(GLTexture::R8UI);
    render_data.block_id->set_slot(1);

    // Shader
    render_data.shader = std::make_shared<GLShader>();
    render_data.shader->add_source("../assets/shaders/3D/voxel.vert");
    render_data.shader->add_source("../assets/shaders/3D/voxel.geom");
    render_data.shader->add_source("../assets/shaders/3D/lighting.frag");
    render_data.shader->add_source("../assets/shaders/3D/voxel.frag");
    render_data.shader->compile();

    render_data.shadow_shader = std::make_shared<GLShader>();
    render_data.shadow_shader->add_source("../assets/shaders/3D/voxel.vert");
    render_data.shadow_shader->add_source("../assets/shaders/3D/shadow_voxel.geom");
    render_data.shadow_shader->add_source("../assets/shaders/3D/shadow.frag");
    render_data.shadow_shader->compile();

    // lut for uv indices
    int32_t lut[16 * 16 * 6];
    for (int i = 0; i < 16*16; i++)
        for (int side = 0; side < 6; side++)
            lut[6*i + side] = i;
    
    render_data.uv_idx_map = std::make_shared<GLBuffer>(
        GLBuffer::UNIFORM_BUFFER, (void*) lut, sizeof(lut), GLBuffer::STATIC_DRAW);

    // texture map
    render_data.texture_map = std::make_shared<RegularTextureAtlas>();
    render_data.texture_map->set_min_filter(GLTexture::NEAREST);
    render_data.texture_map->set_mag_filter(GLTexture::NEAREST);
    render_data.texture_map->load("../assets/texture_map.png");
    render_data.texture_map->set_element_pixel_size(16, 16);
    render_data.texture_map->set_slot(2);
}

void VoxelRenderer::begin(glm::mat4& projectionview) const {
    render_data.shader->bind();
    render_data.va->bind();

    render_data.shader->set_uniform("block_id", 1);
    render_data.block_id->bind();

    render_data.uv_idx_map->bind();
    render_data.shader->set_uniform_block("uv_lut_block", 0);
    render_data.uv_idx_map->bind_buffer_base(0);

    render_data.shader->set_uniform("texture_map", 2);
    render_data.shader->set_uniform("tex_uv_size", render_data.texture_map->get_uv_size());
    render_data.shader->set_uniform("tex_index_size", render_data.texture_map->get_index_size());
    render_data.texture_map->bind();

    render_data.shader->set_uniform("projectionview", projectionview);
    // TODO:
    render_data.shader->set_uniform("light_direction", glm::normalize(glm::vec3(0.0f, -1.0f, 0.0)));
    render_data.shader->set_uniform("light_color", glm::vec3(0.8f, 0.95f, 1.0f));
    render_data.shader->set_uniform("ambient_color", glm::vec3(0.2f));
}

void VoxelRenderer::render(Entity e) const {
    auto& chunk = e.get<Component::Chunk>();
    auto& transform = e.get<Component::Transform>();
    render_data.block_id->set_data(chunk.data, GLTexture::RED_INTEGER, Component::Chunk::LENGTH, Component::Chunk::LENGTH, Component::Chunk::LENGTH);
    render_data.shader->set_uniform("model", transform.get_matrix());
    render_data.shader->set_uniform("normalmatrix", transform.get_normalmatrix());
    glDrawArrays(GL_POINTS, 0, Component::Chunk::SIZE);
}

void VoxelRenderer::end() const {
    render_data.va->unbind();
    render_data.block_id->unbind();
}

void VoxelRenderer::begin_shadow(glm::mat4 &projectionview) const {
    render_data.shadow_shader->bind();
    render_data.va->bind();
    render_data.shadow_shader->set_uniform("block_id", 0);
    render_data.block_id->bind();
    render_data.shadow_shader->set_uniform("projectionview", projectionview);
}

void VoxelRenderer::render_shadow(Entity e) const {
    auto& chunk = e.get<Component::Chunk>();
    auto& transform = e.get<Component::Transform>();
    render_data.block_id->set_data(chunk.data, GLTexture::RED_INTEGER, Component::Chunk::LENGTH, Component::Chunk::LENGTH, Component::Chunk::LENGTH);
    render_data.shadow_shader->set_uniform("model", transform.get_matrix());
    glDrawArrays(GL_POINTS, 0, Component::Chunk::SIZE);
}
