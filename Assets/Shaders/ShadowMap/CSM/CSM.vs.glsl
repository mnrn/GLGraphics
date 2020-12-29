#version 410

const int kCascadesMax = 8;

layout (location=0) in vec3 VertexPosition;
layout (location=1) in vec3 VertexNormal;
layout (location=2) in vec2 VertexTexCoord;

out vec3 Position;
out vec3 Normal;
out vec4 ShadowCoords[kCascadesMax];

uniform int CascadesNum;
uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 MVP;
uniform mat4 ShadowMatrices[kCascadesMax];

void main() {
    Position = vec3(ModelViewMatrix * vec4(VertexPosition, 1.0));
    Normal = normalize(NormalMatrix * VertexNormal);
    for (int i = 0; i < CascadesNum; i++){
        ShadowCoords[i] = ShadowMatrices[i] * vec4(VertexPosition, 1.0);
    }
    gl_Position = MVP * vec4(VertexPosition, 1.0);
}
