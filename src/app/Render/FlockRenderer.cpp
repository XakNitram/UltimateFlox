#include "pch.hpp"
#include "FlockRenderer.hpp"

FlockRenderer::FlockRenderer(size_t size): flockSize(size) {
    data.store<float>(nullptr, size * sizeof(Boid), lwvl::bits::Dynamic);
}

void FlockRenderer::update(const BoidArray &array) {
    data.update(array.get(), flockSize * sizeof(Boid));
}

void FlockRenderer::resize(size_t size) {
    data.store<float>(nullptr, size * sizeof(Boid), lwvl::bits::Dynamic);
    flockSize = size;
}

void FlockRenderer::draw(BoidModel *model, BoidShader *shader) {
    shader->draw(model);
}
