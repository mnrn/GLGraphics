/**
 * @brief シャドウマッピングのテストシーン
 */

#include "SceneCSM.h"

#include <boost/assert.hpp>
#include <iostream>

#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>


// ********************************************************************************
// Override functions
// ********************************************************************************

void SceneCSM::OnInit() {
  if (const auto msg = CompileAndLinkShader()) {
    std::cerr << msg.value() << std::endl;
    BOOST_ASSERT_MSG(false, "failed to compile or link!");
  }

  // フレームバッファオブジェクトの生成
  if (!csmFBO_.OnInit(kCascadesNum, kShadowMapWidth, kShadowMapHeight)) {
    BOOST_ASSERT_MSG(false, "Framebuffer is not complete.");
  }

  glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
  glEnable(GL_DEPTH_TEST);

  camPt_ = glm::vec3(kCameraCenter * 11.5f * cos(angle_), kCameraCenter * 7.0f,
                     kCameraCenter * 11.5f * sin(angle_));

  const float kLightCenter = 1.65f;
  glm::vec3 lightPos = glm::vec3(0.0f, kLightCenter * 5.25f, kLightCenter * 7.5f);
  lightFrustum0_.Orient(lightPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
  lightFrustum0_.SetPerspective(kFOVY, 1.0f, 0.1f, 100.0f);

  progs_[kShadeWithCascadedShadow].Use();
  progs_[kShadeWithCascadedShadow].SetUniform("Light.La", glm::vec3(0.85f));
  progs_[kShadeWithCascadedShadow].SetUniform("Light.Ld", glm::vec3(0.85f));
  progs_[kShadeWithCascadedShadow].SetUniform("Light.Ls", glm::vec3(0.85f));
  progs_[kShadeWithCascadedShadow].SetUniform("ShadowMaps[0]", 0);
  progs_[kShadeWithCascadedShadow].SetUniform("ShadowMaps[1]", 1);
  progs_[kShadeWithCascadedShadow].SetUniform("ShadowMaps[2]", 2);
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
#if false
  camPt_ = glm::vec3(kCameraCenter * 11.5f * cos(angle_), kCameraCenter * 7.0f, 
    kCameraCenter * 11.5f * sin(angle_));
#endif
}

void SceneCSM::OnRender() {
  Pass1();
  Pass2();
  // Pass3();
}

void SceneCSM::OnResize(int w, int h) {
  SetDimensions(w, h);
  glViewport(0, 0, w, h);
}

// ********************************************************************************
// Shader settings
// ********************************************************************************

std::optional<std::string> SceneCSM::CompileAndLinkShader() {
  // compile and links
  if (!(progs_[kRecordDepth].Compile("./Assets/Shaders/ShadowMap/RecordDepth.vs.glsl",
                      ShaderType::Vertex) &&
        progs_[kRecordDepth].Compile("./Assets/Shaders/ShadowMap/RecordDepth.fs.glsl",
                      ShaderType::Fragment) &&
        progs_[kRecordDepth].Link())) {
    return progs_[kRecordDepth].GetLog();
  }

  if (!(progs_[kShadeWithCascadedShadow ].Compile("./Assets/Shaders/CSM/CSM.vs.glsl",
                      ShaderType::Vertex) &&
        progs_[kShadeWithCascadedShadow].Compile("./Assets/Shaders/CSM/CSM.fs.glsl",
                      ShaderType::Fragment) &&
        progs_[kShadeWithCascadedShadow].Link())) {
    return progs_[kShadeWithCascadedShadow].GetLog();
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

void SceneCSM::SetMatrices() {
  const glm::mat4 kShadowBias = glm::mat4(
      glm::vec4(0.5f, 0.0f, 0.0f, 0.0f), glm::vec4(0.0f, 0.5f, 0.0f, 0.0f),
      glm::vec4(0.0f, 0.0f, 0.5f, 0.0f), glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));

  if (pass_ == kRecordDepth) {
    progs_[kRecordDepth].SetUniform("MVP", shadowMatrices_[curShadowId_] * model_);
  } else if (pass_ == kShadeWithCascadedShadow) {
    const glm::mat4 mv = view_ * model_;
    progs_[kShadeWithCascadedShadow].SetUniform("ModelViewMatrix", mv);
    progs_[kShadeWithCascadedShadow].SetUniform("NormalMatrix", glm::mat3(mv));
    progs_[kShadeWithCascadedShadow].SetUniform("MVP", proj_ * mv);

    progs_[kShadeWithCascadedShadow].SetUniform(
        "ShadowMatrices[0]", kShadowBias * shadowMatrices_[0] * model_);
    progs_[kShadeWithCascadedShadow].SetUniform(
        "ShadowMatrices[1]", kShadowBias * shadowMatrices_[1] * model_);
    progs_[kShadeWithCascadedShadow].SetUniform(
        "ShadowMatrices[2]", kShadowBias * shadowMatrices_[2] * model_);
  }
}

void SceneCSM::SetMatrialUniforms(const glm::vec3& diff, const glm::vec3& amb,
  const glm::vec3& spec,
  float shininess) {

  if (pass_ != kShadeWithCascadedShadow) {
    return;
  }
  progs_[kShadeWithCascadedShadow].SetUniform("Material.Ka", amb);
  progs_[kShadeWithCascadedShadow].SetUniform("Material.Kd", diff);
  progs_[kShadeWithCascadedShadow].SetUniform("Material.Ks", spec);
  progs_[kShadeWithCascadedShadow].SetUniform("Material.Shininess", shininess);
}

void SceneCSM::ComputeSplitPlanes(int split, float lambda, float near, float far) {
  splitPlanes_.clear();
  if (split < 1) {
    split = 1;
  }

  splitPlanes_.resize(split + 1);
  splitPlanes_[0] = near;
  splitPlanes_[split] = far;
  for (int i = 1; i < split; i++) {
    const float cilog = near * std::powf(far / near, static_cast<float>(i) / split);
    const float ciuni = near + (far - near) * static_cast<float>(i) / split;
    splitPlanes_[i] = lambda * cilog + ciuni * (1.0f - lambda);
  }
}

void SceneCSM::ComputeShadowMatrices() {
  const glm::mat4 kLightView = lightFrustum0_.GetViewMatrix();
  const glm::mat4 kLightProj = lightFrustum0_.GetProjectionMatrix();
  const glm::mat4 kLightPV = kLightProj * kLightView;
  
  const float kAspectRatio = static_cast<float>(width_) / height_; 

#if true
  proj_ = glm::perspective(glm::radians(kFOVY),
                       static_cast<float>(width_) / height_, kEyeNear, kEyeFar);
  view_ = glm::lookAt(camPt_, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
  const std::vector<glm::vec4> points = GetWorldSpaceFrustum();
  const glm::mat4 lightPV = GetLightPVMatrix(points);
  shadowMatrices_[0] = lightPV;
  //shadowMatrices_[0] = kLightPV;
#else
  Frustum f;
  f.Orient(camPt_, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
  ComputeSplitPlanes(kCascadesNum, kLambda, kEyeNear, kEyeFar);
  for (int i = 0; i < kCascadesNum; i++) {
    f.SetPerspective(kFOVY, kAspectRatio, splitPlanes_[i], splitPlanes_[i + 1]);
    AABB box = f.ComputeAABB(pv);
    shadowMatrices_[i] = box.ComputeCropMatrix();
  }
#endif
}

std::vector<glm::vec4> SceneCSM::GetWorldSpaceFrustum() const {
  std::vector<glm::vec4> points;

  const glm::mat4 inverseProjectionMatrix = glm::inverse(proj_ * view_);
  for (unsigned int x = 0; x < 2; x++)
    for (unsigned int y = 0; y < 2; y++)
      for (unsigned int z = 0; z < 2; z++) {
        glm::vec4 projClipSpacePosition(x * 2.0f - 1.0f, y * 2.0f - 1.0f,
                                        z * 2.0f - 1.0f, 1.0f);
        glm::vec4 projWorldSpacePosition =
            inverseProjectionMatrix * projClipSpacePosition;
        points.emplace_back(projWorldSpacePosition / projWorldSpacePosition.w);
      }
  return points;
}

glm::mat4 SceneCSM::GetLightPVMatrix(const std::vector<glm::vec4>& points) const {
  const glm::vec3 lightDir = -lightFrustum0_.GetOrigin();
  glm::mat4 lvMatrix =
      glm::lookAt(glm::normalize(lightDir), glm::vec3(0.0f), glm::vec3(1.0f));

  glm::vec4 transf = lvMatrix * points[0];
  float minZ = transf.z;
  float maxZ = transf.z;
  float minX = transf.z;
  float maxX = transf.x;
  float minY = transf.y;
  float maxY = transf.y;

  for (unsigned int i = 1; i < 8; i++) {
    transf = lvMatrix * points[i];

    if (transf.z > maxZ)
      maxZ = transf.z;
    if (transf.z < minZ)
      minZ = transf.z;
    if (transf.x > maxX)
      maxX = transf.x;
    if (transf.x < minX)
      minX = transf.x;
    if (transf.y > maxY)
      maxY = transf.y;
    if (transf.y < minY)
      minY = transf.y;
  }

  glm::mat4 lpMatrix = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, minZ, maxZ);

  const float scaleX = 2.0f / (maxX - minX);
  const float scaleY = 2.0f / (maxY - minY);
  const float offsetX = -0.5f * (minX + maxX) * scaleX;
  const float offsetY = -0.5f * (minY + maxY) * scaleY;

  glm::mat4 cropMatrix(1.0f);
  cropMatrix[0][0] = scaleX;
  cropMatrix[1][1] = scaleY;
  cropMatrix[3][0] = offsetX;
  cropMatrix[3][1] = offsetY;

  return cropMatrix * lpMatrix * lvMatrix;
}

// ********************************************************************************
// Drawing
// ********************************************************************************

// Shadow map generation
void SceneCSM::Pass1() {
  pass_ = RenderPass::kRecordDepth;
  progs_[kRecordDepth].Use();

  ComputeShadowMatrices();

  glViewport(0, 0, kShadowMapWidth, kShadowMapHeight);

  
  // 前面をカリングします。
  glEnable(GL_CULL_FACE);
  glCullFace(GL_FRONT);

  // Polygon offset を有効にします。
  glEnable(GL_POLYGON_OFFSET_FILL);
  glPolygonOffset(2.5f, 10.0f);

  csmFBO_.BindForWriting();
  for (int i = 0; i < 1; i++) {
    curShadowId_ = i;
    csmFBO_.AttachFramebuffer(i);
    glClear(GL_DEPTH_BUFFER_BIT);
    DrawScene();
  }

  // Polygon offset を無効にします。
  glEnable(GL_POLYGON_OFFSET_FILL);

  // 背面カリングに戻します。
  glCullFace(GL_BACK);
  glFlush();
}

// render
void SceneCSM::Pass2() {
  pass_ = RenderPass::kShadeWithCascadedShadow;
  progs_[kShadeWithCascadedShadow].Use();

  proj_ = glm::perspective(glm::radians(kFOVY), static_cast<float>(width_) / height_, kEyeNear, kEyeFar);
  view_ = glm::lookAt(camPt_, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
  progs_[kShadeWithCascadedShadow].SetUniform("Light.Position",
                   view_ * glm::vec4(lightFrustum0_.GetOrigin(), 1.0f));

#if true
  progs_[kShadeWithCascadedShadow].SetUniform("SplitPlanes[0]", kEyeFar);
#else
  progs_[kShadeWithCascadedShadow].SetUniform("SplitPlanes[0]",
                                              splitPlanes_[1]);
  progs_[kShadeWithCascadedShadow].SetUniform("SplitPlanes[1]",
                                              splitPlanes_[2]);
  progs_[kShadeWithCascadedShadow].SetUniform("SplitPlanes[2]",
                                              splitPlanes_[3]);
#endif

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0, 0, width_, height_);

  csmFBO_.BindForReading({GL_TEXTURE0, GL_TEXTURE1, GL_TEXTURE2});

  DrawScene();
}

// draw the light's frustum
void SceneCSM::Pass3() {
  pass_ = kDebugFrustum;
  progs_[kDebugFrustum].Use();
  progs_[kDebugFrustum].SetUniform("Color", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
  const glm::mat4 mv = view_ * lightFrustum0_.GetInvViewMatrix();
  progs_[kDebugFrustum].SetUniform("MVP", proj_ * mv);
  //lightFrustum0_.Render();
}

void SceneCSM::DrawScene() {
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
