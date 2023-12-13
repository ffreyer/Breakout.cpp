// only need this for value ptr so I guess we keep it here?
#include <glm/gtc/type_ptr.hpp>

#include "GLShader.hpp"

#include <iostream>

// From Below
std::string file_to_string(const char* filepath);

// Class Functions
GLShader::GLShader(std::initializer_list<const char*> paths) 
    : GLShader()
{
    for (const char* path : paths)
        add_source(path);
    compile();
}

GLShader::~GLShader() {
    if (m_id)
        glDeleteProgram(m_id);
}

bool GLShader::add_source(const char *filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cout << "File at " << filepath << " could not be opened." << std::endl;
        return false;
    }
    file.close();
    int N = strlen(filepath);
    const char* ptr = (const char*) ((unsigned long long) filepath + N - 4);
    if (strcmp(ptr, "vert") == 0)
        m_vertex_shader_filepaths.push_back(filepath);
    else if (strcmp(ptr, "frag") == 0)
        m_fragment_shader_filepaths.push_back(filepath);
    else if (strcmp(ptr, "geom") == 0)
        m_geometry_shader_filepaths.push_back(filepath);
    else {
        std::cout << "Failed to recognize file ending: " << ptr << std::endl;
        return false;
    }

    return true;
}

bool GLShader::_compile_single(std::vector<const char*>& paths, unsigned int type, const char* name, std::vector<unsigned int>& ids) const {
    for (const char* filepath : paths) {
        unsigned int id = generate_shader(file_to_string(filepath).c_str(), type);
        if (id)
            ids.push_back(id);
        else {
            std::cout << "Failed to create " << name << " shader from " << filepath << std::endl;
            return false;
        }
    }
    return true;
}

bool GLShader::compile() {
    std::vector<unsigned int> vertex_shader_ids;
    if (!_compile_single(m_vertex_shader_filepaths, GL_VERTEX_SHADER, "vertex", vertex_shader_ids))
        return false;

    std::vector<unsigned int> geometry_shader_ids;
    if (!_compile_single(m_geometry_shader_filepaths, GL_GEOMETRY_SHADER, "geometry", geometry_shader_ids))
        return false;

    std::vector<unsigned int> fragment_shader_ids;
    if (!_compile_single(m_fragment_shader_filepaths, GL_FRAGMENT_SHADER, "fragment", fragment_shader_ids))
        return false;

    m_id = generate_program(vertex_shader_ids, geometry_shader_ids, fragment_shader_ids);

    return false;
}

unsigned int GLShader::get_uniform_location(const std::string &name) const {
    unsigned int id = glGetUniformLocation(m_id, name.c_str());
    if (id == -1)
        std::cout << "Failed to find location of " << name << std::endl;
    return id;
}

unsigned int GLShader::get_block_index(const std::string &name) const {
    unsigned int id = glGetUniformBlockIndex(m_id, name.c_str());
    if (id == -1)
        std::cout << "Failed to find block index of " << name << std::endl;
    return id;
}

void GLShader::bind() {
    glUseProgram(m_id);
    m_name_to_slot.clear();
    m_texture_slot = 0;
}

void GLShader::set_uniform(const std::string &name, bool v1) const {
    glUniform1i(get_uniform_location(name), v1);
}
void GLShader::set_uniform(const std::string &name, bool v1, bool v2) const {
    glUniform2i(get_uniform_location(name), v1, v2);
}
void GLShader::set_uniform(const std::string &name, bool v1, bool v2, bool v3) const {
    glUniform3i(get_uniform_location(name), v1, v2, v3);
}
void GLShader::set_uniform(const std::string &name, bool v1, bool v2, bool v3, bool v4) const {
    glUniform4i(get_uniform_location(name), v1, v2, v3, v4);
}

void GLShader::set_uniform(const std::string &name, int v1) const {
    glUniform1i(get_uniform_location(name), v1);
}
void GLShader::set_uniform(const std::string &name, int v1, int v2) const {
    glUniform2i(get_uniform_location(name), v1, v2);
}
void GLShader::set_uniform(const std::string &name, int v1, int v2, int v3) const {
    glUniform3i(get_uniform_location(name), v1, v2, v3);
}
void GLShader::set_uniform(const std::string &name, int v1, int v2, int v3, int v4) const {
    glUniform4i(get_uniform_location(name), v1, v2, v3, v4);
}

void GLShader::set_uniform(const std::string &name, float v1) const {
    glUniform1f(get_uniform_location(name), v1);
}
void GLShader::set_uniform(const std::string &name, float v1, float v2) const {
    glUniform2f(get_uniform_location(name), v1, v2);
}
void GLShader::set_uniform(const std::string &name, float v1, float v2, float v3) const {
    glUniform3f(get_uniform_location(name), v1, v2, v3);
}
void GLShader::set_uniform(const std::string &name, float v1, float v2, float v3, float v4) const {
    glUniform4f(get_uniform_location(name), v1, v2, v3, v4);
}


