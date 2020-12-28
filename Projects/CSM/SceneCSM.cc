/**
 * @brief Cascaded shadow mapping のテストシーン
 */

#include "SceneCSM.h"

#include <boost/assert.hpp>
#include <cmath>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

#define FMT_HEADER_ONLY
#include <fmt/format.h>

#include "HID/KeyInput.h"
#include "UI/Font.h"
#include "UI/Text.h"

#ifdef far
#undef far
#endif
#ifdef near
#undef near
#endif

// ********************************************************************************
// constexpr variables
// ********************************************************************************

static constexpr int kCascadedNum = 3;
static constexpr int kSplitPlanesNum = kCascadedNum + 1;

static constexpr float kLambda = 0.5f;

static constexpr float kCameraFOVY = 50.0f;
static constexpr float kCameraNear = 0.3f;
static constexpr float kCameraFar = 8.0f;

static constexpr float kCameraHeight = 1.5f;
static constexpr float kCameraRadius = 2.25f;
static constexpr float kCameraDefaultAngle = glm::two_pi<float>() * 0.85f;

static constexpr glm::vec3 kLightDefaultPosition{-2.0f, 2.0f, -2.0f};
static constexpr glm::vec3 kLightDefaultTarget{0.0f};
static constexpr glm::vec3 kLightDefaultDir =
    kLightDefaultTarget - kLightDefaultPosition;

static constexpr float kRotSpeed = 0.5f;
static constexpr int kShadowMapSize = 2048;
static constexpr int kShadowMapWidth = kShadowMapSize;
static constexpr int kShadowMapHeight = kShadowMapSize;

static constexpr glm::vec3 kLightColor{0.85f};
static constexpr glm::mat4 kShadowBias{0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.5f,
                                       0.0f, 0.0f, 0.0f, 0.0f, 0.5f, 0.0f,
                                       0.5f, 0.5f, 0.5f, 1.0f};

// ********************************************************************************
// Override functions
// ********************************************************************************

void SceneCSM::OnInit() {
  SetupCamera();
  SetupLight();

  if (const auto msg = CompileAndLinkShader()) {
    std::cerr << msg.value() << std::endl;
    BOOST_ASSERT_MSG(false, "failed to compile or link!");
  } else {
    progs_[kShadeWithShadow].Use();
    progs_[kShadeWithShadow].SetUniform("Light.La", kLightColor);
    progs_[kShadeWithShadow].SetUniform("Light.Ld", kLightColor);
    progs_[kShadeWithShadow].SetUniform("Light.Ls", kLightColor);
    progs_[kShadeWithShadow].SetUniform("ShadowMaps", 0);
  }

  // CSM用のFBOの初期化を行います。
  if (!csmFBO_.OnInit(kCascadedNum, kShadowMapWidth, kShadowMapHeight)) {
    BOOST_ASSERT_MSG(false, "Framebuffer is not complete.");
  }
}

void SceneCSM::OnDestroy() { spdlog::drop_all(); }

void SceneCSM::OnUpdate(float t) {
  const float deltaT = tPrev_ == 0.0f ? 0.0f : t - tPrev_;
  tPrev_ = t;

  angle_ += kRotSpeed * deltaT;
  if (angle_ > glm::two_pi<float>()) {
    angle_ -= glm::two_pi<float>();
  }

  const glm::vec3 kCamPt = glm::vec3(kCameraRadius * cos(angle_), kCameraHeight,
                                     kCameraRadius * sin(angle_));
  camera_.SetPosition(kCamPt);
}

void SceneCSM::OnRender() {
  OnPreRender();

  glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D_ARRAY, csmFBO_.GetDepthTextureArray());
  {
    Pass1();
    Pass2();
  }
  glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

  glDisable(GL_CULL_FACE);
  glDisable(GL_DEPTH_TEST);

  DrawStatus();
}

void SceneCSM::OnResize(int w, int h) {
  SetDimensions(w, h);
  glViewport(0, 0, w, h);
}

void SceneCSM::OnPreRender() {
  const auto splits = ComputeSplitPlanes(kCascadedNum, kCameraNear, kCameraFar);
  UpdateSplitPlanesUniform(kCascadedNum, splits);
  UpdateFrustums(kCascadedNum, splits);
  UpdateCropMatrices(kCascadedNum);
}

// ********************************************************************************
// Shader settings
// ********************************************************************************

