#version 460 core

layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in vec3 vPos[];

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;
uniform vec3 eye;
uniform vec3 uColor;

out vec3 gColor;

void emitEdge(int i, int j) {
    gl_Position = uProjection * uView * uModel * vec4(vPos[i], 1.0);
    gColor = uColor;
    EmitVertex();

    gl_Position = uProjection * uView * uModel * vec4(vPos[j], 1.0);
    gColor = uColor;
    EmitVertex();

    EndPrimitive();
}

void main() {
    vec3 p0 = vec3(uModel * vec4(vPos[0], 1.0));
    vec3 p1 = vec3(uModel * vec4(vPos[1], 1.0));
    vec3 p2 = vec3(uModel * vec4(vPos[2], 1.0));

    vec3 n = normalize(cross(p1 - p0, p2 - p0));
    float d = -dot(n, p0);
    float side = dot(n, eye) + d;

    if (side >= 0.0) {
        return;
    }

    emitEdge(0, 1);
    emitEdge(1, 2);
    emitEdge(2, 0); 
}