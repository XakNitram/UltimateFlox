#pragma once
#include "pch.hpp"
#include "Renderer/VertexArray.hpp"
#include "Renderer/Buffer.hpp"
#include "Math/Geometrics.hpp"

constexpr float PI = 3.141592653589793f;


struct Boid {
    static const float scale;
    static const float visionRadius;

    Point position;
    Vector velocity;
    Vector velocityBuffer;

    Boid() = default;
};


class Flock {
    lwvl::VertexArray m_array;
    lwvl::ArrayBuffer m_modelBuffer;
    lwvl::ElementBuffer m_indexBuffer;
    lwvl::ArrayBuffer m_instanceBuffer;

    std::vector<float> m_instanceData;
    const uint32_t m_boidCount = 0;

    std::vector<Boid> m_flock;
    std::vector<Boid> m_flockCopy;

public:
    Flock(uint32_t maxBoids);

    void update(std::vector<LineSegment>& scene);
    void draw();
};
