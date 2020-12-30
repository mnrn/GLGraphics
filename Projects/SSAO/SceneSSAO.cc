/**
 * @brief SSAOのレンダリングテストシーン
 */

#include "SceneSSAO.h"

#include <boost/assert.hpp>
#include <iostream>
#include <random>

#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/integer.hpp>

#include "Graphics/Texture.h"
#include "HID/KeyInput.h"
#include "Math/UniformDistribution.h"
#include "UI/Font.h"
#include "UI/Text.h"

// ********************************************************************************
// Overrides scene
// ********************************************************************************

void SceneSSAO::OnInit() {
  Text::Create();
  Font::Create();
  KeyInput::Create();

  if ((fontObj_ = Font::Get().Entry(
           "./Assets/Fonts/UbuntuMono/UbuntuMono-Regular.ttf"))) {
    fontObj_->SetupWithSize(36);
  }

  if (const auto msg = CompileAndLinkShader()) {
    std::cerr << msg.value() << std::endl;
    BOOST_ASSERT_MSG(false, "failed to compile or link!");
  } else {
    SetupShaderConfig();
  }

  sceneProj_ = glm::perspective(
      glm::radians(kFOVY),
      static_cast<float>(width_) / static_cast<float>(height_), 0.3f, 100.0f);

  CreateVAO();

  textures_[to_i(Textures::WoodTex)] =
      Texture::Load("./Assets/Textures/Wood/wood.jpeg");
  textures_[to_i(Textures::BrickTex)] =
      Texture::Load("./Assets/Textures/Brick/ruin_wall_01.png");

  std::uint32_t seed = std::random_device()();
  BuildKernel(seed);
  BuildRandRotTex(seed);

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

  if (KeyInput::Get().IsTrg(Key::Left)) {
    type_ = static_cast<RenderType>(
        glm::mod(to_i(type_) - 1, to_i(RenderType::Num)));
  } else if (KeyInput::Get().IsTrg(Key::Right)) {
    type_ = static_cast<RenderType>(
        glm::mod(to_i(type_) + 1, to_i(RenderType::Num)));
  }
}

void SceneSSAO::OnRender() {
  glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
  glEnable(GL_DEPTH_TEST);

  {
    Pass1();
    Pass2();
    Pass3();
    Pass4();
  }

  DrawText();
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
  if (const auto msg = progs_[to_i(RenderPass::RecordGBuffer)].CompileAndLink(
          {{"./Assets/Shaders/SSAO/GBuffer.vs.glsl", ShaderType::Vertex},
           {"./Assets/Shaders/SSAO/GBuffer.fs.glsl", ShaderType::Fragment}})) {
    return msg;
  }
  if (const auto msg = progs_[to_i(RenderPass::SSAO)].CompileAndLink(
          {{"./Assets/Shaders/SSAO/SSAO.vs.glsl", ShaderType::Vertex},
           {"./Assets/Shaders/SSAO/SSAO.fs.glsl", ShaderType::Fragment}})) {
    return msg;
  }
  if (const auto msg = progs_[to_i(RenderPass::Blur)].CompileAndLink(
          {{"./Assets/Shaders/SSAO/SSAO.vs.glsl", ShaderType::Vertex},
           {"./Assets/Shaders/SSAO/Blur.fs.glsl", ShaderType::Fragment}})) {
    return msg;
  }
  if (const auto msg = progs_[to_i(RenderPass::Lighting)].CompileAndLink(
          {{"./Assets/Shaders/SSAO/SSAO.vs.glsl", ShaderType::Vertex},
           {"./Assets/Shaders/SSAO/Lighting.fs.glsl", ShaderType::Fragment}})) {
    return msg;
  }
  return std::nullopt;
}

