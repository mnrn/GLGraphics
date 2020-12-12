/**
 * @brief SSAOのレンダリングテストシーン
 */

#include "SceneSSAO.h"

#include <boost/assert.hpp>
#include <iostream>
#include <random>

#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Math/UniformDistribution.h"
#include "Texture.h"

// ********************************************************************************
// Overrides scene
// ********************************************************************************

void SceneSSAO::OnInit() {
  if (const auto msg = CompileAndLinkShader()) {
    std::cerr << msg.value() << std::endl;
    BOOST_ASSERT_MSG(false, "failed to compile or link!");
  }

  glEnable(GL_DEPTH_TEST);

  sceneProj_ = glm::perspective(
      glm::radians(kFOVY), static_cast<float>(width_) / height_, 0.3f, 100.0f);

  CreateVAO();

  textures_[WoodTex] = Texture::Load("./Assets/Textures/Wood/wood.jpeg");
  textures_[BrickTex] =
      Texture::Load("./Assets/Textures/Brick/ruin_wall_01.png");

  prog_.SetUniform("Light.L", glm::vec3(0.3f));
  prog_.SetUniform("Light.La", glm::vec3(0.5f));

  std::uint32_t seed = std::random_device()();
  BuildKernel(seed);
  BuildRandDirTex(seed);

  gbuffer_.OnInit(width_, height_);
}

void SceneSSAO::OnDestroy() {
  glDeleteTextures(static_cast<GLsizei>(textures_.size()), textures_.data());
  glDeleteVertexArrays(1, &quad_);
  glDeleteBuffers(static_cast<GLsizei>(vbo_.size()), vbo_.data());
}

void SceneSSAO::OnUpdate(float t) {
  const float deltaT = tPrev_ == 0.0f ? 0.0f : t - tPrev_;
  tPrev_ = t;

  angle_ += kRotSpeed * deltaT;
  if (angle_ > glm::two_pi<float>()) {
    angle_ -= glm::two_pi<float>();
  }
}

void SceneSSAO::OnRender() {
  Pass1();
  Pass2();
  Pass3();
  Pass4();
}

void SceneSSAO::OnResize(int w, int h) {
  SetDimensions(w, h);
  glViewport(0, 0, w, h);
}

// ********************************************************************************
// Shader settings
// ********************************************************************************

std::optional<std::string> SceneSSAO::CompileAndLinkShader() {
  // Compile and links
  if (prog_.Compile("./Assets/Shaders/SSAO/SSAO.vs.glsl", ShaderType::Vertex) &&
      prog_.Compile("./Assets/Shaders/SSAO/SSAO.fs.glsl",
                    ShaderType::Fragment) &&
      prog_.Link()) {
    prog_.Use();
    return std::nullopt;
  } else {
    return prog_.GetLog();
  }
}

void SceneSSAO::CreateVAO() {
  // 四角形ポリゴン用配列
  GLfloat verts[] = {-1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 1.0f,  1.0f, 0.0f,
                     -1.0f, -1.0f, 0.0f, 1.0f, 1.0f,  0.0f, -1.0f, 1.0f, 0.0f};
  GLfloat tc[] = {0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
                  0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f};

  glGenBuffers(static_cast<GLsizei>(vbo_.size()), vbo_.data());

  glBindBuffer(GL_ARRAY_BUFFER, vbo_[VertexPosition]);
  glBufferData(GL_ARRAY_BUFFER, 6 * 3 * sizeof(float), verts, GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, vbo_[TextureCoordinates]);
  glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(float), tc, GL_STATIC_DRAW);

  // 頂点配列オブジェクトの設定
  glGenVertexArrays(1, &quad_);
  glBindVertexArray(quad_);

  glBindBuffer(GL_ARRAY_BUFFER, vbo_[VertexPosition]);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(0); // VertexPosition

  glBindBuffer(GL_ARRAY_BUFFER, vbo_[TextureCoordinates]);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(2); // TexCoord

  glBindVertexArray(0);
}

void SceneSSAO::SetMatrices() {
  const glm::mat4 mv = view_ * model_;
  prog_.SetUniform("ModelViewMatrix", mv);
  prog_.SetUniform("NormalMatrix", glm::mat3(mv));
  prog_.SetUniform("MVP", proj_ * mv);
}

// ********************************************************************************
// Drawing
// ********************************************************************************

void SceneSSAO::Pass1() {
  prog_.SetUniform("Pass", 1);

  const GLuint deferredFBO = gbuffer_.GetDeferredFBO();
  glBindFramebuffer(GL_FRAMEBUFFER, deferredFBO);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);

  view_ = glm::lookAt(glm::vec3(2.1f, 1.5f, 2.1f), glm::vec3(0.0f, 1.0f, 0.0f),
                      glm::vec3(0.0f, 1.0f, 0.0f));
  proj_ = sceneProj_;
  DrawScene();
}

void SceneSSAO::Pass2() {
  prog_.SetUniform("Pass", 2);

  const GLuint ssaoFBO = gbuffer_.GetSSAOFBO();
  glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);

  // AOTexに描きこみます。
  const GLuint aoTex = gbuffer_.GetAOTex();
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         aoTex, 0);
  glClear(GL_COLOR_BUFFER_BIT);
  glDisable(GL_DEPTH_TEST);
  prog_.SetUniform("ProjectionMatrix", sceneProj_);
  DrawQuad();
}

