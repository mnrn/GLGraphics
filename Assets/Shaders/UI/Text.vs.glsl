#version 410

layout (location=0) in vec4 VertexTexCoord;

out vec2 TexCoord;

void main()
{
    TexCoord = VertexTexCoord.zw;
    gl_Position = vec4(VertexTexCoord.xy, 0.0, 1.0);
}
