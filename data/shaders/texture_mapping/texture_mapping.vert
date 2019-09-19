#version 430

layout (location=0) in vec3 inVertexPositon;
layout (location=1) in vec4 inVertexColor;
layout (location=2) in vec2 inTextureCoord;

out vec4 outColor;
out vec2 outTextureCoord;


void main()
{
    // 色およびテクスチャの情報をそのままフラグメントシェーダーへ送る
    outColor        = inVertexColor;
    outTextureCoord = inTextureCoord;

    // とりあえず変換なしで位置情報出力
    gl_Position = vec4(inVertexPositon, 1.0);
}
