#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;

out vec4 vertexColor;

uniform mat4 uMVP;
uniform float uTime;
uniform float uAnimSpeed;

// per-object animation flags
uniform bool uPyAnim;
uniform bool uGridAnim;

// which object is this? 0 = pyramid, 1 = grid
uniform int uObjectID;

// rotation toggles
uniform bool uRotateAnimX;
uniform bool uRotateAnimY;
uniform bool uRotateAnimZ;

void main()
{
    float angle = uTime * uAnimSpeed;

    mat4 rotationX = mat4(
        1.0, 0.0,         0.0,         0.0,
        0.0, cos(angle), -sin(angle),  0.0,
        0.0, sin(angle),  cos(angle),  0.0,
        0.0, 0.0,         0.0,         1.0
    );

    mat4 rotationY = mat4(
        cos(angle),  0.0, sin(angle), 0.0,
        0.0,         1.0, 0.0,        0.0,
        -sin(angle), 0.0, cos(angle), 0.0,
        0.0,         0.0, 0.0,        1.0
    );

    mat4 rotationZ = mat4(
        cos(angle), -sin(angle), 0.0, 0.0,
        sin(angle),  cos(angle), 0.0, 0.0,
        0.0,         0.0,        1.0, 0.0,
        0.0,         0.0,        0.0, 1.0
    );

    mat4 rot = mat4(1.0f);

    bool anim = false;
    if (uObjectID == 0) {
        anim = uPyAnim;
    } else if (uObjectID == 1) {
        anim = uGridAnim;
    }

    if (anim && uRotateAnimX) {
        rot = rotationX * rot;
    }

    if (anim && uRotateAnimY) {
        rot = rotationY * rot;
    }

    if (anim && uRotateAnimZ) {
        rot = rotationZ * rot;
    }

    gl_Position = uMVP * rot * vec4(aPos, 1.0);
    vertexColor = aColor;
}
