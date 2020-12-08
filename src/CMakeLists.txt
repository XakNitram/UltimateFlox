add_executable(
    ultimate-flox Application.cpp
    Flock.hpp
    Flock.cpp

    # Core
    Core/Utils.hpp
    Core/Utils.cpp
    Core/Simulation.hpp
    Core/Simulation.cpp

    # Math
    Math/Geometrics.hpp
    Math/Geometrics.cpp

    # Renderer
    Renderer/GLDebug.hpp
    Renderer/GLDebug.cpp
    Renderer/Shader.hpp
    Renderer/Shader.cpp
    Renderer/VertexArray.hpp
    Renderer/VertexArray.cpp
    Renderer/Buffer.hpp
)

target_link_libraries(ultimate-flox PRIVATE glad)
target_link_libraries(ultimate-flox PRIVATE glfw)
target_precompile_headers(ultimate-flox PRIVATE pch.hpp pch.cpp)

set_target_properties(
    ultimate-flox PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

add_custom_command(
    TARGET ultimate-flox POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/res/Shaders/
    $<TARGET_FILE_DIR:ultimate-flox>/Shaders/
)
message(STATUS "Copied shaders to output")
