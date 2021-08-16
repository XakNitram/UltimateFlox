#pragma once

#include "Shader.hpp"
#include "Buffer.hpp"
#include "VertexArray.hpp"
#include "Common.hpp"

#include "World.hpp"
#include "Boid.hpp"
#include "Renderer.hpp"


class Flock {
    // without any steering, this number can go above 500,000 before dipping below 60fps
    size_t flockSize;

    BoidArray m_primaryFlock = std::make_unique<Boid[]>(flockSize);
    BoidArray m_secondaryFlock = std::make_unique<Boid[]>(flockSize);

    Vector m_bounds;
    detail::BoidRenderer m_renderer;

public:
    explicit Flock(size_t flock_size, int width, int height);
    void update(float dt);
    void draw();

    detail::BoidRenderer& renderer();
};