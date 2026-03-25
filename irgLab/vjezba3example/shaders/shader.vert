#version 330 core
layout (location = 0) in vec3 aPos; // In VBO
layout (location = 1) in vec3 aCol; // In VBO

out vec3 color; // Pass color to shader.frag

uniform mat4 tMatrix; // Transformation matrix

void main() {
	color = aCol;
    gl_Position = tMatrix * vec4(aPos, 1.0); // Transform the vertex
}