#pragma once

#include "pch.hpp"
#include "Models.hpp"

// Determine whether these should have to be passed shader strings

class BoidShader {
public:
    virtual ~BoidShader() = default;

    void draw(Model *);

protected:
    lwvl::Program control;
};


class DefaultBoidShader : public BoidShader {
public:
    explicit DefaultBoidShader(Projection &proj);

    ~DefaultBoidShader() override = default;
};


class SpeedDebugShader : public BoidShader {
public:
    explicit SpeedDebugShader(Projection &proj);

    ~SpeedDebugShader() override = default;
};


class VisionShader : public BoidShader {
public:
    explicit VisionShader(Projection &proj);

    ~VisionShader() override = default;

    void radius(float);

private:
    lwvl::Uniform u_radius;
};