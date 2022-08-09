#include "pch.hpp"
#include "World/Boid.hpp"
#include "Controls.hpp"

void BoidShader::draw(Model *model) {
    control.bind();
    model->draw();
    lwvl::Program::clear();
}

DefaultBoidShader::DefaultBoidShader(Projection &proj) {
    control.link(
        lwvl::VertexShader::readFile("Data/Shaders/boid.vert"),
        lwvl::FragmentShader::readFile("Data/Shaders/boid.frag")
    );

    control.bind();
    control.uniform("scale").setF(Boid::scale);
    control.uniform("color").setF(Boid::color.r, Boid::color.g, Boid::color.b);
    control.uniform("projection").matrix4F(&proj[0][0]);
}

SpeedDebugShader::SpeedDebugShader(Projection &proj) {
    control.link(
        lwvl::VertexShader::readFile("Data/Shaders/speeddebug.vert"),
        lwvl::FragmentShader::readFile("Data/Shaders/speeddebug.frag")
    );

    control.bind();
    control.uniform("scale").setF(Boid::scale);
    control.uniform("projection").matrix4F(&proj[0][0]);
    control.uniform("maxSpeed").setF(Boid::maxSpeed);
}

VisionShader::VisionShader(Projection &proj) {
    control.link(
        lwvl::VertexShader::readFile("Data/Shaders/vision.vert"),
        lwvl::FragmentShader::readFile("Data/Shaders/default.frag")
    );

    control.bind();
    control.uniform("color").setF(1.0f, 1.0f, 1.0f);
    control.uniform("alpha").setF(0.30f);
    control.uniform("projection").matrix4F(&proj[0][0]);
}

void VisionShader::radius(float radius) {
    control.bind();
    control.uniform("scale").setF(radius);
}
