#include "pch.hpp"
#include "Flock.hpp"

Vector Boid::steer(Vector vec) const {
    vec.magnitude(Boid::maxSpeed);
    vec -= velocity;
    vec.magnitude(Boid::maxForce);
    return vec;
}


Flock::Flock(size_t flock_size, float aspect) : flockSize(flock_size), bounds{worldBound, worldBound, worldBound} {

    // Set up boid starting locations
    const float tauOverSize = constants::tau / static_cast<float>(flockSize);
    for (size_t i = 0; i < flockSize; i++) {
        Boid &boid = m_primaryFlock[i];
        auto angle = static_cast<float>(i) * tauOverSize;
        Vector offsets {cosf(angle), sinf(angle), 0.0f};
        boid.position = 50.0f * offsets;
        boid.velocity = 10.0f * offsets;
        boid.velocity.z = cosf(angle);
        boid.velocity.magnitude(Boid::maxSpeed);
    }

    // Set up boid batch rendering
    configureRendering(aspect);
}

void Flock::configureRendering(float aspect) {
    // Construct shader
    lwvl::VertexShader vertexSource{lwvl::VertexShader::readFile("Data/Shaders/boid.vert")};
    lwvl::FragmentShader fragmentSource{lwvl::FragmentShader::readFile("Data/Shaders/boid.frag")};
    boidShader.link(vertexSource, fragmentSource);

    boidShader.bind();
    boidShader.uniform("scale").set1f(Boid::scale);

    float x, y, z;
    if (aspect >= 1.0f) {
        x = bounds.x * aspect;
        y = bounds.y;
        z = bounds.z * constants::root3 + 50.0f;
    } else {
        x = bounds.x;
        y = bounds.y / aspect;
        z = bounds.z * constants::root3 + 50.0f;
    }

    boidShader.uniform("projection").setOrthographic(
        y, -y, x, -x, z, -z
    );

    //boidShader.uniform("color").set3f(1.00000f, 0.00000f, 0.00000f);  // Red
    //boidShader.uniform("color").set3f(1.00000f, 1.00000f, 1.00000f);  // White
    //boidShader.uniform("color").set3f(0.05098f, 0.19608f, 0.30196f);  // Prussian Blue
    //boidShader.uniform("color").set3f(0.30980f, 0.00392f, 0.27843f);  // Tyrian Purple
    boidShader.uniform("color").set3f(0.71373f, 0.09020f, 0.29412f);  // Pictoral Carmine
    //boidShader.uniform("color").set3f(0.76471f, 0.92157f, 0.47059f);  // Yellow Green Crayon

    arrayBuffer.bind();
    arrayBuffer.instances(flockSize);

    // Boid model vertex buffer
    vertexBuffer.bind();
    vertexBuffer.construct<float>(nullptr, maxBufferSize);
    vertexBuffer.update(defaultModel.begin(), defaultModel.end());
    arrayBuffer.attribute(3, GL_FLOAT, 3 * sizeof(float), 0);

    // Boid model indexing buffer
    indexBuffer.bind();
    std::array<uint8_t, 12> indexData {
        0, 1, 2, 2, 3, 0, 0, 4, 2, 2, 5, 0
    };

    indexBuffer.construct(indexData.begin(), indexData.end());

    // Per-boid offset buffer
    offsetBuffer.bind();
    for (size_t i = 0; i < flockSize; i++) {
        const Boid &boid = m_primaryFlock[i];
        const float componentFactor = 1.0f / boid.velocity.magnitude();

        offsetArray[i * 6 + 0] = boid.position.x;
        offsetArray[i * 6 + 1] = boid.position.y;
        offsetArray[i * 6 + 2] = boid.position.z;

        offsetArray[i * 6 + 3] = boid.velocity.x * componentFactor;
        offsetArray[i * 6 + 4] = boid.velocity.y * componentFactor;
        offsetArray[i * 6 + 5] = boid.velocity.z * componentFactor;
    }

    offsetBuffer.construct(offsetArray.get(), 6 * flockSize);
    arrayBuffer.attribute(3, GL_FLOAT, 6 * sizeof(float), 0, 1);
    arrayBuffer.attribute(3, GL_FLOAT, 6 * sizeof(float), 3 * sizeof(float), 1);
}

