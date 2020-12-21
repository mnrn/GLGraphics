/**
 * @brief シャドウマッピングのテストシーン
 */

#include "SceneShadowMap.h"

#include <boost/assert.hpp>
#include <iostream>

#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

// ********************************************************************************
// constexpr variables
// ********************************************************************************

static constexpr glm::mat4 kShadowBias{
    glm::vec4(0.5f, 0.0f, 0.0f, 0.0f), glm::vec4(0.0f, 0.5f, 0.0f, 0.0f),
    glm::vec4(0.0f, 0.0f, 0.5f, 0.0f), glm::vec4(0.5f, 0.5f, 0.5f, 1.0f)};

static constexpr float kCameraCenter = 2.0f;

static constexpr glm::vec3 kLightColor{0.85f};
static constexpr float kLightCenter = 1.65f;
static constexpr glm::vec3 kDefaultLightPosition{0.0f, kLightCenter * 5.25f,
                                                 kLightCenter * 7.5f};

// ********************************************************************************
// Override functions
// ********************************************************************************

void SceneShadowMap::OnInit() {
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
    progs_[kShadeWithShadow].SetUniform("ShadowMap", 0);
  }

  // フレームバッファオブジェクトの生成
  SetupFBO();
}

void SceneShadowMap::OnDestroy() {
  glDeleteBuffers(1, &shadowFBO_);
  glDeleteTextures(1, &depthTex_);
}

void SceneShadowMap::OnUpdate(float t) {
  const float deltaT = tPrev_ == 0.0f ? 0.0f : t - tPrev_;
  tPrev_ = t;

  angle_ += kRotSpeed * deltaT;
  if (angle_ > glm::two_pi<float>()) {
    angle_ -= glm::two_pi<float>();
  }

  const glm::vec3 kCamPt =
      glm::vec3(kCameraCenter * 11.5f * cos(angle_), kCameraCenter * 7.0f,
                kCameraCenter * 11.5f * sin(angle_));
  camera_.SetPosition(kCamPt);
}

void SceneShadowMap::OnRender() {
  glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);

  // シャドウマップをチャンネル0に登録します。
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, depthTex_);
  {
    Pass1();
    Pass2();
  }
  glBindTexture(GL_TEXTURE_2D, 0);
}

void SceneShadowMap::OnResize(int w, int h) {
  SetDimensions(w, h);
  glViewport(0, 0, w, h);
}

// ********************************************************************************
// Shader settings
// ********************************************************************************

std::optional<std::string> SceneShadowMap::CompileAndLinkShader() {
  // compile and links
  if (const auto msg = progs_[kRecordDepth].CompileAndLink(
          {{"./Assets/Shaders/ShadowMap/RecordDepth.vs.glsl",
            ShaderType::Vertex},
           {"./Assets/Shaders/ShadowMap/RecordDepth.fs.glsl",
            ShaderType::Fragment}})) {
    return msg;
  }
  if (const auto msg = progs_[kShadeWithShadow].CompileAndLink(
          {{"./Assets/Shaders/ShadowMap/ShadowMap.vs.glsl", ShaderType::Vertex},
           {"./Assets/Shaders/ShadowMap/ShadowMap.fs.glsl",
            ShaderType::Fragment}})) {
    return msg;
  }
  return std::nullopt;
}

void SceneShadowMap::SetMatrices() {
  const glm::mat4 mv = view_ * model_;
  if (pass_ == kRecordDepth) {
    progs_[kRecordDepth].SetUniform("MVP", proj_ * mv);
  } else if (pass_ == kShadeWithShadow) {
    progs_[kShadeWithShadow].SetUniform("ModelViewMatrix", mv);
    progs_[kShadeWithShadow].SetUniform("NormalMatrix", glm::mat3(mv));
    progs_[kShadeWithShadow].SetUniform("MVP", proj_ * mv);
    progs_[kShadeWithShadow].SetUniform("ShadowMatrix", lightPV_ * model_);
  }
}

