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

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-macros"
#define FMT_HEADER_ONLY
#include <fmt/format.h>
#pragma clang diagnostic pop

#include "CSM.h"
#include "GUI/GUI.h"
#include "HID/KeyInput.h"
#include "UI/Font.h"
#include "UI/Text.h"

#ifdef WIN32
#ifdef far
#undef far
#endif
#ifdef near
#undef near
#endif
#endif

// ********************************************************************************
// constexpr variables
// ********************************************************************************

static constexpr int kCascadedNum = 3;

static constexpr float kLambda = 0.5f;

static constexpr float kCameraFOVY = 50.0f;
static constexpr float kCameraNear = 0.3f;
static constexpr float kCameraFar = 8.0f;

static constexpr float kCameraHeight = 1.0f;
static constexpr float kCameraRadius = 2.25f;
static constexpr float kCameraDefaultAngle = glm::two_pi<float>() * 0.85f;

static constexpr glm::vec3 kLightDefaultPosition{-2.0f, 2.0f, -2.0f};
static constexpr glm::vec3 kLightDefaultTarget{0.0f};
static constexpr glm::vec3 kLightDefaultDir =
    kLightDefaultTarget - kLightDefaultPosition;

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

  angle_ += param_.rotSpeed * deltaT;
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

  DrawGUI();
}

void SceneCSM::OnResize(int w, int h) {
  SetDimensions(w, h);
  glViewport(0, 0, w, h);
}

void SceneCSM::OnPreRender() {
  CSM csm;

  const auto splits =
      csm.ComputeSplitPlanes(kCascadedNum, kCameraNear, kCameraFar, kLambda);

  progs_[kShadeWithShadow].Use();
  progs_[kShadeWithShadow].SetUniform("CascadesNum", kCascadedNum);
  csm.UpdateSplitPlanesUniform(
      kCascadedNum, splits, camera_, [&](int i, float clip) {
        const std::string plane =
            fmt::format("CameraHomogeneousSplitPlanes[{}]", i);
        progs_[kShadeWithShadow].SetUniform(plane.c_str(), clip);
      });

  csm.UpdateFrustums(kCascadedNum, splits, camera_);
  vpCrops_ = csm.ComputeCropMatrices(kCascadedNum, kLightDefaultDir,
                                     static_cast<float>(kShadowMapSize));
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
    const glm::mat4 mvp = vpCrops_[cascadeIdx_] * model_;
    progs_[kRecordDepth].SetUniform("MVP", mvp);
  } else if (pass_ == kShadeWithShadow) {
    const glm::mat4 mv = view_ * model_;
    progs_[kShadeWithShadow].SetUniform("ModelViewMatrix", mv);
    progs_[kShadeWithShadow].SetUniform("NormalMatrix", glm::mat3(mv));

    const glm::mat4 mvp = proj_ * mv;
    progs_[kShadeWithShadow].SetUniform("MVP", mvp);

    const glm::mat4 kInvView = camera_.GetInverseViewMatrix();
    for (int i = 0; i < kCascadedNum; i++) {
      const glm::mat4 kLightMVP = kShadowBias * vpCrops_[i] * kInvView;
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
  glPolygonOffset(2.5f, 10.0f);

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
  progs_[kShadeWithShadow].SetUniform("IsPCF", param_.isPCF);
  progs_[kShadeWithShadow].SetUniform("IsShadowOnly", param_.isShadowOnly);
  progs_[kShadeWithShadow].SetUniform("IsVisibleIndicator",
                                      param_.isVisibleIndicator);

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

void SceneCSM::DrawGUI() {
  GUI::NewFrame();

  ImGui::Begin("Cascaded Shadow Maps Config");
  ImGui::Checkbox("PCF ON", &param_.isPCF);
  ImGui::Checkbox("Visible Indicator", &param_.isVisibleIndicator);
  ImGui::Checkbox("Shadow Only", &param_.isShadowOnly);
  ImGui::SliderFloat("Camera Rotate Speed", &param_.rotSpeed, 0.0f, 0.5f);
  ImGui::End();

  GUI::Render();
}

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

void SceneCSM::SetupLight() {}