void Flock::update(float dt) {
    for (size_t i = 0; i < flockSize; i++) {
        m_secondaryFlock[i] = m_primaryFlock[i];
        Boid &currentBoid = m_secondaryFlock[i];

        Vector centerSteer;
        // Precursor to Rectangle class
        if (currentBoid.position.x - Boid::scale <= -bounds.x
            || currentBoid.position.x + Boid::scale >= bounds.x
            || currentBoid.position.y - Boid::scale <= -bounds.y
            || currentBoid.position.y + Boid::scale >= bounds.y
            || currentBoid.position.z - Boid::scale <= -bounds.z
            || currentBoid.position.z + Boid::scale >= bounds.z
        ) {
            centerSteer -= currentBoid.position;
            centerSteer = currentBoid.steer(centerSteer);
        }

        // Desire to move at full speed
        Vector fullSpeed = currentBoid.velocity;
        fullSpeed = currentBoid.steer(fullSpeed);

        Vector separation;  // Desire to separate from flockmates
        Vector alignment;  // Desire to align with the direction of other flockmates
        Vector cohesion;  // Desire to shrink the distance between self and flockmates
        size_t cohesiveTotal = 0;
        size_t disruptiveTotal = 0;
        for(size_t j = 0; j < flockSize; j++) {
            if (i == j) {
                continue;
            }

            const Boid& otherBoid = m_primaryFlock[j];
            const float d = currentBoid.position.distance(otherBoid.position);
            if (d < Boid::disruptiveRadius && d > 0.0f) {
                Vector diff = currentBoid.position - otherBoid.position;
                separation += diff / (d * d);
                disruptiveTotal++;
            }

            if (d < Boid::cohesiveRadius) {
                alignment += otherBoid.velocity;
                cohesion += otherBoid.position;
                cohesiveTotal++;
            }
        }

        if (disruptiveTotal > 0) {
            separation /= static_cast<float>(cohesiveTotal);
            separation = currentBoid.steer(separation);
        }

        if (cohesiveTotal > 0) {
            const float countFactor = 1.0f / static_cast<float>(cohesiveTotal);
            alignment *= countFactor;
            alignment = currentBoid.steer(alignment);

            cohesion *= countFactor;
            cohesion -= currentBoid.position;
            cohesion = currentBoid.steer(cohesion);
        }

        currentBoid.acceleration += centerSteer * 1.0f;
        currentBoid.acceleration += fullSpeed * 0.00625f;
        currentBoid.acceleration += separation * 1.0f;
        currentBoid.acceleration += alignment * 0.8f;
        currentBoid.acceleration += cohesion * 0.2f;

        currentBoid.acceleration.magnitude(Boid::maxForce);

        currentBoid.velocity += currentBoid.acceleration;
        currentBoid.position += currentBoid.velocity * dt;
        currentBoid.acceleration *= 0.0f;

        // Update the offsets
        offsetArray[i * 6 + 0] = currentBoid.position.x;
        offsetArray[i * 6 + 1] = currentBoid.position.y;
        offsetArray[i * 6 + 2] = currentBoid.position.z;

        // In terms of calculating a rotation,
        // sqrt seems to be faster than atan2
        // and saves cos and sin computations on the gpu
        float componentFactor = 1.0f / currentBoid.velocity.magnitude();
        offsetArray[i * 6 + 3] = currentBoid.velocity.x * componentFactor;
        offsetArray[i * 6 + 4] = currentBoid.velocity.y * componentFactor;
        offsetArray[i * 6 + 5] = currentBoid.velocity.z * componentFactor;
    }

    offsetBuffer.bind();
    offsetBuffer.update(offsetArray.get(), flockSize * 6);

    // Move array pointers
    auto temp = std::move(m_primaryFlock);
    m_primaryFlock = std::move(m_secondaryFlock);
    m_secondaryFlock = std::move(temp);

    //std::cout << "Boid 1 position: " << m_primaryFlock[0].position << std::endl;
    //for (size_t i = 0; i < 6; i++) {
    //    std::cout << "angle " << i << ": " << offsetArray[i] << std::endl;
    //}
}

void Flock::draw() {
    boidShader.bind();
    arrayBuffer.bind();
    arrayBuffer.drawElements(
        renderMode, indexCount,
        lwvl::ByteFormat::UnsignedByte
    );
}

static void loadDefaultModel(lwvl::ArrayBuffer &buffer) {
    buffer.bind();
    buffer.update(defaultModel.begin(), defaultModel.end());
}

static void loadPaperPlaneModel(lwvl::ArrayBuffer &buffer) {
    buffer.bind();
    buffer.update(paperPlaneModel, 18);
}

static void loadSpaceshipModel(lwvl::ArrayBuffer &buffer) {
    buffer.bind();
    buffer.update(spaceshipModel, 15);
}

void Flock::changeRenderMode(uint8_t mode) {
    switch(mode) {
        case 0: {
            renderMode = lwvl::PrimitiveMode::Lines;
            indexCount = 10;
            std::array<uint8_t, 10> indexData {
                0, 1,
                1, 2,
                2, 3,
                2, 0,
                3, 0
            };
            indexBuffer.bind();
            indexBuffer.update(indexData.begin(), indexData.end());
            loadPaperPlaneModel(vertexBuffer);
            return;
        }
        case 1: {
            renderMode = lwvl::PrimitiveMode::Triangles;
            indexCount = 12;
            std::array<uint8_t, 12> indexData {
                0, 1, 2,
                2, 3, 0,
                0, 4, 2,
                2, 5, 0
            };
            indexBuffer.bind();
            indexBuffer.update(indexData.begin(), indexData.end());
            loadDefaultModel(vertexBuffer);
            return;
        }
        case 2: {
            renderMode = lwvl::PrimitiveMode::Lines;
            indexCount = 6;
            indexBuffer.bind();
            std::array<uint8_t, 6> indexData {
                0, 1,
                0, 2,
                3, 4
            };
            indexBuffer.bind();
            indexBuffer.update(indexData.begin(), indexData.end());
            loadSpaceshipModel(vertexBuffer);
            return;
        }
        default:
            return;
    }
}
