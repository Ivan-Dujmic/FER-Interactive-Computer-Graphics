#version 460 core

layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in vec4 vPos[];

uniform vec3 uColor;

out vec3 gColor;

void emitEdge(vec4 a, vec4 b) {
    gl_Position = a;
    gColor = uColor;
    EmitVertex();

    gl_Position = b;
    gColor = uColor;
    EmitVertex();

    EndPrimitive();
}

void main() {
    vec3 p0 = vPos[0].xyz / vPos[0].w;
    vec3 p1 = vPos[1].xyz / vPos[1].w;
    vec3 p2 = vPos[2].xyz / vPos[2].w;

    vec2 a = p1.xy - p0.xy;
    vec2 b = p2.xy - p0.xy;

    float winding = a.x * b.y - a.y * b.x;
    if (winding >= 0.0) {
        return;
    }

    emitEdge(vPos[0], vPos[1]);
    emitEdge(vPos[1], vPos[2]);
    emitEdge(vPos[2], vPos[0]);
}