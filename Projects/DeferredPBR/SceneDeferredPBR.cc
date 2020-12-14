/**
 * @brief PBRの遅延レンダリングのテストシーン
 */

#include "SceneDeferredPBR.h"

#include <boost/assert.hpp>
#include <iostream>

#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

// ********************************************************************************
// Override functions
// ********************************************************************************

void SceneDeferredPBR::OnInit() {
  if (const auto msg = CompileAndLinkShader()) {
    std::cerr << msg.value() << std::endl;
    BOOST_ASSERT_MSG(false, "failed to compile or link!");
  }

  glEnable(GL_DEPTH_TEST);
  view_ = glm::lookAt(glm::vec3(0.0f, 3.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f),
                      glm::vec3(0.0f, 1.0f, 0.0f));
  proj_ = glm::perspective(glm::radians(kFOVY),
                           static_cast<float>(width_) / height_, 0.1f, 100.0f);

  CreateVAO();

  gbuffer_.OnInit(width_, height_);

  lightPositions_ = {glm::vec4(7.0f, 3.0f, 0.0f, 1.0f),
                      glm::vec4(0.0f, -0.15f, 0.0f, 0.0f),
                      glm::vec4(-8.0f, 3.0f, 0.0f, 1.0f)};

  prog_.SetUniform("Light[0].L", glm::vec3(45.0f));
  prog_.SetUniform("Light[0].Position", view_ * lightPositions_[0]);
  prog_.SetUniform("Light[1].L", glm::vec3(0.1f));
  prog_.SetUniform("Light[1].Position", lightPositions_[1]);
  prog_.SetUniform("Light[2].L", glm::vec3(30.0f));
  prog_.SetUniform("Light[2].Position", view_ * lightPositions_[2]);

  prog_.SetUniform("PositionTex", 0);
  prog_.SetUniform("NormalTex", 1);
  prog_.SetUniform("ColorTex", 2);
}

void SceneDeferredPBR::OnDestroy() {
  glDeleteVertexArrays(1, &quad_);
  glDeleteBuffers(static_cast<GLsizei>(vbo_.size()), vbo_.data());
}

void SceneDeferredPBR::OnUpdate(float t) {
  const float deltaT = tPrev_ == 0.0f ? 0.0f : t - tPrev_;
  tPrev_ = t;

  if (IsAnimate()) {
    lightAngle_ = glm::mod(lightAngle_ + deltaT * kLightRotationSpeed,
                           glm::two_pi<float>());
    lightPositions_[0] =
        glm::vec4(glm::cos(lightAngle_) * 7.0f, 3.0f,
                  glm::sin(lightAngle_) * 7.0f, lightPositions_[0].w);
  }
}

void SceneDeferredPBR::OnRender() {
  Pass1();
  Pass2();
}

void SceneDeferredPBR::OnResize(int w, int h) {
  SetDimensions(w, h);
  glViewport(0, 0, w, h);
  proj_ = glm::perspective(glm::radians(kFOVY), static_cast<float>(w) / h, 0.3f,
                           100.0f);
}

// ********************************************************************************
// Shader settings
// ********************************************************************************

void SceneDeferredPBR::SetMatrices() {
  const glm::mat4 mv = view_ * model_;
  prog_.SetUniform("ModelViewMatrix", mv);
  prog_.SetUniform("NormalMatrix", glm::mat3(mv));
  prog_.SetUniform("MVP", proj_ * mv);
}

std::optional<std::string> SceneDeferredPBR::CompileAndLinkShader() {
  // Compile and links
  if (prog_.Compile("./Assets/Shaders/DeferredPBR/DeferredPBR.vs.glsl",
                    ShaderType::Vertex) &&
      prog_.Compile("./Assets/Shaders/DeferredPBR/DeferredPBR.fs.glsl",
                    ShaderType::Fragment) &&
      prog_.Link()) {
    prog_.Use();
    return std::nullopt;
  } else {
    return prog_.GetLog();
  }
}

