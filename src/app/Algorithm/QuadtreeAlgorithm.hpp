#pragma once

#include "pch.hpp"
#include "Algorithm.hpp"
#include "World/Boidtree.hpp"

// Expose tree for rendering
class QuadtreeAlgorithm : public Algorithm {
public:
    explicit QuadtreeAlgorithm(Vector bounds);

    ~QuadtreeAlgorithm() override = default;

    void update(DoubleBuffer<Boid> &boids, float delta) override;

    [[nodiscard]] Boidtree const &tree() const;

protected:
    Rectangle m_bounds;
    Rectangle m_treeBounds;
    Boidtree m_tree;

    Boidtree::ResultVector m_results;
};
