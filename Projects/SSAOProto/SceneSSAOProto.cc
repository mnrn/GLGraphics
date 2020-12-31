/**
 * @brief SSAOのテストシーン
 */

#include "SceneSSAOProto.h"

#include <boost/assert.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include "Graphics/Texture.h"

// ********************************************************************************
// Constexpr variables
// ********************************************************************************

static constexpr float kCameraFOVY = 50.0f;

static constexpr glm::vec4 kLightPos{3.0f, 3.0f, 1.5f, 1.0f};

// NOTE: シェーダーのカーネルサイズと一致させる必要があります。
static constexpr std::size_t kKernelSize = 64;

// ********************************************************************************
// Override functions
// ********************************************************************************

void SceneSSAOProto::OnInit() {
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
  gbuffer_.OnInit(width_, height_);

  textures_[WoodTex] = Texture::Load("./Assets/Textures/Wood/wood.jpeg");
  textures_[BrickTex] =
      Texture::Load("./Assets/Textures/Brick/ruin_wall_01.png");
}

void SceneSSAOProto::OnDestroy() {
  glDeleteVertexArrays(1, &quadVAO_);
  glDeleteBuffers(1, &quadVBO_);
}

void SceneSSAOProto::OnUpdate(float) {}

void SceneSSAOProto::OnRender() {
  Pass1();
  Pass2();
  Pass3();
  Pass4();
}

void SceneSSAOProto::OnResize(int w, int h) {
  SetDimensions(w, h);
  glViewport(0, 0, w, h);
}

// ********************************************************************************
// Member functions
// ********************************************************************************

void SceneSSAOProto::CreateVAO() {
  const float quadVertices[] = {
      // positions        // texture Coords
      -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
      1.0f,  1.0f, 0.0f, 1.0f, 1.0f, 1.0f,  -1.0f, 0.0f, 1.0f, 0.0f,
  };

  // 頂点配列オブジェクトの設定
  glGenVertexArrays(1, &quadVAO_);
  glGenBuffers(1, &quadVBO_);
  glBindVertexArray(quadVAO_);

  glBindBuffer(GL_ARRAY_BUFFER, quadVBO_);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices,
               GL_STATIC_DRAW);
  glEnableVertexAttribArray(0); // VertexPosition
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), nullptr);
  glEnableVertexAttribArray(1); // TexCoord
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        reinterpret_cast<void *>(3 * sizeof(float)));

  glBindVertexArray(0);
}

void SceneSSAOProto::SetMatrices() {
  const glm::mat4 view = camera_.GetViewMatrix();
  const glm::mat4 proj = camera_.GetProjectionMatrix();
  const glm::mat4 mv = view * model_;

  progs_[RecordGBufferPass].SetUniform("ModelViewMatrix", mv);
  progs_[RecordGBufferPass].SetUniform("NormalMatrix", glm::mat3(mv));
  progs_[RecordGBufferPass].SetUniform("MVP", proj * mv);
}

std::optional<std::string> SceneSSAOProto::CompileAndLinkShader() {
  // Compile and links
  if (const auto msg = progs_[RecordGBufferPass].CompileAndLink(
          {{"./Assets/Shaders/SSAO/GBuffer.vs.glsl", ShaderType::Vertex},
           {"./Assets/Shaders/SSAO/GBuffer.fs.glsl", ShaderType::Fragment}})) {
    return msg;
  }
  if (const auto msg = progs_[SSAOPass].CompileAndLink(
          {{"./Assets/Shaders/SSAO/SSAO.vs.glsl", ShaderType::Vertex},
           {"./Assets/Shaders/SSAO/SSAO.fs.glsl", ShaderType::Fragment}})) {
    return msg;
  }
  if (const auto msg = progs_[BlurPass].CompileAndLink(
          {{"./Assets/Shaders/SSAO/SSAO.vs.glsl", ShaderType::Vertex},
           {"./Assets/Shaders/SSAO/Blur.fs.glsl", ShaderType::Fragment}})) {
    return msg;
  }
  if (const auto msg = progs_[LightingPass].CompileAndLink(
          {{"./Assets/Shaders/SSAO/SSAO.vs.glsl", ShaderType::Vertex},
           {"./Assets/Shaders/SSAO/Lighting.fs.glsl", ShaderType::Fragment}})) {
    return msg;
  }
  return std::nullopt;
}

void SceneSSAOProto::SetupShaderConfig() {
  progs_[RecordGBufferPass].Use();
  progs_[RecordGBufferPass].SetUniform("DiffTex", 0);

  progs_[SSAOPass].Use();
  progs_[SSAOPass].SetUniform("PositionTex", 0);
  progs_[SSAOPass].SetUniform("NormalTex", 1);
  progs_[SSAOPass].SetUniform("RandRotTex", 2);

  progs_[BlurPass].Use();
  progs_[BlurPass].SetUniform("AOTex", 0);

  progs_[LightingPass].Use();
  progs_[LightingPass].SetUniform("PositionTex", 0);
  progs_[LightingPass].SetUniform("NormalTex", 1);
  progs_[LightingPass].SetUniform("ColorTex", 2);
}

// ********************************************************************************
// Render
// ********************************************************************************

void SceneSSAOProto::Pass1() {
  glBindFramebuffer(GL_FRAMEBUFFER, gbuffer_.GetDeferredFBO());
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);

  progs_[RecordGBufferPass].Use();
  DrawScene();

  glDisable(GL_DEPTH_TEST);
}

void SceneSSAOProto::Pass2() {}

void SceneSSAOProto::Pass3() {}

void SceneSSAOProto::Pass4() {
  // デフォルトのフレームバッファに戻します
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glClear(GL_COLOR_BUFFER_BIT);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, gbuffer_.GetPosTex());
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, gbuffer_.GetNormTex());
  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, gbuffer_.GetColorTex());

  progs_[LightingPass].Use();
  progs_[LightingPass].SetUniform("Light.Position",
                                  camera_.GetViewMatrix() * kLightPos);
  progs_[LightingPass].SetUniform("Light.L", glm::vec3(0.3f));
  progs_[LightingPass].SetUniform("Light.La", glm::vec3(0.5f));

  DrawQuad();
}

// ********************************************************************************
// Render subroutine
// ********************************************************************************

void SceneSSAOProto::DrawScene() {
  // 床の描画
  auto DrawFloor = [this]() {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures_[WoodTex]);

    progs_[RecordGBufferPass].SetUniform("Material.UseTex", 1);
    model_ = glm::mat4(1.0f);
    SetMatrices();

    plane_.Render();
  };

  // 壁の描画1
  auto DrawWallL = [this]() {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures_[BrickTex]);

    model_ = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -2.0f));
    model_ =
        glm::rotate(model_, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    SetMatrices();

    plane_.Render();
  };

  // 壁の描画2
  auto DrawWallR = [this]() {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures_[BrickTex]);

    model_ = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, 0.0f, 0.0f));
    model_ =
        glm::rotate(model_, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    model_ =
        glm::rotate(model_, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    SetMatrices();

    plane_.Render();
  };

  // メインメッシュの描画
  auto DrawMesh = [this]() {
    progs_[RecordGBufferPass].SetUniform("Material.UseTex", 0);
    progs_[RecordGBufferPass].SetUniform("Material.Kd",
                                         glm::vec3(0.9f, 0.5f, 0.2f));
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

void SceneSSAOProto::DrawQuad() {
  glBindVertexArray(quadVAO_);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  glBindVertexArray(0);
}