void SceneDeferredPBR::CreateVAO() {
  // 四角形ポリゴン用配列
  GLfloat verts[] = {-1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 1.0f,  1.0f, 0.0f,
                     -1.0f, -1.0f, 0.0f, 1.0f, 1.0f,  0.0f, -1.0f, 1.0f, 0.0f};
  GLfloat tc[] = {0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
                  0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f};

  glGenBuffers(vbo_.size(), vbo_.data());

  glBindBuffer(GL_ARRAY_BUFFER, vbo_[0]);
  glBufferData(GL_ARRAY_BUFFER, 6 * 3 * sizeof(float), verts, GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, vbo_[1]);
  glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(float), tc, GL_STATIC_DRAW);

  // 頂点配列オブジェクトの設定
  glGenVertexArrays(1, &quad_);
  glBindVertexArray(quad_);

  glBindBuffer(GL_ARRAY_BUFFER, vbo_[0]);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(0); // VertexPosition

  glBindBuffer(GL_ARRAY_BUFFER, vbo_[1]);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(2); // TexCoord

  glBindVertexArray(0);
}

// ********************************************************************************
// Drawing
// ********************************************************************************

void SceneDeferredPBR::Pass1() {
  prog_.SetUniform("Pass", 1);

  glBindFramebuffer(GL_FRAMEBUFFER, gbuffer_.GetDeferredFBO());
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);

  view_ = glm::lookAt(glm::vec3(0.0f, 4.0f, 7.0f), glm::vec3(0.0f, 0.0f, 0.0f),
                      glm::vec3(0.0f, 1.0f, 0.0f));
  proj_ = glm::perspective(glm::radians(kFOVY),
                           static_cast<float>(width_) / height_, 0.3f, 100.0f);
  DrawScene();
}

void SceneDeferredPBR::Pass2() {
  prog_.SetUniform("Pass", 2);
  prog_.SetUniform("Light[0].Position", view_ * lightPositions_[0]);

  // デフォルトのフレームバッファに戻します
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glDisable(GL_DEPTH_TEST);

  view_ = glm::mat4(1.0f);
  proj_ = glm::mat4(1.0f);
  model_ = glm::mat4(1.0f);
  SetMatrices();

  // 四角形ポリゴンとして描画していく
  glBindVertexArray(quad_);
  glDrawArrays(GL_TRIANGLES, 0, 6);
}

bool SceneDeferredPBR::IsAnimate() const { return true; }

void SceneDeferredPBR::DrawScene() {
  DrawFloor();

  for (int i = 0; i < kNumCows; i++) {
    const float cowX = i * (20.0f / (kNumCows - 1)) - 10.0f;
    const float rough = (i + 1) * (1.0f / kNumCows);
    DrawMesh(glm::vec3(cowX, 0.0f, 0.0f), rough, 0, kBaseCowColor);
  }

  const std::vector<glm::vec3> kMetalColors = {
      glm::vec3(1.0f, 0.71f, 0.29f),    // Gold
      glm::vec3(0.95f, 0.64f, 0.54f),   // Copper
      glm::vec3(0.91f, 0.92f, 0.92f),   // Aluminum
      glm::vec3(0.542f, 0.497, 0.449f), // Titanium
      glm::vec3(0.95f, 0.93f, 0.88f)    // Silver
  };
  const float kOffsetX = 2.5f;
  float cowX = -5.0f;
  for (int i = 0; i < 5; i++) {
    DrawMesh(glm::vec3(cowX, 0.0f, 3.0f), kMetalRough, 1, kMetalColors[i]);
    cowX += kOffsetX;
  }
}

void SceneDeferredPBR::DrawFloor() {
  model_ = glm::mat4(1.0f);
  prog_.SetUniform("Material.Roughness", 0.9f);
  prog_.SetUniform("Material.Metallic", 0);
  prog_.SetUniform("Material.Color", glm::vec3(0.0f));
  model_ = glm::translate(model_, glm::vec3(0.0f, -0.75f, 0.0f));
  SetMatrices();
  plane_.Render();
}

void SceneDeferredPBR::DrawMesh(const glm::vec3 &pos, float rough, int metal,
                                const glm::vec3 &color) {
  model_ = glm::mat4(1.0f);
  prog_.SetUniform("Material.Roughness", rough);
  prog_.SetUniform("Material.metallic", metal);
  prog_.SetUniform("Material.Color", color);
  model_ = glm::translate(model_, pos);
  model_ =
      glm::rotate(model_, glm::radians(200.0f), glm::vec3(0.0f, 1.0f, 0.0f));
  model_ = glm::scale(model_, glm::vec3(0.5f, 0.5f, 0.5f));
  SetMatrices();

  teapot_->Render();
}
