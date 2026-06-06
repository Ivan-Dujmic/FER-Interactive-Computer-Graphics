#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 vColor;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;
uniform vec3 eye;

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

uniform Light light;
uniform Material material;

void main() {
    vec3 worldPos = vec3(uModel * vec4(aPos, 1.0));

    vec3 N = normalize(mat3(transpose(inverse(uModel))) * aNormal);
    vec3 L = normalize(light.position - worldPos);
    vec3 V = normalize(eye - worldPos);
    vec3 R = reflect(-L, N);

    vec3 ambient = light.ambient * material.ambient;

    float diffuseFactor = max(dot(N, L), 0.0);
    vec3 diffuse = light.diffuse * material.diffuse * diffuseFactor;

    float specularFactor = 0.0;
    if (diffuseFactor > 0.0) {
        specularFactor = pow(max(dot(R, V), 0.0), material.shininess);
    }   
    vec3 specular = light.specular * material.specular * specularFactor;

    gl_Position = uProjection * uView * vec4(worldPos, 1.0);
    vColor = clamp(ambient + diffuse + specular, 0.0, 1.0);
}