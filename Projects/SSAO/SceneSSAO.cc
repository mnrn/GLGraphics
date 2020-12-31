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
// Constexpr variables
// ********************************************************************************

static constexpr float kCameraFOVY = 50.0f;

static constexpr glm::vec4 kLightPos{3.0f, 3.0f, 1.5f, 1.0f};

// NOTE: シェーダーのカーネルサイズと一致させる必要があります。
static constexpr std::size_t kKernelSize = 64;

// ********************************************************************************
// Overrides scene
// ********************************************************************************

void SceneSSAO::OnInit() {
  camera_.SetupOrient(glm::vec3(2.1f, 1.5f, 2.1f), glm::vec3(0.0f, 1.0f, 0.0f),
                      glm::vec3(0.0f, 1.0f, 0.0f));
  camera_.SetupPerspective(
      glm::radians(kCameraFOVY),
      static_cast<float>(width_) / static_cast<float>(height_), 0.3f, 100.0f);

  if (const auto msg = CompileAndLinkShader()) {
    std::cerr << msg.value() << std::endl;
    BOOST_ASSERT_MSG(false, "failed to compile or link!");
  } else {
    SetupShaderConfig();
  }

  CreateVAO();

  textures_[to_i(Textures::WoodTex)] =
      Texture::Load("./Assets/Textures/Wood/wood.jpeg");
  textures_[to_i(Textures::BrickTex)] =
      Texture::Load("./Assets/Textures/Brick/ruin_wall_01.png");

  /*
  std::uint32_t seed = std::random_device()();
  BuildKernel(seed);
  BuildRandRotTex(seed);
  */

  gbuffer_.OnInit(width_, height_);
}

void SceneSSAO::OnDestroy() {
  glDeleteTextures(static_cast<GLsizei>(textures_.size()), textures_.data());
  glDeleteVertexArrays(1, &quad_);
  glDeleteBuffers(static_cast<GLsizei>(vbo_.size()), vbo_.data());
}

void SceneSSAO::OnUpdate(float t) {}

void SceneSSAO::OnRender() {
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  {
    Pass1();
    Pass4();
  }
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
  /*
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
  */
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

  /*
  progs_[to_i(RenderPass::SSAO)].Use();
  progs_[to_i(RenderPass::SSAO)].SetUniform("PositionTex", 0);
  progs_[to_i(RenderPass::SSAO)].SetUniform("NormalTex", 1);
  progs_[to_i(RenderPass::SSAO)].SetUniform("RandRotTex", 2);

  progs_[to_i(RenderPass::Blur)].Use();
  progs_[to_i(RenderPass::Blur)].SetUniform("AOTex", 0);
  */
  progs_[to_i(RenderPass::Lighting)].Use();
  progs_[to_i(RenderPass::Lighting)].SetUniform("PositionTex", 0);
  progs_[to_i(RenderPass::Lighting)].SetUniform("NormalTex", 1);
  progs_[to_i(RenderPass::Lighting)].SetUniform("ColorTex", 2);
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
  const glm::mat4 view = camera_.GetViewMatrix();
  const glm::mat4 proj = camera_.GetProjectionMatrix();
  const glm::mat4 mv = view * model_;

  progs_[to_i(RenderPass::RecordGBuffer)].SetUniform("ModelViewMatrix", mv);
  progs_[to_i(RenderPass::RecordGBuffer)].SetUniform("NormalMatrix",
                                                     glm::mat3(mv));
  progs_[to_i(RenderPass::RecordGBuffer)].SetUniform("MVP", proj * mv);
}

// ********************************************************************************
// Drawing
// ********************************************************************************

void SceneSSAO::Pass1() {
  glBindFramebuffer(GL_FRAMEBUFFER, gbuffer_.GetDeferredFBO());
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);

  progs_[to_i(RenderPass::RecordGBuffer)].Use();

  DrawScene();

  glDisable(GL_DEPTH_TEST);
}

void SceneSSAO::Pass2() {
  glBindFramebuffer(GL_FRAMEBUFFER, gbuffer_.GetSSAOFBO());
  glClear(GL_COLOR_BUFFER_BIT);

  progs_[to_i(RenderPass::SSAO)].Use();
  progs_[to_i(RenderPass::SSAO)].SetUniform("ProjectionMatrix",
                                            camera_.GetProjectionMatrix());
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
}

void SceneSSAO::Pass4() {
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glClear(GL_COLOR_BUFFER_BIT);

  progs_[to_i(RenderPass::Lighting)].Use();
  progs_[to_i(RenderPass::Lighting)].SetUniform(
      "Light.Position", camera_.GetViewMatrix() * kLightPos);
  progs_[to_i(RenderPass::Lighting)].SetUniform("Light.L", glm::vec3(0.3f));
  progs_[to_i(RenderPass::Lighting)].SetUniform("Light.La", glm::vec3(0.5f));
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, gbuffer_.GetPosTex());
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, gbuffer_.GetNormTex());
  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, gbuffer_.GetColorTex());

  DrawQuad();
}

void SceneSSAO::DrawScene() {
  // 床の描画
  auto DrawFloor = [this]() {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures_[to_i(Textures::WoodTex)]);

    progs_[to_i(RenderPass::RecordGBuffer)].SetUniform("Material.UseTex", 1);
    model_ = glm::mat4(1.0f);
    SetMatrices();

    plane_.Render();
  };

  // 壁の描画1
  auto DrawWallL = [this]() {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures_[to_i(Textures::BrickTex)]);

    model_ = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    model_ =
        glm::rotate(model_, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    SetMatrices();

    plane_.Render();
  };

  // 壁の描画2
  auto DrawWallR = [this]() {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures_[to_i(Textures::BrickTex)]);

    model_ = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    model_ =
        glm::rotate(model_, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    model_ =
        glm::rotate(model_, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    SetMatrices();

    plane_.Render();
  };

  // メインメッシュの描画
  auto DrawMesh = [this]() {
    progs_[to_i(RenderPass::RecordGBuffer)].SetUniform("Material.UseTex", 0);
    progs_[to_i(RenderPass::RecordGBuffer)].SetUniform(
        "Material.Kd", glm::vec3(0.9f, 0.5f, 0.2f));

    model_ = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.282958f, 0.0f));
    model_ =
        glm::rotate(model_, glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    model_ = glm::scale(model_, glm::vec3(0.3f));
    SetMatrices();

    mesh_->Render();
  };

  DrawFloor();
  DrawWallL();
  DrawWallR();
  DrawMesh();
}

void SceneSSAO::DrawQuad() {
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
