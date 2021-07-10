#pragma once

#include "pch.hpp"
#include "Boid.hpp"
#include "Shader.hpp"
#include "VertexArray.hpp"
#include "Buffer.hpp"

enum class BoidMode : uint8_t {
    Classic,
    Filled
};

constexpr std::array<float, 8> defaultModel {
    1.0, 0.0,
    -0.7071067811865475f, 0.7071067811865476f,
    -0.5, 0.0,
    -0.7071067811865477f, -0.7071067811865475f,
};

constexpr std::array<float, 34> visionModel{
    1.0f, 0.0f,
    0.92388f, 0.38268f,
    0.70711f, 0.70711f,
    0.38268f, 0.92388f,
    0.0f, 1.0f,
    -0.38268f, 0.92388f,
    -0.70711f, 0.70711f,
    -0.92388f, 0.38268f,
    -1.0f, 0.0f,
    -0.92388f, -0.38268f,
    -0.70711f, -0.70711f,
    -0.38268f, -0.92388f,
    -0.0f, -1.0f,
    0.38268f, -0.92388f,
    0.70711f, -0.70711f,
    0.92388f, -0.38268f,
    1.0f, 0.0f,
};

class BoidRenderer {
public:
    BoidRenderer(size_t size, glm::vec2 bounds);

    void changeRenderMode(BoidMode mode);
    void toggleVisionRendering();
    void toggleBoidRendering();

    void update(BoidArray const &boids);

    void draw();

private:
    size_t flockSize;

    // This can be replaced with glMapBuffer when that abstraction is available.
    std::unique_ptr<float[]> offsetStore = std::make_unique<float[]>(flockSize * 4);

    // Boid Rendering
    lwvl::ShaderProgram boidModifier;
    lwvl::VertexArray boidController;
    lwvl::ArrayBuffer boidVertices{lwvl::Usage::Static};
    lwvl::ElementBuffer boidIndices{lwvl::Usage::Static};

    // Boid Vision Rendering
    lwvl::ShaderProgram visionModifier;
    lwvl::VertexArray visionController;
    lwvl::ArrayBuffer visionVertices{lwvl::Usage::Static};

    // Offset Buffer
    lwvl::ArrayBuffer offsets{lwvl::Usage::Stream};

    lwvl::PrimitiveMode renderMode = lwvl::PrimitiveMode::TriangleFan;
    int32_t indexCount = 10;
    bool renderVision = false;
    bool renderBoids = true;
};
