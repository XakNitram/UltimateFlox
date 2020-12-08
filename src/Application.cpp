#include "pch.hpp"
#include "Core/Utils.hpp"
#include "Core/Simulation.hpp"
#include "Renderer/GLDebug.hpp"
#include "Renderer/Shader.hpp"
#include "Math/Geometrics.hpp"
#include "Flock.hpp"

// TODO: Implement a scope-based OpenGL binder that binds on construction, then unbinds on destruction.


class UltimateFLox final : public Simulation {
    static void gl_callback(
        lwvl::debug::Source source, lwvl::debug::Type type, lwvl::debug::Severity severity, 
        unsigned int id, int length, const char* message, const void* userState
    ) {
        if (type == lwvl::debug::Type::ERROR) {
            throw std::exception(message);
        } else {
#ifndef NDEBUG
            std::cout << message << std::endl;
#endif
        }
    }

    lwvl::debug::GLEventListener listener;
    lwvl::ShaderProgram program;
    Flock flock;
    std::vector<LineSegment> scene;

public:
    UltimateFLox(unsigned int width, unsigned int height, GLFWmonitor* monitor = nullptr):
        Simulation(width, height, "UltimateFlox", monitor), listener(this, gl_callback), flock(10)
    {
        program.link(
            readFile("Shaders/boid.vert"),
            readFile("Shaders/boid.frag")
        );

        program.bind();

        float rightBound, topBound; {
            int actualWidth, actualHeight;
            glfwGetFramebufferSize(window, &actualWidth, &actualHeight);

            float widthf = static_cast<float>(actualWidth);
            float heightf = static_cast<float>(actualHeight);

            float aspectCorrection = 1.0f / std::min<float>(widthf, heightf);
            rightBound = 200.0f * widthf  * aspectCorrection; // at least 200 width
            topBound   = 200.0f * heightf * aspectCorrection; // at least 200 height

            // rightBound = widthf;
            // topBound = heightf;
        }
        
        program.uniform("scale").set1f(Boid::scale);
        program.uniform("projection").set2DOrthographic(
            static_cast<float>(topBound), static_cast<float>(-topBound),
            static_cast<float>(-rightBound), static_cast<float>(rightBound)
        );


        program.clear();

        Point NW{-rightBound, topBound};
        Point SW{-rightBound, -topBound};
        Point NE{rightBound, topBound};
        Point SE{rightBound, -topBound};

        scene.reserve(4);
        scene.emplace_back(SW, NW);
        scene.emplace_back(NW, NE);
        scene.emplace_back(NE, SE);
        scene.emplace_back(SE, SW);
    }

    void update(double dt) override {
        flock.update(scene);
    }

    void draw() override {
        glClear(GL_COLOR_BUFFER_BIT);
        program.bind();
        flock.draw();
        program.clear();
    }
};


int main() {
    try {
        UltimateFLox app(800, 600);
        app.run();
    } catch(std::exception& e) {
        std::cout << e.what() << std::endl;
    }
}
