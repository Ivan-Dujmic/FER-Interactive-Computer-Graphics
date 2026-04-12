#version 460 core

layout (location = 0) in vec3 aPos;

uniform vec3 uColor;

out vec3 vColor;

void main() {
    vColor = uColor;
	gl_Position = vec4(aPos, 1.0);
}