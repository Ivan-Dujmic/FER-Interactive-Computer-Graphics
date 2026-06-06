#version 460 core

in vec3 vPos;
in vec3 vNormal;
in vec2 vTexCoords;

out vec4 fColor;

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

    bool hasDiffuseTexture;
    sampler2D diffuseTexture;
};

uniform Light light;
uniform Material material;

void main() {
    vec3 N = normalize(vNormal);
    vec3 L = normalize(light.position - vPos);
    vec3 V = normalize(eye - vPos);
    vec3 R = reflect(-L, N);

    vec3 diffuseBase = material.diffuse;    

    if (material.hasDiffuseTexture) {
        diffuseBase = texture(material.diffuseTexture, vTexCoords).rgb;
    }

    vec3 ambient = light.ambient * material.ambient;

    float diffuseFactor = max(dot(N, L), 0.0);
    vec3 diffuse = light.diffuse * diffuseBase * diffuseFactor;

    float specularFactor = 0.0;
    if (diffuseFactor > 0.0) {
        specularFactor = pow(max(dot(R, V), 0.0), material.shininess);
    }   
    vec3 specular = light.specular * material.specular * specularFactor;

    vec3 color = ambient + diffuse + specular;

    fColor = vec4(color, 1.0f);
}