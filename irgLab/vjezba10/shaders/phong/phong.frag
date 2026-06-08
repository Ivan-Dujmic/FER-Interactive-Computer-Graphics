#version 460 core

in vec3 vPos;
in vec3 vNormal;
in vec2 vTexCoords;
in vec4 vLightSpacePos;

out vec4 fColor;

uniform vec3 eye;
uniform sampler2D shadowMap;
uniform bool useShadowMap;

struct Light {
    vec3 position;
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float cutOff;
    float outerCutOff;
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

float calculateShadow(vec4 lightSpacePos, vec3 normal, vec3 lightDir) {
    if (!useShadowMap) return 0.0; // Not shadowed

    vec3 projCoords = lightSpacePos.xyz / lightSpacePos.w;
    projCoords = projCoords * 0.5 + 0.5; // Same range as UVCoords

    // If outside of light view then set to not shadowed
    if (projCoords.z > 1.0) return 0.0;
    if (projCoords.x < 0.0 || projCoords.x > 1.0 ||
        projCoords.y < 0.0 || projCoords.y > 1.0) return 0.0;

    float currentDepth = projCoords.z; // Compare against the first scan depth
    float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.001);

    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    float shadow = 0.0;
    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            float closestDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > closestDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    return shadow;
}

void main() {
    vec3 N = normalize(vNormal);
    vec3 L = normalize(light.position - vPos);
    vec3 V = normalize(eye - vPos);
    vec3 R = reflect(-L, N);

    vec3 diffuseBase = material.diffuse;    

    if (material.hasDiffuseTexture) {
        diffuseBase = texture(material.diffuseTexture, vTexCoords).rgb;
    }

    vec3 ambient = light.ambient * material.ambient * diffuseBase;

    float theta = dot(L, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float spotlightIntensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    float diffuseFactor = max(dot(N, L), 0.0);
    vec3 diffuse = light.diffuse * diffuseBase * diffuseFactor;

    float specularFactor = 0.0;
    if (diffuseFactor > 0.0) {
        specularFactor = pow(max(dot(R, V), 0.0), material.shininess);
    }   
    vec3 specular = light.specular * material.specular * specularFactor;

    float shadow = calculateShadow(vLightSpacePos, N, L);
    vec3 color = ambient + (1.0 - shadow) * spotlightIntensity * (diffuse + specular);

    fColor = vec4(color, 1.0f);
}