std::optional<std::string> SceneCSM::CompileAndLinkShader() {
  // compile and links
  if (const auto msg = progs_[kRecordDepth].CompileAndLink(
          {{"./Assets/Shaders/ShadowMap/RecordDepth.vs.glsl",
            ShaderType::Vertex},
           {"./Assets/Shaders/ShadowMap/RecordDepth.fs.glsl",
            ShaderType::Fragment}})) {
    return msg;
  }
  if (const auto msg = progs_[kShadeWithShadow].CompileAndLink(
          {{"./Assets/Shaders/ShadowMap/CSM/CSM.vs.glsl", ShaderType::Vertex},
           {"./Assets/Shaders/ShadowMap/CSM/CSM.fs.glsl",
            ShaderType::Fragment}})) {
    return msg;
  }
  return std::nullopt;
}

void SceneCSM::SetMatrices() {
  if (pass_ == kRecordDepth) {
    const glm::mat4 mvp = vpCrop_[cascadeIdx_] * model_;
    progs_[kRecordDepth].SetUniform("MVP", mvp);
  } else if (pass_ == kShadeWithShadow) {
    const glm::mat4 mv = view_ * model_;
    progs_[kShadeWithShadow].SetUniform("ModelViewMatrix", mv);
    progs_[kShadeWithShadow].SetUniform("NormalMatrix", glm::mat3(mv));

    const glm::mat4 mvp = proj_ * mv;
    progs_[kShadeWithShadow].SetUniform("MVP", mvp);

    for (int i = 0; i < kCascadedNum; i++) {
      const glm::mat4 kLightMVP = kShadowBias * vpCrop_[i] * model_;
      const std::string kUniLiMVP = fmt::format("ShadowMatrices[{}]", i);
      progs_[kShadeWithShadow].SetUniform(kUniLiMVP.c_str(), kLightMVP);
    }
  }
}

void SceneCSM::SetMatrialUniforms(const glm::vec3 &diff, const glm::vec3 &amb,
                                  const glm::vec3 &spec, float shininess) {

  if (pass_ != kShadeWithShadow) {
    return;
  }
  progs_[kShadeWithShadow].SetUniform("Material.Ka", amb);
  progs_[kShadeWithShadow].SetUniform("Material.Kd", diff);
  progs_[kShadeWithShadow].SetUniform("Material.Ks", spec);
  progs_[kShadeWithShadow].SetUniform("Material.Shininess", shininess);
}

// ********************************************************************************
// Drawing
// ********************************************************************************

// Shadow map generation
void SceneCSM::Pass1() {
  pass_ = RenderPass::kRecordDepth;

  glViewport(0, 0, kShadowMapWidth, kShadowMapHeight);

  glCullFace(GL_FRONT);
  glEnable(GL_POLYGON_OFFSET_FILL);
  glPolygonOffset(1.0f, 4096.0f);

  glBindFramebuffer(GL_FRAMEBUFFER, csmFBO_.GetShadowFBO());
  progs_[kRecordDepth].Use();
  for (int i = 0; i < kCascadedNum; i++) {
    cascadeIdx_ = i;
    glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                              csmFBO_.GetDepthTextureArray(), 0, i);
    glClear(GL_DEPTH_BUFFER_BIT);

    // ライトから見たシーンの描画
    DrawScene();
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glDisable(GL_POLYGON_OFFSET_FILL);
  glCullFace(GL_BACK);
}

// render
void SceneCSM::Pass2() {
  pass_ = RenderPass::kShadeWithShadow;

  proj_ = camera_.GetProjectionMatrix();
  view_ = camera_.GetViewMatrix();
  progs_[kShadeWithShadow].Use();
  progs_[kShadeWithShadow].SetUniform(
      "Light.Position", view_ * glm::vec4(kLightDefaultPosition, 1.0f));

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0, 0, width_, height_);

  DrawScene();
}

void SceneCSM::DrawScene() {
  const glm::vec3 diff = glm::vec3(1.0f, 0.85f, 0.55f);
  const glm::vec3 amb = diff * 0.1f;
  const glm::vec3 spec = glm::vec3(0.0f);

  // 建物の描画
  SetMatrialUniforms(diff, amb, spec, 1.0f);
  model_ = glm::mat4(1.0f);
  SetMatrices();
  building_->Render();

  // 平面の描画
  SetMatrialUniforms(glm::vec3(0.25f, 0.25f, 0.25f),
                     glm::vec3(0.0f, 0.0f, 0.0f),
                     glm::vec3(0.05f, 0.05f, 0.05f), 1.0f);
  model_ = glm::mat4(1.0f);
  SetMatrices();
  plane_.Render();
}

