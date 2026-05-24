#version 460 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vec3 vPos[];

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

out vec3 gColor;

vec3 calcPhong(vec3 position, vec3 normal) {
    vec3 N = normalize(normal);
    vec3 L = normalize(light.position - position);
    vec3 V = normalize(eye - position);
    vec3 R = reflect(-L, N);

    vec3 ambient = light.ambient * material.ambient;

    float diffuseFactor = max(dot(N, L), 0.0);
    vec3 diffuse = light.diffuse * material.diffuse * diffuseFactor;

    float specularFactor = 0.0;
    if (diffuseFactor > 0.0) {
        specularFactor = pow(max(dot(R, V), 0.0), material.shininess);
    }   
    vec3 specular = light.specular * material.specular * specularFactor;

    return clamp(ambient + diffuse + specular, 0.0, 1.0);
}

void main() {
    vec3 p0 = vec3(uModel * vec4(vPos[0], 1.0));
    vec3 p1 = vec3(uModel * vec4(vPos[1], 1.0));
    vec3 p2 = vec3(uModel * vec4(vPos[2], 1.0));

    vec3 center = (p0 + p1 + p2) / 3.0;
    vec3 normal = normalize(cross(p2 - p0, p1 - p0));

    gColor = calcPhong(center, normal);

    gl_Position = uProjection * uView * vec4(p0, 1.0);
    EmitVertex();

    gl_Position = uProjection * uView * vec4(p1, 1.0);
    EmitVertex();

    gl_Position = uProjection * uView * vec4(p2, 1.0);
    EmitVertex();

    EndPrimitive();
}