// learnopengl uses a #ifndef - guard instead of this...?
#pragma once

#include <glad/gl.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#include <glm/glm.hpp>

// TODO:
// - move file reading code to function
// - file ending -> shader type
// - add method for adding sources, make constructor not load stuff
// - compile method
// - function/constructor for quick construction
// - consider multiple dispatch for set uniform? and templates?

class GLShader
{
private:
    unsigned int m_id = 0;

    std::vector<const char*> m_vertex_shader_filepaths;
    std::vector<const char*> m_geometry_shader_filepaths;
    std::vector<const char*> m_fragment_shader_filepaths;

public:
    GLShader() : 
        m_vertex_shader_filepaths(std::vector<const char*>()),
        m_geometry_shader_filepaths(std::vector<const char*>()),
        m_fragment_shader_filepaths(std::vector<const char*>())
    { 
    }

    GLShader(std::initializer_list<const char*> paths);
    ~GLShader();

    bool add_source(const char* filepath);
    bool compile();

    void use() const { glUseProgram(m_id); };
    void bind() const { glUseProgram(m_id); };

    void set_uniform(const std::string &name, bool v1) const;
    void set_uniform(const std::string &name, bool v1, bool v2) const;
    void set_uniform(const std::string &name, bool v1, bool v2, bool v3) const;
    void set_uniform(const std::string &name, bool v1, bool v2, bool v3, bool v4) const;

    void set_uniform(const std::string &name, int v1) const;
    void set_uniform(const std::string &name, int v1, int v2) const;
    void set_uniform(const std::string &name, int v1, int v2, int v3) const;
    void set_uniform(const std::string &name, int v1, int v2, int v3, int v4) const;

    void set_uniform(const std::string &name, float v1) const;
    void set_uniform(const std::string &name, float v1, float v2) const;
    void set_uniform(const std::string &name, float v1, float v2, float v3) const;
    void set_uniform(const std::string &name, float v1, float v2, float v3, float v4) const;

    void set_uniform(const std::string &name, glm::mat2 mat) const;
    void set_uniform(const std::string &name, const glm::mat3& mat) const;
    void set_uniform(const std::string &name, const glm::mat4& mat) const;

    void set_uniform(const std::string &name, glm::vec2 vec) const;
    void set_uniform(const std::string &name, glm::vec3 vec) const;
    void set_uniform(const std::string &name, glm::vec4 vec) const;

    void set_uniform(const std::string &name, glm::ivec2 vec) const;
    void set_uniform(const std::string &name, glm::ivec3 vec) const;
    void set_uniform(const std::string &name, glm::ivec4 vec) const;

    void set_uniform_block(const std::string &name, int trg) const;

private:
    unsigned int generate_shader(const char *shader_source, unsigned int type) const;
    unsigned int generate_program(
        std::vector<unsigned int>& vertex_shaders, 
        std::vector<unsigned int>& geometry_shaders,
        std::vector<unsigned int>& fragment_shaders
    ) const;
    
    bool _compile_single(
        std::vector<const char*>& paths, 
        unsigned int type, const char* name, 
        std::vector<unsigned int>& ids
    ) const;
    
    unsigned int get_uniform_location(const std::string &name) const;
    unsigned int get_block_index(const std::string &name) const;
};
