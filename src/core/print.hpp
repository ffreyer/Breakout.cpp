#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

// TODO: rework this to work with <<

void print(glm::mat4 M) {
    glm::mat4 temp = glm::transpose(M);
    for (int i = 0; i < 4; i++) {
        glm::vec4 v = temp[i];
        for (int j = 0; j < 4; j++) {
            std::cout << v[j] << ", ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

template<int L, typename T, glm::qualifier Q>
void print(glm::vec<L, T, Q> v) {
    for (int j = 0; j < L-1; j++)
        std::cout << v[j] << ", ";
    std::cout << v[L-1] << std::endl;
}
