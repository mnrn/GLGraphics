#version 410

const float kPI = 3.14159265358979323846264;
const float kGamma = 2.2;
const int kLightMax = 4;

in vec3 Position;
in vec3 Normal;

layout (location=0) out vec4 FragColor;

uniform struct LightInfo {
    vec4 Position;  // カメラ座標系におけるライトの位置
    vec3 L;         // ライトの強さ
} Light[kLightMax];

uniform struct MaterialInfo {
    float Roughness;    // 粗さ
    float Metallic;     // 金属かどうか 1.0=metal(導体,金属), 0.0=dielectric(誘電体, 非金属)
    float Reflectance;  // 誘電体(Dielectric)の拡散反射率
    vec3 BaseColor;     // 誘電体(Dielectric)のDiffuse色(Albedo)もしくは金属(Metal)のSpecular色
} Material;

// ライトの数
uniform int LightNum = 3;
uniform float RoughnessParameterization = 2.0f;

/**
 * @brief The GGX distribution (GGX分布関数)
 */
float D_GGX(float NoH, float roughness) {
    float a2 = roughness * roughness;
    float f = (NoH * a2 - NoH) * NoH + 1.0;
    return a2 / (kPI * f * f);
}

/**
 * @brief The Smith geometric shadowing function (を簡単にした関数です。)
 */
float V_SmithFast(float NoV, float NoL, float roughness) {
    float GGXV = NoL * (NoV * (1.0 - roughness) + roughness);
    float GGXL = NoV * (NoL * (1.0 - roughness) + roughness);
    return 0.5 / (GGXV * GGXL);
}

/**
 * @brief The Schlick approximation for the Fresnel term(フレネル項のSchlick近似)
 */
vec3 F_Schlick(float u, vec3 f0) {
    return f0 + (vec3(1.0) - f0) * pow(1.0 - u, 5.0);
}

vec3 GammaCorrection(vec3 color) {
    return pow(color, vec3(1.0 / kGamma));
}

vec3 MicroFacetModel(int lightIdx, vec3 pos, vec3 n) {
    // 誘電体(非金属)ならDiffuse色(Albedo)取得
    vec3 diff = (1.0 - Material.Metallic) * Material.BaseColor;

    // 金属(導体)ならSpecular色取得
    vec3 f0 = 0.16 * Material.Reflectance * Material.Reflectance * (1.0 - Material.Metallic) + Material.BaseColor * Material.Metallic;

    // ライトに関して。
    vec3 l = vec3(0.0);
    vec3 lightIntensity = Light[lightIdx].L;
    if (Light[lightIdx].Position.w == 0.0) {    // Directional Lightの場合
        l = normalize(Light[lightIdx].Position.xyz);
    } else {                                    // Positional Lightの場合 
        l = Light[lightIdx].Position.xyz - pos;
        float dist = length(l);
        l = normalize(l);
        lightIntensity /= (dist * dist);
    }

    vec3 v = normalize(-pos);   // 視線ベクトル
    vec3 h = normalize(v + l);  // ハーフベクトル(Bling-Phongモデルと同じ)
    float NoV = dot(n, v);      // abs(dot(n, v)) + 1e-5
    float NoL = dot(n, l);      // clamp(dot(n, l), 0.0, 1.0)
    float NoH = dot(n, h);      // clamp(dot(n, h), 0.0, 1.0)
    float LoH = dot(l, h);      // clamp(dot(l, h), 0.0, 1.0)

    // ラフネスをパラメタ化します。
    float roughness = pow(Material.Roughness, RoughnessParameterization);

    // Specular BRDF
    float D = D_GGX(NoH, roughness);
    vec3 F = F_Schlick(LoH, f0);
    float V = V_SmithFast(NoV, NoL, roughness);
    vec3 spec = D * V * F;

    return (diff + kPI * spec) * lightIntensity * NoL;
}

void main() {
    vec3 color = vec3(0.0);
    vec3 n = normalize(Normal);

    for (int i =0; i < LightNum; i++) {
        color += MicroFacetModel(i, Position, n);
    }

    FragColor = vec4(GammaCorrection(color), 1.0);
}
