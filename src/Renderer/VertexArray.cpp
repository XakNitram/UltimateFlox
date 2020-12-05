#include "../pch.hpp"
#include "VertexArray.hpp"

lwvl::VertexArray::VertexArray() {
    glGenVertexArrays(1, &id);
}

lwvl::VertexArray::~VertexArray() {
    glDeleteVertexArrays(1, &id);
}

void lwvl::VertexArray::bind() {
    glBindVertexArray(id);
}

void lwvl::VertexArray::clear() {
    glBindVertexArray(0);
}

void lwvl::VertexArray::attribute(uint8_t dimensions, GLenum type, int64_t stride, int64_t offset) {
    glEnableVertexAttribArray(attributes);
    glVertexAttribPointer(attributes, dimensions, type, GL_FALSE, stride, reinterpret_cast<void*>(offset));
    attributes++;
}

void lwvl::VertexArray::drawArrays(GLenum mode, int count) {
    glDrawArrays(mode, 0, count);
}
