#pragma once

#include <glad/gl.h>

template <typename T>
[[maybe_unused]] static GLenum gl_type_convert();

template <> [[maybe_unused]] GLenum gl_type_convert<float>() { return GL_FLOAT; }
template <> [[maybe_unused]] GLenum gl_type_convert<int>() { return GL_INT; }
template <> [[maybe_unused]] GLenum gl_type_convert<unsigned int>() { return GL_UNSIGNED_INT; }
template <> [[maybe_unused]] GLenum gl_type_convert<int8_t>() { return GL_BYTE; }
template <> [[maybe_unused]] GLenum gl_type_convert<uint8_t>() { return GL_UNSIGNED_BYTE; }
template <> [[maybe_unused]] GLenum gl_type_convert<int16_t>() { return GL_SHORT; }
template <> [[maybe_unused]] GLenum gl_type_convert<uint16_t>() { return GL_UNSIGNED_SHORT; }
template <> [[maybe_unused]] GLenum gl_type_convert<bool>() { return GL_BOOL; }

/*
For reference:
GL_UNSIGNED_BYTE, GL_BYTE, GL_UNSIGNED_SHORT, GL_SHORT, GL_UNSIGNED_INT, GL_INT, GL_FLOAT, GL_UNSIGNED_BYTE_3_3_2, GL_UNSIGNED_BYTE_2_3_3_REV, GL_UNSIGNED_SHORT_5_6_5, GL_UNSIGNED_SHORT_5_6_5_REV, GL_UNSIGNED_SHORT_4_4_4_4, GL_UNSIGNED_SHORT_4_4_4_4_REV, GL_UNSIGNED_SHORT_5_5_5_1, GL_UNSIGNED_SHORT_1_5_5_5_REV, GL_UNSIGNED_INT_8_8_8_8, GL_UNSIGNED_INT_8_8_8_8_REV, GL_UNSIGNED_INT_10_10_10_2, and GL_UNSIGNED_INT_2_10_10_10_REV. 
*/