void SceneSSAO::SetupShaderConfig() {
  progs_[to_i(RenderPass::RecordGBuffer)].Use();
  progs_[to_i(RenderPass::RecordGBuffer)].SetUniform("DiffTex", 0);

  progs_[to_i(RenderPass::SSAO)].Use();
  progs_[to_i(RenderPass::SSAO)].SetUniform("PositionTex", 0);
  progs_[to_i(RenderPass::SSAO)].SetUniform("NormalTex", 1);
  progs_[to_i(RenderPass::SSAO)].SetUniform("RandRotTex", 2);

  progs_[to_i(RenderPass::Blur)].Use();
  progs_[to_i(RenderPass::Blur)].SetUniform("AOTex", 0);

  progs_[to_i(RenderPass::Lighting)].Use();
  progs_[to_i(RenderPass::Lighting)].SetUniform("PositionTex", 0);
  progs_[to_i(RenderPass::Lighting)].SetUniform("NormalTex", 1);
  progs_[to_i(RenderPass::Lighting)].SetUniform("ColorTex", 2);
  progs_[to_i(RenderPass::Lighting)].SetUniform("AOTex", 3);
}

void SceneSSAO::CreateVAO() {
  // 四角形ポリゴン用配列
  GLfloat verts[] = {-1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 1.0f,  1.0f, 0.0f,
                     -1.0f, -1.0f, 0.0f, 1.0f, 1.0f,  0.0f, -1.0f, 1.0f, 0.0f};
  GLfloat tc[] = {0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
                  0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f};

  glGenBuffers(static_cast<GLsizei>(vbo_.size()), vbo_.data());

  glBindBuffer(GL_ARRAY_BUFFER, vbo_[to_i(VertexBuffer::VertexPosition)]);
  glBufferData(GL_ARRAY_BUFFER, 6 * 3 * sizeof(float), verts, GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, vbo_[to_i(VertexBuffer::TextureCoordinates)]);
  glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(float), tc, GL_STATIC_DRAW);

  // 頂点配列オブジェクトの設定
  glGenVertexArrays(1, &quad_);
  glBindVertexArray(quad_);

  glBindBuffer(GL_ARRAY_BUFFER, vbo_[to_i(VertexBuffer::VertexPosition)]);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(0); // VertexPosition

  glBindBuffer(GL_ARRAY_BUFFER, vbo_[to_i(VertexBuffer::TextureCoordinates)]);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(1); // TexCoord

  glBindVertexArray(0);
}

void SceneSSAO::SetMatrices() {
  const glm::mat4 mv = view_ * model_;

  progs_[to_i(RenderPass::RecordGBuffer)].SetUniform("ModelViewMatrix", mv);
  progs_[to_i(RenderPass::RecordGBuffer)].SetUniform("NormalMatrix",
                                                     glm::mat3(mv));
  progs_[to_i(RenderPass::RecordGBuffer)].SetUniform("MVP", proj_ * mv);
}

// ********************************************************************************
// Drawing
// ********************************************************************************

void SceneSSAO::Pass1() {
  glBindFramebuffer(GL_FRAMEBUFFER, gbuffer_.GetDeferredFBO());
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  view_ = glm::lookAt(glm::vec3(2.1f, 1.5f, 2.1f), glm::vec3(0.0f, 1.0f, 0.0f),
                      glm::vec3(0.0f, 1.0f, 0.0f));
  proj_ = sceneProj_;
  progs_[to_i(RenderPass::RecordGBuffer)].Use();

  DrawScene();

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SceneSSAO::Pass2() {
  glBindFramebuffer(GL_FRAMEBUFFER, gbuffer_.GetSSAOFBO());
  glClear(GL_COLOR_BUFFER_BIT);

  progs_[to_i(RenderPass::SSAO)].Use();
  progs_[to_i(RenderPass::SSAO)].SetUniform("ProjectionMatrix", sceneProj_);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, gbuffer_.GetPosTex());
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, gbuffer_.GetNormTex());
  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, textures_[to_i(Textures::RandRotTex)]);

  DrawQuad();

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SceneSSAO::Pass3() {
  glBindFramebuffer(GL_FRAMEBUFFER, gbuffer_.GetSSAOBlurFBO());
  glClear(GL_COLOR_BUFFER_BIT);

  progs_[to_i(RenderPass::Blur)].Use();
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, gbuffer_.GetAOTex());

  DrawQuad();

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SceneSSAO::Pass4() {
  glClear(GL_COLOR_BUFFER_BIT);

  progs_[to_i(RenderPass::Lighting)].Use();
  progs_[to_i(RenderPass::Lighting)].SetUniform("Light.Position",
                                                view_ * lightPos_);
  progs_[to_i(RenderPass::Lighting)].SetUniform("Light.L", glm::vec3(0.3f));
  progs_[to_i(RenderPass::Lighting)].SetUniform("Light.La", glm::vec3(0.5f));
  progs_[to_i(RenderPass::Lighting)].SetUniform("Type", to_i(type_));
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, gbuffer_.GetPosTex());
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, gbuffer_.GetNormTex());
  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, gbuffer_.GetColorTex());
  glActiveTexture(GL_TEXTURE3);
  glBindTexture(GL_TEXTURE_2D, gbuffer_.GetBlurAOTex());

  DrawQuad();

  glBindTexture(GL_TEXTURE_2D, 0);
}

