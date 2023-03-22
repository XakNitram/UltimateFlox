// Copyright (c) 2023 Tate "Xak Nitram" Mioton

#include "pch.hpp"
#include "Object.hpp"
#include "Structures/RawArray.hpp"

Object::Object(size_t vertexCount, size_t indexCount, lwvl::PrimitiveMode mode) :
    m_vertexCount(vertexCount), m_indexCount(indexCount), m_drawMode(mode), m_vertices(vertexCount), m_indices(indexCount)
{}

Object::Object(Object &&other)  noexcept :
    m_vertexCount(other.m_vertexCount),
    m_indexCount(other.m_indexCount),
    m_drawMode(other.m_drawMode),
    m_vertices(std::move(other.m_vertices)),
    m_indices(std::move(other.m_indices))
{}

Object &Object::operator=(Object &&rhs) noexcept {
    m_vertexCount = rhs.m_vertexCount;
    m_indexCount = rhs.m_indexCount;
    m_drawMode = rhs.m_drawMode;

    m_vertices = std::move(rhs.m_vertices);
    m_indices = std::move(rhs.m_indices);

    return *this;
}

const std::vector<float> &Object::vertices() const {
    return m_vertices;
}

std::vector<float> &Object::vertices() {
    return m_vertices;
}

size_t Object::vertexCount() const {
    return m_vertexCount;
}

const std::vector<unsigned int> &Object::indices() const {
    return m_indices;
}

std::vector<unsigned int> &Object::indices() {
    return m_indices;
}

size_t Object::indexCount() const {
    return m_indexCount;
}

lwvl::PrimitiveMode Object::drawMode() const {
    return m_drawMode;
}

Object loadObject(
    const float *_vertices, size_t vertexCount,
    const unsigned int *_indices, size_t indexCount,
    lwvl::PrimitiveMode mode
) {
    auto vertices = RawArray(_vertices, vertexCount);
    auto indices = RawArray(_indices, indexCount);
    Object obj{vertexCount, indexCount, mode};

    std::copy(vertices.begin(), vertices.end(), obj.vertices().data());
    std::copy(indices.begin(), indices.end(), obj.indices().data());

    return obj;
}

void loadObject(std::unordered_map<std::string, Object> &objects, const std::string &objDescriptor) {

}
