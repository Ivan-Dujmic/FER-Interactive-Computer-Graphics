#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 vPos;
out vec3 vNormal;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

void main() {
    vec3 worldPos = vec3(uModel * vec4(aPos, 1.0));

    gl_Position = uProjection * uView * vec4(worldPos, 1.0);

    mat3 normalMatrix = mat3(transpose(inverse(uModel)));
    
    vPos = worldPos;
    vNormal = normalize(normalMatrix * aNormal); 
}