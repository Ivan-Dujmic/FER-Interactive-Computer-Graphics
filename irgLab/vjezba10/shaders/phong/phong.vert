#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 vPos;
out vec3 vNormal;
out vec2 vTexCoords;
out vec4 vLightSpacePos;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;
uniform mat4 uLightView;
uniform mat4 uLightProjection;

void main() {
    vec4 worldPos = uModel * vec4(aPos, 1.0);

    gl_Position = uProjection * uView * worldPos;

    mat3 normalMatrix = mat3(transpose(inverse(uModel)));
    
    vPos = worldPos.xyz;
    vNormal = normalize(normalMatrix * aNormal); 
    vTexCoords = aTexCoords;
    vLightSpacePos = uLightProjection * uLightView * worldPos;
}