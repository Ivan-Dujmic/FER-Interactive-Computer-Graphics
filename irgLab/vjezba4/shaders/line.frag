#version 460 core

in vec2 vPos;

out vec4 fColor;

uniform float uTime;

void main() {
    float r = vPos.x * 0.5 + 0.5;
    float g = vPos.y * 0.5 + 0.5;
    float b = sin(uTime) * 0.5 + 0.5;
    fColor = vec4(r, g, b, 1.0);
}