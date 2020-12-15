#version 410

in vec2 TexCoord;

layout (location=0) out vec4 FragColor;

uniform sampler2D Tex;
uniform vec4 Color;

void main()
{
    FragColor = vec4(1.0, 1.0, 1.0, texture(Tex, TexCoord).a) * Color;
}
