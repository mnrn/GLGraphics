#version 410

layout (location = 0) in vec2 vertexPosition;

uniform mat4 MVP;

void main() {
  gl_Position = MVP * vec4(vertexPosition, 0.0, 1.0);
}
