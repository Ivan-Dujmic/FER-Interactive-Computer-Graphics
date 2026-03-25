#version 330 core
out vec4 FragColor;

in vec3 color; // Interpolated from shader.vert

void main() {
    FragColor = vec4(color, 0.0); // The added value is opacity
} 