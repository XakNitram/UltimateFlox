#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec3 offset;
layout(location = 2) in vec3 rotation;

uniform mat4 projection = mat4(1.0);
uniform float scale = 5.0;

vec4 rotate(vec3 rot, vec3 vec) {
    const float b2 = rot.x * rot.x + rot.y * rot.y + rot.z * rot.z;
    return (vec * (1.0 - b2) + rot * (dot(vec, rot) * 2.0) + cross(rot, vec) * (1.0 * 2.0));
}

void main() {

    // Non-rotating model matrix can remain, but rotations in 3D need to be done with quaternion calculations.
    const mat4 model = mat4(
        scale, 0.0, 0.0, 0.0,
        0.0, scale, 0.0, 0.0,
        0.0, 0.0, scale, 0.0,
        offset.x, offset.y, offset.z, 1.0
    );

    gl_Position = projection * model * fullRotation * position;
}