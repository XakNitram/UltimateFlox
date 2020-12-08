#include "pch.hpp"
#include "Flock.hpp"


const float Boid::scale = 10.0f;
const float Boid::visionRadius = Boid::scale * 2.0f;


Flock::Flock(uint32_t maxBoids): m_instanceData(maxBoids * 3), m_boidCount(maxBoids), m_flock(maxBoids), m_flockCopy(maxBoids) {
    // Boid Model
    std::array<float, 8> model {
        std::cosf(0.0f),       std::sinf(0.0f),
        std::cosf(0.75f * PI), std::sinf(0.75f * PI),
        0.5f * std::cosf(PI),  0.5f * std::sinf(PI),
        std::cosf(1.25f * PI), std::sinf(1.25 * PI)
    };

    m_array.bind();
    m_modelBuffer.bind();
    m_modelBuffer.construct(model.begin(), model.end());
    m_array.attribute(2, GL_FLOAT, 2 * sizeof(float), 0, 0);
    m_modelBuffer.clear();

    // Indices into model
    std::array<uint8_t, 10> indices { 0, 1, 0, 2, 0, 3, 2, 1, 2, 3 };

    m_indexBuffer.bind();
    m_indexBuffer.construct(indices.begin(), indices.end());
    m_indexBuffer.clear();

    float angle = 0.0f;
    float slice = 2.0f * PI / m_boidCount;
    for (uint32_t i = 0; i < m_boidCount; i++) {
        float xComp = std::cosf(angle);
        float yComp = std::sinf(angle);
        
        float x = 10.0f * slice * xComp;
        float y = 10.0f * slice * yComp;

        Boid& boid = m_flock[i];
        boid.position.x = x;
        boid.position.y = y;

        boid.velocity.x = 0.5f * xComp;
        boid.velocity.y = 0.5f * yComp;

        m_instanceData[i * 3 + 0] = x;
        m_instanceData[i * 3 + 1] = y;
        m_instanceData[i * 3 + 2] = angle;
        
        m_flockCopy[i] = boid;

        angle += slice;
    }

    m_array.instances(m_boidCount);
    m_instanceBuffer.bind();
    m_instanceBuffer.usage(lwvl::Usage::Stream);
    m_instanceBuffer.construct(m_instanceData.begin(), m_instanceData.end());
    m_array.attribute(2, GL_FLOAT, 3 * sizeof(float), 0,                 1);
    m_array.attribute(1, GL_FLOAT, 3 * sizeof(float), 2 * sizeof(float), 1);
    m_instanceBuffer.clear();
    m_array.clear();
}

void Flock::update(std::vector<LineSegment>& scene) {
    std::vector<float>& drawData = m_instanceData;

    for (uint32_t i = 0; i < m_boidCount; i++) {
        Boid& boidCopy = m_flockCopy[i];
        Boid& boid = m_flock[i];

        boid.velocity += boidCopy.velocityBuffer;
        boid.position += boid.velocity;
        boid.velocityBuffer *= 0.0f;

        // Update draw data
        drawData[i * 3 + 0] = boid.position.x;
        drawData[i * 3 + 1] = boid.position.y;
        drawData[i * 3 + 2] = boid.velocity.angle();
    }

    m_instanceBuffer.bind();
    m_instanceBuffer.update(drawData.begin(), drawData.end());
    m_instanceBuffer.clear();

    // Once we have processed all of the boids,
    // we copy their data back into the copy flock
    // for the next tick's update.
    for (uint32_t i = 0; i < m_boidCount; i++) {
        m_flockCopy[i] = m_flock[i];
    }
}


void Flock::draw() {
    m_array.bind();
    m_indexBuffer.bind();
    m_array.drawElements(GL_LINES, 10, GL_UNSIGNED_BYTE);
    m_indexBuffer.clear();
    m_array.clear();
}
