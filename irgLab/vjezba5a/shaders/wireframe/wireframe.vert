#version 460 core

layout (location = 0) in vec3 aPos;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;
uniform vec3 uColor;

out vec3 vColor;

void main() {
    vColor = uColor;
	gl_Position = uProj * uView * uModel * vec4(aPos, 1.0);
}