#pragma once

#include "Shader.hpp"
#include "Buffer.hpp"
#include "VertexArray.hpp"
#include "Common.hpp"
#include "Math/Constants.hpp"
#include "Math/Vector.hpp"

struct Boid {
    static constexpr float scale = 5.0f;
    static constexpr float maxSpeed = 25.0f;

    Vector position, velocity, acceleration;
};


class Flock {
    // without any steering, this number can go above 500,000 before dipping below 60fps
    uint64_t flockSize;

    std::unique_ptr<Boid[]> m_primaryFlock = std::make_unique<Boid[]>(flockSize);
    std::unique_ptr<Boid[]> m_secondaryFlock = std::make_unique<Boid[]>(flockSize);
    std::unique_ptr<float[]> offsetArray = std::make_unique<float[]>(flockSize * 4);

    lwvl::ShaderProgram boidShader;
    lwvl::VertexArray arrayBuffer;
    lwvl::ArrayBuffer vertexBuffer{lwvl::Usage::Static};
    lwvl::ElementBuffer indexBuffer{lwvl::Usage::Static};
    lwvl::ArrayBuffer offsetBuffer{lwvl::Usage::Stream};

    lwvl::PrimitiveMode renderMode = lwvl::PrimitiveMode::Lines;
    uint32_t indexCount = 10;
    Vector bounds;

public:

    explicit Flock(size_t flock_size, float aspect);
    void changeRenderMode(lwvl::PrimitiveMode mode);
    void update(float dt);
    void draw();

private:
    void configureRendering();
};