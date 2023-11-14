#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

template<int Lx, int Ly, typename T, glm::qualifier Q>
std::ostream& operator<<(std::ostream& stream, const glm::mat<Lx, Ly, T, Q>& M) {
    for (int i = 0; i < Ly-1; i++){
        for (int j = 0; j < Lx-1; j++)
            stream << M[i][j] << ", ";
        stream << M[Ly-1][i] << "\n";
    }

    for (int j = 0; j < Ly-1; j++)
            stream << M[j][Lx-1] << ", ";

    stream << M[Lx-1][Ly-1];
    return stream;
}

template<int L, typename T, glm::qualifier Q>
std::ostream& operator<<(std::ostream& stream, const glm::vec<L, T, Q> v) {
    stream << "vec" << L << "(";
    for (int j = 0; j < L-1; j++)
        stream << v[j] << ", ";
    stream << v[L-1] << ")";
	return stream;
}