void SceneCSM::DrawStatus() {}

// ********************************************************************************
// View
// ********************************************************************************

void SceneCSM::SetupCamera() {
  angle_ = kCameraDefaultAngle;
  const glm::vec3 kCamPt = glm::vec3(kCameraRadius * cos(angle_), kCameraHeight,
                                     kCameraRadius * sin(angle_));
  camera_.SetupOrient(kCamPt, glm::vec3(0.0f, 0.0f, 0.0f),
                      glm::vec3(0.0f, 1.0f, 0.0f));
  camera_.SetupPerspective(glm::radians(kCameraFOVY),
                           static_cast<float>(width_) /
                               static_cast<float>(height_),
                           kCameraNear, kCameraFar);
}

void SceneCSM::SetupLight() {
  lightView_.SetupOrient(kLightDefaultPosition, glm::vec3(0.0f),
                         glm::vec3(-1.0f, 0.0f, 0.0f));
}

// ********************************************************************************
// Calculation
// ********************************************************************************

std::vector<float> SceneCSM::ComputeSplitPlanes(int cascades, float near,
                                                float far) {
  if (cascades < 1) {
    cascades = 1;
  }

  std::vector<float> splits(cascades + 1);
  splits[0] = near;
  splits[cascades] = far;
  for (int i = 1; i < cascades; i++) {
    const float cilog =
        near * std::powf(far / near,
                         static_cast<float>(i) / static_cast<float>(cascades));
    const float ciuni = near + (far - near) * static_cast<float>(i) / cascades;
    splits[i] = kLambda * cilog + ciuni * (1.0f - kLambda);
  }
  return splits;
}

void SceneCSM::UpdateSplitPlanesUniform(int cascades,
                                        const std::vector<float> splits) {
  progs_[kShadeWithShadow].Use();
  // カメラから見た Split Planes の同次座標系におけるz位置を計算します。
  for (int i = 0; i < cascades; i++) {
    const glm::mat4 proj = camera_.GetProjectionMatrix();
    const float clip =
        0.5f * (-splits[i + 1] * proj[2][2] + proj[3][2]) / splits[i + 1] +
        0.5f;
    const std::string plane =
        fmt::format("CameraHomogeneousSplitPlanes[{}]", i);
    progs_[kShadeWithShadow].SetUniform(plane.c_str(), clip);
  }
}

void SceneCSM::UpdateFrustums(int cascades, const std::vector<float> splits) {
  const float kAspectRatio =
      static_cast<float>(width_) / static_cast<float>(height_);

  cascadedFrustums_.clear();
  cascadedFrustums_.resize(cascades);
  for (int i = 0; i < cascades; i++) {
    const float far =
        (i + 1 != cascades) ? splits[i + 1] : splits[i + 1] * 1.005f;
    cascadedFrustums_[i].SetupPerspective(kCameraFOVY / 57.2957795f + 0.2f,
                                          kAspectRatio, splits[i], far);
    cascadedFrustums_[i].SetupCorners(camera_.GetPosition(),
                                      camera_.GetTarget(), camera_.GetUpVec());
  }
}

void SceneCSM::UpdateCropMatrices(int cascades) {
  vpCrop_.clear();
  vpCrop_.resize(cascades);
  for (int i = 0; i < cascades; i++) {
    const BSphere bs = cascadedFrustums_[i].GetSphere();
    const glm::vec3 radius3 = glm::vec3(bs.radius);
    const glm::vec3 maxi = radius3;
    const glm::vec3 mini = -radius3;
    const glm::vec3 extends = maxi - mini;

    const auto kLightView =
        ComputeLightViewMatrix(kLightDefaultDir, bs.center, mini.z);
#if true
    const auto kLightProj =
        glm::ortho(mini.x, maxi.x, mini.y, maxi.y, 0.0f, extends.z);
#else
    const auto kLightProj =
        glm::perspective(glm::radians(kLightFOVY), 1.0f, kLightNear, kLightFar);
#endif

    // Crop Matrix の計算を行います。
    vpCrop_[i] = ComputeCropMatrix(kLightView, kLightProj);
  }
}