void SceneShadowMap::SetupFBO() {
  // シャドウマップの生成を行います。
  glGenTextures(1, &depthTex_);
  glBindTexture(GL_TEXTURE_2D, depthTex_);
  glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT24, kShadowMapWidth,
                 kShadowMapHeight);

  // テクスチャの拡大・縮小の方法を指定します。
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  // テクスチャの繰り返しの方法を指定します。
  const GLfloat border[] = {1.0f, 0.0f, 0.0f, 0.0f};
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border);

  // 書き込むポリゴンのテスクチャ座標のR値とテクスチャとの比較を行うように設定します。
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE,
                  GL_COMPARE_REF_TO_TEXTURE);
  // R値がテクスチャの値よりも小さい場合に真となります。(つまり日向となります。)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LESS);

  // シャドウマップ用の FBO を生成し、デプステクスチャをアタッチします。
  glGenFramebuffers(1, &shadowFBO_);
  glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO_);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
                         depthTex_, 0);

  GLenum drawBuffers[] = {GL_NONE};
  glDrawBuffers(1, drawBuffers);

  const GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (result == GL_FRAMEBUFFER_COMPLETE) {
    std::cout << "Framebuffer is complete." << std::endl;
  } else {
    std::cout << "Framebuffer is not complete." << std::endl;
  }

  glBindTexture(GL_TEXTURE_2D, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SceneShadowMap::SetMatrialUniforms(const glm::vec3 &diff,
                                        const glm::vec3 &amb,
                                        const glm::vec3 &spec,
                                        float shininess) {

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
void SceneShadowMap::Pass1() {
  pass_ = RenderPass::kRecordDepth;

  glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO_);
  glClear(GL_DEPTH_BUFFER_BIT);
  glViewport(0, 0, kShadowMapWidth, kShadowMapHeight);

  glCullFace(GL_FRONT);
  glEnable(GL_POLYGON_OFFSET_FILL);
  glPolygonOffset(2.5f, 10.0f);

  // ライトから見たシーンの描画
  view_ = lightView_.GetViewMatrix();
  proj_ = lightView_.GetProjectionMatrix();
  progs_[kRecordDepth].Use();
  DrawScene();

  glDisable(GL_POLYGON_OFFSET_FILL);
  glCullFace(GL_BACK);
}

// render
void SceneShadowMap::Pass2() {
  pass_ = RenderPass::kShadeWithShadow;

  proj_ = camera_.GetProjectionMatrix();
  view_ = camera_.GetViewMatrix();
  progs_[kShadeWithShadow].Use();
  progs_[kShadeWithShadow].SetUniform(
      "Light.Position", view_ * glm::vec4(lightView_.GetPosition(), 1.0f));

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0, 0, width_, height_);

  DrawScene();
}

void SceneShadowMap::DrawScene() {
  const glm::vec3 diff = glm::vec3(0.7f, 0.5f, 0.3f);
  const glm::vec3 amb = diff * 0.05f;
  const glm::vec3 spec = glm::vec3(0.9f, 0.9f, 0.9f);

  // ティーポットの描画
  SetMatrialUniforms(diff, amb, spec, 150.0f);
  model_ = glm::mat4(1.0f);
  model_ =
      glm::rotate(model_, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
  SetMatrices();
  teapot_.Render();

  // トーラスの描画
  SetMatrialUniforms(diff, amb, spec, 150.0f);
  model_ = glm::mat4(1.0f);
  model_ = glm::translate(model_, glm::vec3(0.0f, 2.0f, 5.0f));
  model_ =
      glm::rotate(model_, glm::radians(-45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
  SetMatrices();
  torus_.Render();

  // 平面の描画
  SetMatrialUniforms(glm::vec3(0.25f, 0.25f, 0.25f),
                     glm::vec3(0.0f, 0.0f, 0.0f),
                     glm::vec3(0.05f, 0.05f, 0.05f), 1.0f);
  model_ = glm::mat4(1.0f);
  SetMatrices();
  plane_.Render();

  model_ = glm::mat4(1.0f);
  model_ = glm::translate(model_, glm::vec3(-5.0f, 5.0f, 0.0f));
  model_ =
      glm::rotate(model_, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
  SetMatrices();
  plane_.Render();

  model_ = glm::mat4(1.0f);
  model_ = glm::translate(model_, glm::vec3(0.0f, 5.0f, -5.0f));
  model_ =
      glm::rotate(model_, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
  SetMatrices();
  plane_.Render();

  model_ = glm::mat4(1.0f);
}

// ********************************************************************************
// View
// ********************************************************************************

void SceneShadowMap::SetupCamera() {
  const glm::vec3 kCamPt =
      glm::vec3(kCameraCenter * 11.5f * cos(angle_), kCameraCenter * 7.0f,
                kCameraCenter * 11.5f * sin(angle_));
  camera_.SetupOrient(kCamPt, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
  camera_.SetupPerspective(
      kFOVY, static_cast<float>(width_) / static_cast<float>(height_), 0.1f,
      100.0f);
}

void SceneShadowMap::SetupLight() {
  lightView_.SetupOrient(kDefaultLightPosition, glm::vec3(0.0f),
                         glm::vec3(0.0f, 1.0f, 0.0f));
  lightView_.SetupPerspective(kFOVY, 1.0f, 1.0f, 25.0f);
  lightPV_ = kShadowBias * lightView_.GetProjectionMatrix() *
             lightView_.GetViewMatrix();
}
