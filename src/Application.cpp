#include "pch.hpp"
#include "Core/Utils.hpp"
#include "Core/Simulation.hpp"
#include "Renderer/Shader.hpp"
#include "Flock.hpp"


class UltimateFLox final : public Simulation {
    lwvl::ShaderProgram program;
public:
    UltimateFLox(unsigned int width, unsigned int height, GLFWmonitor* monitor = nullptr):
        Simulation(width, height, "UltimateFlox", monitor) 
    {
        
    }

    void draw() final {
        
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
