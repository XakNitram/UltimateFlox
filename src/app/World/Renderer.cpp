#include "pch.hpp"
#include "Renderer.hpp"

DataBufferUpdater::DataBufferUpdater(size_t size) : flockSize(size) {
    dataBuffer.store<float>(nullptr, size * 6, lwvl::bits::Dynamic);
}

lwvl::Buffer &DataBufferUpdater::buffer() {
    return dataBuffer;
}

void DataBufferUpdater::update(const BoidArray &array) {
    dataBuffer.update(array.get(), flockSize);
}

void DataBufferUpdater::resize(size_t size) {
    dataBuffer.store<float>(nullptr, size * 6, lwvl::bits::Dynamic);
    flockSize = size;
}

BoidRenderer::BoidRenderer(size_t size, int width, int height, lwvl::Buffer& offsetBuffer) {
    const glm::vec2 bounds{calculateBounds(static_cast<float>(width) / static_cast<float>(height))};

    lwvl::Program& boidDefaultControl = controls[defaultControl];
    activeControl = boidDefaultControl;
    boidDefaultControl.link(
        lwvl::VertexShader::readFile("Data/Shaders/boid.vert"),
        lwvl::FragmentShader::readFile("Data/Shaders/boid.frag")
    );

    lwvl::Program& boidSpeedDebugControl = controls[speedDebugControl];
    boidSpeedDebugControl.link(
        lwvl::VertexShader::readFile("Data/Shaders/speeddebug.vert"),
        lwvl::FragmentShader::readFile("Data/Shaders/speeddebug.frag")
    );
    boidSpeedDebugControl.bind();
    boidSpeedDebugControl.uniform("maxSpeed").setF(Boid::maxSpeed);

    for (auto &control : controls) {
        control.bind();
        control.uniform("scale").setF(Boid::scale);
        control.uniform("projection").ortho2D(bounds.y, -bounds.y, bounds.x, -bounds.x);

        //control.uniform("color").set3f(1.00000f, 0.00000f, 0.00000f);  // Red
        //control.uniform("color").set3f(1.00000f, 1.00000f, 1.00000f);  // White
        //control.uniform("color").set3f(0.05098f, 0.19608f, 0.30196f);  // Prussian Blue
        //control.uniform("color").set3f(0.30980f, 0.00392f, 0.27843f);  // Tyrian Purple
        control.uniform("color").setF(0.71373f, 0.09020f, 0.29412f);  // Pictoral Carmine
        //control.uniform("color").set3f(0.17647f, 0.18824f, 0.27843f);  // Space Cadet
        //control.uniform("color").set3f(0.76471f, 0.92157f, 0.47059f);  // Yellow Green Crayon
        //control.uniform("color").set3f(0.91765f, 0.88235f, 0.31765f);  // Hard Yellow
    }

    layout.instances = size;

    // Boid model vertex buffer
    vertices.construct<float>(nullptr, defaultModel.size());  // Create a buffer of 10 floats
    vertices.update(defaultModel.begin(), defaultModel.end());
    layout.array(vertices, 0, 0, 2 * sizeof(float));
    layout.attribute(0, 0, 2, lwvl::ByteFormat::Float, 0);

    // Boid model indexing buffer
    std::array<uint8_t, 10> indexData{
        0, 1, 2, 3
    };

    indices.construct(indexData.begin(), indexData.end());
    layout.element(indices);
    layout.array(offsetBuffer, 1, 0, sizeof(Boid));
    layout.attribute(1, 1, 2, lwvl::ByteFormat::Float, 0);
    layout.attribute(1, 2, 2, lwvl::ByteFormat::Float, 2 * sizeof(float));
    layout.divisor(1, 1);
}

void BoidRenderer::changeRenderMode(RenderMode mode) {
    switch (mode) {
        case RenderMode::Classic: {
            renderMode = lwvl::PrimitiveMode::Lines;
            indexCount = 10;
            std::array<uint8_t, 10> indexData{
                0, 1, 1, 2, 2, 3, 2, 0, 3, 0
            };
            indices.update(indexData.begin(), indexData.end());
            vertices.update(
                defaultModel.begin(),
                defaultModel.end()
            );
            return;
        }
        case RenderMode::Filled: {
            renderMode = lwvl::PrimitiveMode::TriangleFan;
            indexCount = 4;
            std::array<uint8_t, 4> indexData{
                0, 1, 2, 3
            };
            indices.update(indexData.begin(), indexData.end());
            vertices.update(
                defaultModel.begin(),
                defaultModel.end()
            );
            return;
        }
        default:return;
    }
}

void BoidRenderer::changeControlMode(BoidRenderer::ControlMode mode) {
    switch(mode) {
        case ControlMode::Default:
            activeControl = controls[defaultControl];
            break;
        case ControlMode::SpeedDebug:
            activeControl = controls[speedDebugControl];
            break;
    }
}

void BoidRenderer::draw() {
    activeControl.bind();
    layout.drawElements(
        renderMode, indexCount,
        lwvl::ByteFormat::UnsignedByte
    );
}

void BoidRenderer::resize(size_t size) {
    layout.instances = size;
}

VisionRenderer::VisionRenderer(size_t size, int width, int height, lwvl::Buffer &offsetBuffer) {
    const glm::vec2 bounds{calculateBounds(static_cast<float>(width) / static_cast<float>(height))};

    // Construct Shader
    lwvl::VertexShader visionVertexModifier{lwvl::VertexShader::readFile("Data/Shaders/vision.vert")};
    lwvl::FragmentShader visionFragmentModifier{lwvl::FragmentShader::readFile("Data/Shaders/default.frag")};
    control.link(visionVertexModifier, visionFragmentModifier);
    control.bind();
    control.uniform("projection").ortho2D(bounds.y, -bounds.y, bounds.x, -bounds.x);

    layout.instances = size;

    vertices.construct(visionModel.begin(), visionModel.end());
    layout.array(vertices, 0, 0, 2 * sizeof(float));
    layout.attribute(0, 0, 2, lwvl::ByteFormat::Float, 0);

    layout.array(offsetBuffer, 1, 0, sizeof(Boid));
    layout.attribute(1, 1, 2, lwvl::ByteFormat::Float, 0);
    layout.divisor(1, 1);
}

void VisionRenderer::draw() {
    control.bind();
    control.uniform("scale").setF(Boid::cohesiveRadius);
    layout.drawArrays(lwvl::PrimitiveMode::LineStrip, 17);

    control.uniform("scale").setF(Boid::disruptiveRadius);
    layout.drawArrays(lwvl::PrimitiveMode::LineStrip, 17);
}

void VisionRenderer::resize(size_t size) {
    layout.instances = size;
}
