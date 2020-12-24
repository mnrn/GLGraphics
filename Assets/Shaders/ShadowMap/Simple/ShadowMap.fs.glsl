#version 410

in vec3 Position;
in vec3 Normal;
in vec4 ShadowCoord;

layout(location=0) out vec4 FragColor;

const float kGamma = 2.2;

uniform struct LightInfo {
    vec4 Position;  // カメラ座標系から見たライトの位置
    vec3 La;        // Ambient Light (環境光)の強さ
    vec3 Ld;        // Diffuse Light (拡散光)の強さ
    vec3 Ls;        // Specular Light (鏡面反射光)の強さ 
} Light;

uniform struct MaterialInfo {
    vec3 Ka;  // Ambient reflectivity (環境光の反射係数)
    vec3 Kd;  // Diffsue reflectivity (拡散光の反射係数)
    vec3 Ks;  // Specular reflectivity (鏡面反射光の反射係数)
    float Shininess;  // Specular shininess factor (鏡面反射の強さの係数)
} Material;

uniform sampler2D ShadowMap;

vec4 GammaCorrection(vec4 color) {
    return pow(color, vec4(1.0 / kGamma));
}

vec3 PhongDSModel(vec3 pos, vec3 n) {
    // Phong Shading Equation
    vec3 s = normalize(vec3(Light.Position.xyz - pos));
    float sDotN = max(dot(s, n), 0.0);
    vec3 diff = Light.Ld * Material.Kd  * sDotN;
    vec3 spec = vec3(0.0);
    if (sDotN > 0.0) {
        vec3 v = normalize(-pos.xyz);
        vec3 r = reflect(-s, n);
        spec = Light.Ls * Material.Ks * pow(max(dot(r, v), 0.0), Material.Shininess);
    }
    return diff + spec;
}

float ComputeShadow() {
    float bias = 0.005;
    float z = (ShadowCoord.z - bias) / ShadowCoord.w;
    float depth = texture(ShadowMap, ShadowCoord.xy / ShadowCoord.w).r;
    if (depth < z) {
        return 0.1;
    } else {
        return 1.0;
    }
}

void ShadeWithShadow() {
    vec3 amb = Light.La * Material.Ka;
    vec3 diffSpec = PhongDSModel(Position, Normal);

    float shadow = ComputeShadow();

    // ピクセルが影の中にある場合、Ambient Light (環境光)のみ使用することになります。
    vec4 color = vec4(diffSpec * shadow + amb, 1.0);
    FragColor = GammaCorrection(color);
}

void main() {
    ShadeWithShadow();
}
