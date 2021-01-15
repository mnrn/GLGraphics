/**
 * @brief カスケードシャドウマッピング
 */

#include "CSM.h"

#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <spdlog/spdlog.h>

#ifdef WIN32
#ifdef far
#undef far
#endif
#ifdef near
#undef near
#endif
#endif

// ********************************************************************************
// Calculation
// ********************************************************************************

/**
 * @brief Practical Split Scheme
 * @ref https://developer.nvidia.com/gpugems/GPUGems3/gpugems3_ch10.html
 */
std::vector<float> CSM::ComputeSplitPlanes(int cascades, float near, float far,
                                           float lambda) {
  std::vector<float> splits(cascades + 1);
  splits[0] = near;
  splits[cascades] = far;
  for (int i = 1; i < cascades; i++) {
    const float i_m = static_cast<float>(i) / static_cast<float>(cascades);
    const float cilog = near * std::powf(far / near, i_m);
    const float ciuni = near + (far - near) * i_m;
    splits[i] = lambda * cilog + ciuni * (1.0f - lambda);
  }
  return splits;
}

void CSM::UpdateSplitPlanesUniform(
    int cascades, const std::vector<float> &splits, const Camera &camera,
    const std::function<void(int, float)> &loopEndCallback) {
  const glm::mat4 proj = camera.GetProjectionMatrix();

  // カメラから見た Split Planes の同次座標系におけるz位置を計算します。
  std::vector<float> clips(cascades);
  for (int i = 0; i < cascades; i++) {
    const float clip =
        0.5f * (-splits[i + 1] * proj[2][2] + proj[3][2]) / splits[i + 1] +
        0.5f;
    loopEndCallback(i, clip);
  }
}

void CSM::UpdateFrustums(int cascades, const std::vector<float> &splits,
                         const Camera &camera) {
  const float kAspectRatio = camera.GetAspectRatio();
  const float kCameraFOVY = camera.GetFOVY();

  frustums_.clear();
  frustums_.resize(cascades);
  for (int i = 0; i < cascades; i++) {
    const float near = splits[i];
    const float far =
        (i + 1 == cascades) ? splits[i + 1] : splits[i + 1] * 1.005f;
    // アーティファクトを避けるために0.2fを加算しています。
    frustums_[i].SetupPerspective(kCameraFOVY + 0.2f, kAspectRatio, near, far);
    frustums_[i].SetupCorners(camera.GetPosition(), camera.GetTarget(),
                              camera.GetUpVec());
  }
}

std::vector<glm::mat4> CSM::ComputeCropMatrices(int cascades,
                                                const glm::vec3 &lightDir,
                                                float shadowMapSize) {
  std::vector<glm::mat4> vpCrops(cascades);
  for (int i = 0; i < cascades; i++) {
    // 視錐台の境界球からAABBを計算します。
    const BSphere bs = frustums_[i].ComputeBSphere();
    const glm::vec3 radius3 = glm::vec3(bs.radius);
    const glm::vec3 maxi = radius3;
    const glm::vec3 mini = -radius3;
    const glm::vec3 extends = maxi - mini;

    // Crop Matrix の計算を行います。
    const auto kLightView = ComputeLightViewMatrix(lightDir, bs.center, maxi.z);
    const auto kLightProj =
        glm::ortho(mini.x, maxi.x, mini.y, maxi.y, 0.0f, extends.z);
    vpCrops[i] = ComputeCropMatrix(kLightView, kLightProj, shadowMapSize);
  }
  // 実際は crop * proj * view
  // となるような行列のvectorを返していることに注意してください。
  return vpCrops;
}

glm::mat4 CSM::ComputeLightViewMatrix(const glm::vec3 &lightDir,
                                      const glm::vec3 &center,
                                      float offsetZ) const {
  // ライトの位置をライトの方向に沿って offsetZ だけ押し戻します。
  const glm::vec3 kLightPt = center - glm::normalize(lightDir) * offsetZ;
  return glm::lookAt(kLightPt, center, glm::vec3(0.0f, 1.0f, 0.0f));
}

glm::mat4 CSM::ComputeCropMatrix(const glm::mat4 &view, const glm::mat4 &proj,
                                 float shadowMapSize) const {
  glm::vec4 shadowPt = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
  shadowPt = proj * view * shadowPt;
  shadowPt = shadowPt * shadowMapSize / 2.0f;

  glm::vec4 roundedPt = glm::round(shadowPt);
  glm::vec4 roundOffset = roundedPt - shadowPt;
  roundOffset = roundOffset * (2.0f / shadowMapSize);
  roundOffset.z = 0.0f;
  roundOffset.w = 0.0f;

  glm::mat4 shadow = proj;
  shadow[3][0] += roundOffset.x;
  shadow[3][1] += roundOffset.y;
  shadow[3][2] += roundOffset.z;
  shadow[3][3] += roundOffset.w;

  return shadow * view;
}