glm::mat4 SceneCSM::ComputeCropMatrix(const glm::vec3 &mini,
                                      const glm::vec3 &maxi) const {
  const float sx = 2.0f / (maxi.x - mini.x);
  const float sy = 2.0f / (maxi.y - mini.y);
  const float ox = -0.5f * (maxi.x + mini.x) * sx;
  const float oy = -0.5f * (maxi.y + mini.y) * sy;
#if true
  return glm::mat4(
      glm::vec4(sx, 0.0f, 0.0f, 0.0f), glm::vec4(0.0f, sy, 0.0f, 0.0f),
      glm::vec4(0.0f, 0.0f, 1.0f, 0.0f), glm::vec4(ox, oy, 0.0f, 1.0f));
#else
  const float sz = 1.0f / (maxi.z - mini.z);
  const float oz = -mini.z * sz;
  return glm::mat4(
      glm::vec4(sx, 0.0f, 0.0f, 0.0f), glm::vec4(0.0f, sy, 0.0f, 0.0f),
      glm::vec4(0.0f, 0.0f, sz, 0.0f), glm::vec4(ox, oy, oz, 1.0f));
#endif
}

std::pair<float, float> SceneCSM::FindZRange(const Frustum &frustum,
                                             const glm::mat4 &mv) const {
  // ライトから見た錐台のzの範囲を見つけます。
  glm::vec4 trans = mv * glm::vec4(frustum.GetCorner(0), 1.0f);
  float minZ = trans.z;
  float maxZ = trans.z;

  for (int i = 1; i < 8; i++) {
    trans = mv * glm::vec4(frustum.GetCorner(i), 1.0f);
    if (trans.z > maxZ) {
      maxZ = trans.z;
    }
    if (trans.z < minZ) {
      minZ = trans.z;
    }
  }
  return std::make_pair(minZ, maxZ);
}

std::pair<glm::vec2, glm::vec2>
SceneCSM::FindExtendsProj(const Frustum &frustum, const glm::mat4 &mvp) const {
  float maxX = std::numeric_limits<float>::lowest();
  float maxY = std::numeric_limits<float>::lowest();
  float minX = std::numeric_limits<float>::max();
  float minY = std::numeric_limits<float>::max();

  // ライトの同次座標系に投影された錐台の延長を見つけます。
  for (int i = 0; i < 8; i++) {
    const auto corner = glm::vec4(frustum.GetCorner(i), 1.0f);
    const auto trans = mvp * corner;

    const auto tX = trans.x / trans.w;
    const auto tY = trans.y / trans.w;

    if (tX > maxX) {
      maxX = tX;
    }
    if (tX < minX) {
      minX = tX;
    }
    if (tY > maxY) {
      maxY = tY;
    }
    if (tY < minY) {
      minY = tY;
    }
  }
#if true
  return std::make_pair(glm::vec2(minX, minY), glm::vec2(maxX, maxY));
#else
  return std::make_pair(glm::vec2(-1.0f, -1.0f), glm::vec2(1.0f, 1.0f));
#endif
}

glm::mat4 SceneCSM::ComputeLightViewMatrix(const glm::vec3 &lightDir,
                                           const glm::vec3 &center,
                                           float z) const {
  const glm::vec3 kLightDir = center - glm::normalize(lightDir) * -z;
  return glm::lookAt(kLightDir, center, glm::vec3(0.0f, 1.0f, 0.0f));
}

glm::mat4 SceneCSM::ComputeCropMatrix(const glm::mat4 &view,
                                      const glm::mat4 &proj) const {
  glm::vec4 shadowPt = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
  shadowPt = proj * view * shadowPt;
  shadowPt = shadowPt * static_cast<float>(kShadowMapSize) / 2.0f;

  glm::vec4 roundedPt = glm::round(shadowPt);
  glm::vec4 roundOffset = roundedPt - shadowPt;
  roundOffset = roundOffset * (2.0f / kShadowMapSize);
  roundOffset.z = 0.0f;
  roundOffset.w = 0.0f;

  glm::mat4 shadow = proj;
  shadow[3][0] += roundOffset.x;
  shadow[3][1] += roundOffset.y;
  shadow[3][2] += roundOffset.z;
  shadow[3][3] += roundOffset.w;

  return shadow * view;
}