void SceneSSAO::DrawScene() {
  // テクスチャユニット0番を有効にします。
  glActiveTexture(GL_TEXTURE0);

  // 床の描画
  {
    glBindTexture(GL_TEXTURE_2D, textures_[to_i(Textures::WoodTex)]);

    progs_[to_i(RenderPass::RecordGBuffer)].SetUniform("Material.UseTex", 1);
    model_ = glm::mat4(1.0f);
    SetMatrices();

    plane_.Render();
  }

  // 壁の描画1
  {
    glBindTexture(GL_TEXTURE_2D, textures_[to_i(Textures::BrickTex)]);

    model_ = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -2.0f));
    model_ =
        glm::rotate(model_, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    SetMatrices();

    plane_.Render();
  }

  // 壁の描画2
  {
    model_ = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, 0.0f, 0.0f));
    model_ =
        glm::rotate(model_, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    model_ =
        glm::rotate(model_, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    SetMatrices();

    plane_.Render();
  }

  // メインメッシュの描画
  {
    progs_[to_i(RenderPass::RecordGBuffer)].SetUniform("Material.UseTex", 0);
    progs_[to_i(RenderPass::RecordGBuffer)].SetUniform(
        "Material.Kd", glm::vec3(0.9f, 0.5f, 0.2f));

    model_ = glm::rotate(glm::mat4(1.0f), glm::radians(30.0f),
                         glm::vec3(0.0f, 1.0f, 0.0f));
    model_ = glm::scale(model_, glm::vec3(0.3f));
    model_ = glm::translate(model_, glm::vec3(0.0f, 0.282958f, 0.0f));
    SetMatrices();

    mesh_->Render();
  }

  glBindTexture(GL_TEXTURE_2D, 0);
}

void SceneSSAO::DrawQuad() {
  glBindVertexArray(quad_);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glBindVertexArray(0);
}

void SceneSSAO::DrawText() {
  if (!fontObj_) {
    return;
  }

  const float kOffsetX = 25.0f;
  const float kOffsetY = static_cast<float>(height_) - 40.0f;
  const std::map<RenderType, std::string> kTexts = {
      {RenderType::SSAO, "RenderType: SSAO"},
      {RenderType::SSAOOnly, "RenderType: SSAO Only"},
      {RenderType::NoSSAO, "RenderType: Diffuse Only"},
  };
  if (kTexts.count(type_) <= 0) {
    return;
  }
  Text::Get().Begin(width_, height_);
  {
    Text::Get().Render(kTexts.at(type_).c_str(), kOffsetX, kOffsetY, fontObj_);
    Text::Get().Render("Press <- or ->: Switch RenderType", kOffsetX,
                       kOffsetY - 36.0f, fontObj_);
  }
  Text::Get().End();
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

  progs_[to_i(RenderPass::SSAO)].Use();
  GLuint hProg = progs_[to_i(RenderPass::SSAO)].GetHandle();
  GLuint loc = glGetUniformLocation(hProg, "SampleKernel");
  glUniform3fv(loc, kKernelSize, kern.data());
}

void SceneSSAO::BuildRandRotTex(std::uint32_t seed) {
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

  glGenTextures(1, &textures_[to_i(Textures::RandRotTex)]);
  glBindTexture(GL_TEXTURE_2D, textures_[to_i(Textures::RandRotTex)]);
  glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB16F, kRotTexSize, kRotTexSize);
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, kRotTexSize, kRotTexSize, GL_RGB,
                  GL_FLOAT, randDir.data());
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

  glBindTexture(GL_TEXTURE_2D, 0);
}