void SceneSSAO::Pass3() {
  prog_.SetUniform("Pass", 3);

  // AOTexを読み込み、BlurAOTexに描きこみます。
  const GLuint aoTex = gbuffer_.GetAOTex();
  const GLuint blurAOTex = gbuffer_.GetBlurAOTex();
  glActiveTexture(
      GL_TEXTURE3); // テクスチャユニット3番を有効にAOTexをバインドします。
  glBindTexture(GL_TEXTURE_2D, aoTex);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         blurAOTex, 0);
  glClear(GL_COLOR_BUFFER_BIT);
  glDisable(GL_DEPTH_TEST);
  DrawQuad();
}

void SceneSSAO::Pass4() {
  prog_.SetUniform("Pass", 4);

  // BlurAOTexを読み込みます。
  const GLuint blurAOTex = gbuffer_.GetBlurAOTex();
  glActiveTexture(
      GL_TEXTURE3); // テクスチャユニット3番を有効にしblurAOTexをバインドします。
  glBindTexture(GL_TEXTURE_2D, blurAOTex);

  // デフォルトのFramebufferに戻して描きこみます。
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glClear(GL_COLOR_BUFFER_BIT);
  glDisable(GL_DEPTH_TEST);
  DrawQuad();
}

void SceneSSAO::DrawScene() {
  prog_.SetUniform("Light.Position", view_ * glm::vec4(3.0f, 3.0f, 1.5f, 1.0f));

  // 床の描画
  glActiveTexture(GL_TEXTURE5);
  glBindTexture(GL_TEXTURE_2D, textures_[WoodTex]);
  prog_.SetUniform("Material.UseTex", 1);
  model_ = glm::mat4(1.0f);
  SetMatrices();
  plane_.Render();

  // 壁の描画
  glActiveTexture(GL_TEXTURE5);
  glBindTexture(GL_TEXTURE_2D, textures_[BrickTex]);
  model_ = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -2.0f));
  model_ =
      glm::rotate(model_, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
  SetMatrices();
  plane_.Render();

  model_ = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, 0.0f, 0.0f));
  model_ =
      glm::rotate(model_, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
  model_ =
      glm::rotate(model_, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
  SetMatrices();
  plane_.Render();

  // メインメッシュの描画
  prog_.SetUniform("Material.UseTex", 0);
  prog_.SetUniform("Material.Kd", glm::vec3(0.9f, 0.5f, 0.2f));
  model_ = glm::rotate(glm::mat4(1.0f), glm::radians(30.0f),
                       glm::vec3(0.0f, 1.0f, 0.0f));
  model_ = glm::scale(model_, glm::vec3(0.3f));
  model_ = glm::translate(model_, glm::vec3(0.0f, 0.282958f, 0.0f));
  SetMatrices();
  mesh_->Render();
}

void SceneSSAO::DrawQuad() {
  view_ = glm::mat4(1.0f);
  proj_ = glm::mat4(1.0f);
  model_ = glm::mat4(1.0f);
  SetMatrices();

  glBindVertexArray(quad_);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glBindVertexArray(0);
}

// ********************************************************************************
// Sampling for SSAO
// ********************************************************************************

void SceneSSAO::BuildKernel(std::uint32_t seed) {
  std::mt19937 engine(seed);
  UniformDistribution dist;

  std::vector<float> kern(3 * kKernelSize);
  for (size_t i = 0; i < kKernelSize; i++) {
    glm::vec3 randDir = dist.OnHemisphere(engine);
    const float kScale =
        static_cast<float>(i * i) / (kKernelSize * kKernelSize);
    randDir *= glm::mix(0.1f, 1.0f, kScale);

    kern[3 * i + 0] = randDir.x;
    kern[3 * i + 1] = randDir.y;
    kern[3 * i + 2] = randDir.z;
  }

  GLuint hProg = prog_.GetHandle();
  GLuint loc = glGetUniformLocation(hProg, "SampleKernel");
  glUniform3fv(loc, kKernelSize, kern.data());
}

void SceneSSAO::BuildRandDirTex(std::uint32_t seed) {
  std::mt19937 engine(seed);
  UniformDistribution dist;

  const size_t kRotTexSize = 4; // 4x4 texture
  std::vector<GLfloat> randDir(3 * kRotTexSize * kRotTexSize);

  for (size_t i = 0; i < kRotTexSize * kRotTexSize; i++) {
    glm::vec2 v = dist.OnCircle(engine);
    randDir[3 * i + 0] = v.x;
    randDir[3 * i + 1] = v.y;
    randDir[3 * i + 2] = 0.0f;
  }

  glGenTextures(1, &textures_[RandDirTex]);
  glBindTexture(GL_TEXTURE_2D, textures_[RandDirTex]);
  glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB16F, kRotTexSize, kRotTexSize);
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, kRotTexSize, kRotTexSize, GL_RGB,
                  GL_FLOAT, randDir.data());
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

  // テクスチャユニット4番にRandDirTexを設定
  glActiveTexture(GL_TEXTURE4);
  glBindTexture(GL_TEXTURE_2D, textures_[RandDirTex]);
}
