/**
 * @brief Cascaded shadow mapping のテストシーン
 */

#include "SceneCSM.h"

#include <boost/assert.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <cmath>

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
static constexpr float kCameraNear = 0.1f;
static constexpr float kCameraFar = 10.0f;

static constexpr float kLightFOVY = 40.0f;
static constexpr float kLightNear = 0.1f;
static constexpr float kLightFar = 10.0f;

static constexpr float kRotSpeed = 0.2f;
static constexpr int kShadowMapWidth = 1024;
static constexpr int kShadowMapHeight = 1024;

static constexpr glm::mat4 kShadowBias{0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.5f,
                                       0.0f, 0.0f, 0.0f, 0.0f, 0.5f, 0.0f,
                                       0.5f, 0.5f, 0.5f, 1.0f};

static constexpr float kCameraRadius = 1.8f;

static constexpr glm::vec3 kLightColor{0.85f};
static constexpr glm::vec3 kDefaultLightPosition{-2.5f, 2.0f, -2.5f};

// ********************************************************************************
// Override functions
// ********************************************************************************

void SceneCSM::OnInit() {
  KeyInput::Create();
  Text::Create();
  Font::Create();

  if ((fontObj_ = Font::Get().Entry(
           "./Assets/Fonts/UbuntuMono/UbuntuMono-Regular.ttf"))) {
    fontObj_->SetupWithSize(28);
  }

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
    for (int i = 0; i < kCascadedNum; i++) {
      std::string shadow = fmt::format("ShadowMaps[{}]", i);
      progs_[kShadeWithShadow].SetUniform(shadow.c_str(), i);
    }
  }

  // CSM用のFBOの初期化を行います。
  csmFBO_.OnInit(kCascadedNum, kShadowMapWidth, kShadowMapHeight);
}

void SceneCSM::OnDestroy() {
}

void SceneCSM::OnUpdate(float t) {
  const float deltaT = tPrev_ == 0.0f ? 0.0f : t - tPrev_;
  tPrev_ = t;

  angle_ += kRotSpeed * deltaT;
  if (angle_ > glm::two_pi<float>()) {
    angle_ -= glm::two_pi<float>();
  }

  const glm::vec3 kCamPt =
      glm::vec3(kCameraRadius * cos(angle_), 0.7f, kCameraRadius * sin(angle_));
  camera_.SetPosition(kCamPt);
}

void SceneCSM::OnRender() {
  OnPreRender();

  glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);

  {
    Pass1();
    Pass2();
  }
  glDisable(GL_CULL_FACE);
  glDisable(GL_DEPTH_TEST);

  DrawStatus();
}

void SceneCSM::OnResize(int w, int h) {
  SetDimensions(w, h);
  glViewport(0, 0, w, h);
}

void SceneCSM::OnPreRender() {
  UpdateSplitPlanes(kCascadedNum, kCameraNear, kCameraFar);
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
  const glm::mat4 mv = view_ * model_;
  if (pass_ == kRecordDepth) {
    progs_[kRecordDepth].SetUniform("MVP", proj_ * mv);
  } else if (pass_ == kShadeWithShadow) {
    progs_[kShadeWithShadow].SetUniform("ModelViewMatrix", mv);
    progs_[kShadeWithShadow].SetUniform("NormalMatrix", glm::mat3(mv));

    const glm::mat4 mvp = proj_ * mv;
    progs_[kShadeWithShadow].SetUniform("MVP", mvp);

    std::string lightMVP = fmt::format("LightMVP[{}]", cascadeIdx_);
    progs_[kShadeWithShadow].SetUniform(lightMVP.c_str(), lightPV_ * model_);
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
  glPolygonOffset(2.5f, 10.0f);

  view_ = lightView_.GetViewMatrix();
  proj_ = lightView_.GetProjectionMatrix();

  progs_[kRecordDepth].Use();
  for (int i = 0; i < kSplitPlanesNum; i++) {
    cascadeIdx_ = i;
    // TODO: ライトの射影変換行列の計算

    // ライトから見たシーンの描画
    glClear(GL_DEPTH_BUFFER_BIT);
    csmFBO_.BindForWriting(i);
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
      "Light.Position", view_ * glm::vec4(lightView_.GetPosition(), 1.0f));

  //glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0, 0, width_, height_);

  csmFBO_.BindForReading({GL_TEXTURE0, GL_TEXTURE1, GL_TEXTURE2});
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

void SceneCSM::DrawStatus() {
  if (!fontObj_) {
    return;
  }
  const float kBaseX = 25.0f;
  const float kBaseY = static_cast<float>(height_) - 40.0f;
  const float kOffsetY = -36.0f;

  Text::Get().Begin(width_, height_);
  {

  }
  Text::Get().End();
}

// ********************************************************************************
// View
// ********************************************************************************

void SceneCSM::SetupCamera() {
  const glm::vec3 kCamPt =
      glm::vec3(kCameraRadius * cos(angle_), 0.7f, kCameraRadius * sin(angle_));
  camera_.SetupOrient(kCamPt, glm::vec3(0.0f, -0.175f, 0.0f),
                      glm::vec3(0.0f, 1.0f, 0.0f));
  camera_.SetupPerspective(
      kCameraFOVY, static_cast<float>(width_) / static_cast<float>(height_),
      kCameraNear, kCameraFar);
}

void SceneCSM::SetupLight() {
  lightView_.SetupOrient(kDefaultLightPosition, glm::vec3(0.0f),
                         glm::vec3(0.0f, 1.0f, 0.0f));
  lightView_.SetupPerspective(kLightFOVY, 1.0f, kLightNear, kLightFar);
  lightPV_ = kShadowBias * lightView_.GetProjectionMatrix() *
             lightView_.GetViewMatrix();
}

// ********************************************************************************
// Calculation
// ********************************************************************************

void SceneCSM::UpdateSplitPlanes(int cascades, float near, float far) {
  splitPlanes_.clear();
  if (cascades < 1) {
    cascades = 1;
  }

  splitPlanes_.resize(cascades + 1);
  splitPlanes_[0] = near;
  splitPlanes_[cascades] = far;

  for (int i = 1; i < cascades; i++) {
    const float cilog =
        near * std::powf(far / near, static_cast<float>(i) / static_cast<float>(cascades));
    const float ciuni = near + (far - near) * static_cast<float>(i) / cascades;
    splitPlanes_[i] = kLambda * cilog + ciuni * (1.0f - kLambda);
  }

  progs_[kShadeWithShadow].Use();
  for (int i = 0; i < cascades; i++) {
    // カメラから見た Split Planes の同次座標系におけるz位置を計算します。
    const glm::mat4 proj = camera_.GetProjectionMatrix();
    const float clip = 0.5f * (-splitPlanes_[i + 1] * proj[2][2] + proj[3][2]) /
                           splitPlanes_[i + 1] +
                       0.5f;
    std::string plane = fmt::format("CameraHomogeneousSplitPlanes[{}]", i);
    progs_[kShadeWithShadow].SetUniform(plane.c_str(), clip);
  }
}
