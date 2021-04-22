#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec3 offset;
layout(location = 2) in vec3 rotation;

uniform mat4 projection = mat4(1.0);
uniform float scale = 5.0;

void main() {
    mat4 model = mat4(
        scale * rotation.x,  scale * rotation.y, 0.0,      0.0,
        -scale * rotation.y, scale * rotation.x, 0.0,      0.0,
        0.0,                 0.0,                1.0,      0.0,
        offset.x,            offset.y,           offset.z, 1.0
    );

//    float x = rotation.x;
//    float y = rotation.y;
//    float z = rotation.z;
//    mat4 model = mat4(
//        scale * (x * x),             scale * (y * z),             scale * (-z),       0.0,
//        scale * (x * y * z - y * z), scale * (y * y * z + x * z), scale * (x * y),    0.0,
//        scale * (x * z * z + y * y), scale * (y * z * z - x * y), scale * (x * z),    0.0,
//        offset.x,                    offset.y,                    offset.z,           1.0
//    );

    gl_Position = projection * model * position;
}