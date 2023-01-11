#pragma once

constexpr int MaxDepth = 8;
using Vector = glm::vec2;
using Color = glm::vec3;
using Projection = glm::mat4;

template<int length, typename T, glm::qualifier Q>
std::ostream& operator<<(std::ostream& os, glm::vec<length, T, Q> vec) {
    os << '(';
    for(int i = 0; i < length; ++i) {
        if (i) { os << ", "; }
        os << vec[i];
    }
    os << ')';
    return os;
}
