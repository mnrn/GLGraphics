/**
 * @brief シャドウマッピングのテストシーン
 */

#include "SceneShadowMap.h"

#include <boost/assert.hpp>
#include <iostream>

#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

// ********************************************************************************
// Override functions
// ********************************************************************************

void SceneShadowMap::OnInit() {
  if (const auto msg = CompileAndLinkShader()) {
    std::cerr << msg.value() << std::endl;
    BOOST_ASSERT_MSG(false, "failed to compile or link!");
  }

  glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
  glEnable(GL_DEPTH_TEST);

  // フレームバッファオブジェクトの生成
  SetupFBO();

  shadowBias_ = glm::mat4(
      glm::vec4(0.5f, 0.0f, 0.0f, 0.0f), glm::vec4(0.0f, 0.5f, 0.0f, 0.0f),
      glm::vec4(0.0f, 0.0f, 0.5f, 0.0f), glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));

  const float kCenter = 1.65f;
  glm::vec3 lightPos = glm::vec3(0.0f, kCenter * 5.25f, kCenter * 7.5f);
  lightFrustum_.Orient(lightPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
  lightFrustum_.SetPerspective(kFOVY, 1.0f, 1.0f, 25.0f);
  lightPV_ = shadowBias_ * lightFrustum_.GetProjectionMatrix() *
             lightFrustum_.GetViewMatrix();

  progs_[kShadeWithShadow].Use();
  progs_[kShadeWithShadow].SetUniform("Light.La", glm::vec3(0.85f));
  progs_[kShadeWithShadow].SetUniform("Light.Ld", glm::vec3(0.85f));
  progs_[kShadeWithShadow].SetUniform("Light.Ls", glm::vec3(0.85f));
  progs_[kShadeWithShadow].SetUniform("ShadowMap", 0);
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
}

void SceneShadowMap::OnRender() {
  Pass1();
  Pass2();
  // Pass3();
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
  if (!(progs_[kRecordDepth].Compile(
            "./Assets/Shaders/ShadowMap/RecordDepth.vs.glsl",
            ShaderType::Vertex) &&
        progs_[kRecordDepth].Compile(
            "./Assets/Shaders/ShadowMap/RecordDepth.fs.glsl",
            ShaderType::Fragment) &&
        progs_[kRecordDepth].Link())) {
    return progs_[kRecordDepth].GetLog();
  }

  if (!(progs_[kShadeWithShadow].Compile(
            "./Assets/Shaders/ShadowMap/ShadowMap.vs.glsl",
            ShaderType::Vertex) &&
        progs_[kShadeWithShadow].Compile(
            "./Assets/Shaders/ShadowMap/ShadowMap.fs.glsl",
            ShaderType::Fragment) &&
        progs_[kShadeWithShadow].Link())) {
    return progs_[kShadeWithShadow].GetLog();
  }

  if (!(progs_[kDebugFrustum].Compile("./Assets/Shaders/Solid/Solid.vs.glsl",
                                      ShaderType::Vertex) &&
        progs_[kDebugFrustum].Compile("./Assets/Shaders/Solid/Solid.fs.glsl",
                                      ShaderType::Fragment) &&
        progs_[kDebugFrustum].Link())) {
    return progs_[kDebugFrustum].GetLog();
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

  // シャドウマップをチャンネル0に登録します。
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, depthTex_);

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
  progs_[kRecordDepth].Use();
  view_ = lightFrustum_.GetViewMatrix();
  proj_ = lightFrustum_.GetProjectionMatrix();

  glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO_);
  glClear(GL_DEPTH_BUFFER_BIT);
  glViewport(0, 0, kShadowMapWidth, kShadowMapHeight);

  // 前面をカリングします。
  glEnable(GL_CULL_FACE);
  glCullFace(GL_FRONT);

  // Polygon offset を有効にします。
  glEnable(GL_POLYGON_OFFSET_FILL);
  glPolygonOffset(2.5f, 10.0f);

  DrawScene();

  // 背面カリングに戻します。
  glCullFace(GL_BACK);
  glFlush();
}

// render
void SceneShadowMap::Pass2() {
  pass_ = RenderPass::kShadeWithShadow;
  const float kCenter = 2.0f;
  const glm::vec3 camPt =
      glm::vec3(kCenter * 11.5f * cos(angle_), kCenter * 7.0f,
                kCenter * 11.5f * sin(angle_));
  view_ = glm::lookAt(camPt, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
  progs_[kShadeWithShadow].Use();
  progs_[kShadeWithShadow].SetUniform(
      "Light.Position", view_ * glm::vec4(lightFrustum_.GetOrigin(), 1.0f));
  proj_ = glm::perspective(
      glm::radians(kFOVY),
      static_cast<float>(width_) / static_cast<float>(height_), 0.1f, 100.0f);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0, 0, width_, height_);

  DrawScene();
}

// draw the light's frustum
void SceneShadowMap::Pass3() {
  pass_ = kDebugFrustum;
  progs_[kDebugFrustum].Use();
  progs_[kDebugFrustum].SetUniform("Color", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
  const glm::mat4 mv = view_ * lightFrustum_.GetInvViewMatrix();
  progs_[kDebugFrustum].SetUniform("MVP", proj_ * mv);
  // lightFrustum_.Render();
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