void GLShader::set_uniform(const std::string &name, glm::mat2 mat) const {
    glUniformMatrix2fv(get_uniform_location(name), 1, GL_FALSE, glm::value_ptr(mat));
}
void GLShader::set_uniform(const std::string &name, const glm::mat3& mat) const {
    glUniformMatrix3fv(get_uniform_location(name), 1, GL_FALSE, glm::value_ptr(mat));
}
void GLShader::set_uniform(const std::string &name, const glm::mat4& mat) const {
    glUniformMatrix4fv(get_uniform_location(name), 1, GL_FALSE, glm::value_ptr(mat));
}

void GLShader::set_uniform(const std::string &name, glm::vec2 vec) const {
    glUniform2fv(get_uniform_location(name), 1, glm::value_ptr(vec));
}
void GLShader::set_uniform(const std::string &name, glm::vec3 vec) const {
    glUniform3fv(get_uniform_location(name), 1, glm::value_ptr(vec));
}
void GLShader::set_uniform(const std::string &name, glm::vec4 vec) const {
    glUniform4fv(get_uniform_location(name), 1, glm::value_ptr(vec));
}

void GLShader::set_uniform(const std::string &name, glm::ivec2 vec) const {
    glUniform2iv(get_uniform_location(name), 1, glm::value_ptr(vec));
}
void GLShader::set_uniform(const std::string &name, glm::ivec3 vec) const {
    glUniform3iv(get_uniform_location(name), 1, glm::value_ptr(vec));
}
void GLShader::set_uniform(const std::string &name, glm::ivec4 vec) const {
    glUniform4iv(get_uniform_location(name), 1, glm::value_ptr(vec));
}

void GLShader::set_uniform(const std::string &name, AbstractGLTexture &texture) {
    int8_t slot;
    if (m_name_to_slot.find(name) != m_name_to_slot.end()) {
        // if the name already has a slot we use that slot
        slot = m_name_to_slot[name];
    } else {
        // if the name is not known we allocate a new slot
        slot = m_texture_slot++;
        m_name_to_slot[name] = slot;

        if (slot > m_max_slots) // TODO: improve error type
            throw std::invalid_argument("No more texture slots available!");
    }
    glActiveTexture(GL_TEXTURE0 + slot);
    texture.bind();
    set_uniform(name, slot);
}

void GLShader::set_uniform_block(const std::string &name, int trg) const {
    glUniformBlockBinding(m_id, get_block_index(name), trg);
}

// Utility Functions / verbose Implementations
// TODO: maybe move
std::string file_to_string(const char* filepath) {
    std::ifstream file;
    // ensure ifstream objects can throw exceptions:
    file.exceptions (std::ifstream::failbit | std::ifstream::badbit);

    try {
        // open files
        file.open(filepath);
        std::stringstream str_stream;
        // read file's buffer contents into streams
        str_stream << file.rdbuf();
        // close file handlers
        file.close();
        // convert stream into string
        return str_stream.str();
    } catch(std::ifstream::failure e) {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
        return std::string();
    } 
}

unsigned int GLShader::generate_shader(const char *shader_source, unsigned int type) const {
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &shader_source, NULL);
    glCompileShader(shader);

    int  success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        char shader_type_name[16];

        switch (type) {
        case GL_VERTEX_SHADER:
            strcpy_s(shader_type_name, "VERTEX");
            break;
        case GL_GEOMETRY_SHADER:
            strcpy_s(shader_type_name, "GEOMETRY");
            break;
        case GL_FRAGMENT_SHADER:
            strcpy_s(shader_type_name, "FRAGMENT");
            break;
        default:
            strcpy_s(shader_type_name, "UNKNOWN");
            break;
        }
        std::cout << "ERROR::SHADER::" << shader_type_name << "::COMPILATION_FAILED\n" << infoLog << std::endl;
        std::cout << shader_source << std::endl;
        glDeleteShader(shader);
        return NULL;
    }

    return shader;
}

unsigned int GLShader::generate_program(
        std::vector<unsigned int>& vertex_shaders, 
        std::vector<unsigned int>& geometry_shaders,
        std::vector<unsigned int>& fragment_shaders
    ) const
{
    unsigned int shader_program = glCreateProgram();

    for (unsigned int vertex_shader : vertex_shaders)
        glAttachShader(shader_program, vertex_shader);
    for (unsigned int geometry_shader : geometry_shaders)
        glAttachShader(shader_program, geometry_shader);
    for (unsigned int fragment_shader : fragment_shaders)
        glAttachShader(shader_program, fragment_shader);

    glLinkProgram(shader_program);

    int  success;
    char infoLog[512];
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(shader_program, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::COMPILATION_FAILED\n" << infoLog << std::endl;
        return NULL;
    }

    for (unsigned int vertex_shader : vertex_shaders)
        glDeleteShader(vertex_shader);
    for (unsigned int geometry_shader : geometry_shaders)
        glDeleteShader(geometry_shader);
    for (unsigned int fragment_shader : fragment_shaders)
        glDeleteShader(fragment_shader);

    return shader_program;
}