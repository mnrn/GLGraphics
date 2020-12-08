#version 430

in vec3 Position;
in vec3 Normal;
in vec2 TexCoord;

layout (binding=0) uniform sampler2D PositionTex;
layout (binding=1) uniform sampler2D NormalTex;
layout (binding=2) uniform sampler2D ColorTex;
layout (binding=3) uniform sampler2D AOTex;
layout (binding=4) uniform sampler2D RandDirTex;
layout (binding=5) uniform sampler2D DiffTex;

layout (location=0) out vec4 FragColor;
layout (location=1) out vec3 PositionData;
layout (location=2) out vec3 NormalData;
layout (location=3) out vec3 ColorData;
layout (location=4) out float AOData;

const int kKernelSize = 64;
const vec2 kRandScale = vec2(1280.0 / 4.0, 720.0 / 4.0);
const float kGamma = 2.2;

uniform mat4 ProjectionMatrix;
uniform int Pass;
uniform vec3 SampleKernel[kKernelSize];
uniform float Radius = 0.55;

uniform struct LightInfo {
    vec4 Position;  // カメラ座標系におけるライトの位置
    vec3 L;         // Diffuse Light (拡散光)およびSpecular Light (鏡面反射光)の強さ
    vec3 La;        // Ambient Light (環境光)の強さ
} Light;

uniform struct MaterialInfo {
    vec3 Kd;        // Diffsue reflectivity (拡散光の反射係数)
    bool UseTex;    // テクスチャを使うかどうか
} Material;


vec3 GammaCorrection(vec3 color) {
    return pow(color, vec3(1.0 / kGamma));
}

vec3 AmbientDiffuseModel(vec3 pos, vec3 norm, vec3 diff, float ao) {
    vec3 amb = Light.La * diff * ao;
    vec3 dirToL = normalize(vec3(Light.Position) - pos);
    float NoL = max(dot(norm, dirToL), 0.0);
    //return vec3(ao);
    return amb + Light.L * diff * NoL;
}

void PackGBuffer() {
    // 位置情報、法線情報、色情報をGBufferに詰め込みます。
    PositionData = Position;
    NormalData = normalize(Normal);
    if (Material.UseTex) {
        ColorData = pow(texture(DiffTex, TexCoord.xy).xyz, vec3(kGamma));
    } else {
        ColorData = Material.Kd;
    }
}

void ComputeSSAO() {
    // ランダムに接座標空間->カメラ座標空間変換行列を生成します。
    vec3 randDir = normalize(texture(RandDirTex, TexCoord.xy * kRandScale).xyz);
    vec3 n = normalize(texture(NormalTex, TexCoord.xy).xyz);
    vec3 bitang = cross(n, randDir);
    if (length(bitang) < 0.0001) {  // nとrandDirが平行であれば、nはx-y平面に存在します。
        bitang = cross(n, vec3(0, 0, 1));
    }
    bitang = normalize(bitang);
    vec3 tang = cross(bitang, n);
    mat3 toCamSpace = mat3(tang, bitang, n);

    // サンプリングを行い、AO(環境遮蔽)の係数値を計算します。
    float occ = 0.0;
    vec3 camPos = texture(PositionTex, TexCoord).xyz;
    for (int i = 0; i < kKernelSize; i++) {
        vec3 samplePos = camPos + Radius * (toCamSpace * SampleKernel[i]);

        // カメラ座標->クリッピング座標->正規化デバイス座標->テクスチャ座標
        vec4 p = ProjectionMatrix * vec4(samplePos, 1.0);
        p *= 1.0 / p.w;
        p.xyz = p.xyz * 0.5 + 0.5;

        // サンプル点と比較し、遮蔽されるようであれば環境遮蔽係数に加算します。
        float surfZ = texture(PositionTex, p.xy).z;
        float distZ = surfZ - camPos.z;
        if (distZ >= 0.0 && distZ <= Radius && surfZ > samplePos.z) {
            occ += 1.0;
        }
    }
    // normalized
    occ = occ / kKernelSize;
    AOData = 1.0 - occ;
}

void Blur() {
    ivec2 pix = ivec2(gl_FragCoord.xy);
    float acc = 0.0;

    acc += texelFetchOffset(AOTex, pix, 0, ivec2(-1, -1)).r;
    acc += texelFetchOffset(AOTex, pix, 0, ivec2(-1, -0)).r;
    acc += texelFetchOffset(AOTex, pix, 0, ivec2(-1, 1)).r;
    acc += texelFetchOffset(AOTex, pix, 0, ivec2(0, -1)).r;
    acc += texelFetchOffset(AOTex, pix, 0, ivec2(0, 0)).r;
    acc += texelFetchOffset(AOTex, pix, 0, ivec2(0, 1)).r;
    acc += texelFetchOffset(AOTex, pix, 0, ivec2(1, -1)).r;
    acc += texelFetchOffset(AOTex, pix, 0, ivec2(1, 0)).r;
    acc += texelFetchOffset(AOTex, pix, 0, ivec2(1, 1)).r;
    
    // normalized
    float ao = acc * (1.0 / 9.0);
    AOData = ao;
}

void Visualize() {
    // テクスチャから情報を取り出します。
    vec3 pos = texture(PositionTex, TexCoord).xyz;
    vec3 norm = texture(NormalTex, TexCoord).xyz;
    vec3 diff = texture(ColorTex, TexCoord).rgb;
    float ao = texture(AOTex, TexCoord).r;

    // AOのパラメータ化
    ao = ao * ao * ao * ao;

    vec3 color = AmbientDiffuseModel(pos, norm, diff, ao);
    color = GammaCorrection(color);
    FragColor = vec4(color, 1.0);
}

void main() {
    if (Pass == 1) {
        PackGBuffer();
    } else if (Pass == 2) {
        ComputeSSAO();
    } else if (Pass == 3) {
        Blur();
    } else if (Pass == 4) {
        Visualize();
    }
}
