/**
 * @brief 物理ベースレンダリングのテストシーン
 */

#include "ScenePBR.h"

#include <boost/assert.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include "GUI/GUI.h"

// ********************************************************************************
// Constant expressions
// ********************************************************************************

static constexpr float kFOVY = 60.0f;
static const std::vector<glm::vec3> kMetalColors = {
    glm::vec3(1.0f, 0.71f, 0.29f),    // Gold
    glm::vec3(0.95f, 0.64f, 0.54f),   // Copper
    glm::vec3(0.91f, 0.92f, 0.92f),   // Aluminum
    glm::vec3(0.542f, 0.497, 0.449f), // Titanium
    glm::vec3(0.95f, 0.93f, 0.88f)    // Silver
};

// ********************************************************************************
// Special member functions
// ********************************************************************************

ScenePBR::ScenePBR()
    : lightPositions_{glm::vec4(7.0f, 3.0f, 0.0f, 1.0f),
                      glm::vec4(0.0f, 0.15f, 0.0f, 0.0f),
                      glm::vec4(-7.0f, 3.0f, 7.0f, 1.0f)} {}

// ********************************************************************************
// Override functions
// ********************************************************************************

void ScenePBR::OnInit() {
  view_ = glm::lookAt(glm::vec3(0.0f, 2.0f, 7.0f), glm::vec3(0.0f, 0.0f, 0.0f),
                      glm::vec3(0.0f, 1.0f, 0.0f));
  proj_ = glm::perspective(
      glm::radians(kFOVY),
      static_cast<float>(width_) / static_cast<float>(height_), 0.3f, 100.0f);

  if (const auto msg = CompileAndLinkShader()) {
    std::cerr << msg.value() << std::endl;
    BOOST_ASSERT_MSG(false, "Failed to compile or link.");
  } else {
    prog_.Use();
    prog_.SetUniform("Light[0].L", glm::vec3(45.0f));
    prog_.SetUniform("Light[0].Position", view_ * lightPositions_[0]);
    prog_.SetUniform("Light[1].L", glm::vec3(0.3f));
    prog_.SetUniform("Light[1].Position", lightPositions_[1]);
    prog_.SetUniform("Light[2].L", glm::vec3(45.0f));
    prog_.SetUniform("Light[2].Position", view_ * lightPositions_[2]);
  }

  glEnable(GL_DEPTH_TEST);
}

void ScenePBR::OnUpdate(float t) {
  UpdateGUI();

  const float deltaT = tPrev_ == 0.0f ? 0.0f : t - tPrev_;
  tPrev_ = t;

  lightAngle_ = glm::mod(lightAngle_ + deltaT * lightRotationSpeed_,
                         glm::two_pi<float>());
  lightPositions_[0] =
      glm::vec4(glm::cos(lightAngle_) * 7.0f, 3.0f,
                glm::sin(lightAngle_) * 7.0f, lightPositions_[0].w);
}

void ScenePBR::OnRender() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  prog_.SetUniform("Light[0].Position", view_ * lightPositions_[0]);
  DrawScene();

  GUI::Render();
}

void ScenePBR::OnResize(int w, int h) {
  glViewport(0, 0, w, h);
  SetDimensions(w, h);
}

// ********************************************************************************
// Member functions
// ********************************************************************************

void ScenePBR::SetMatrices() {
  const glm::mat4 mv = view_ * model_;
  prog_.SetUniform("ModelViewMatrix", mv);
  prog_.SetUniform("NormalMatrix", glm::mat3(mv));
  prog_.SetUniform("MVP", proj_ * mv);
}

std::optional<std::string> ScenePBR::CompileAndLinkShader() {
  // Compile and links
  return prog_.CompileAndLink(
      {{"./Assets/Shaders/PBR/PBR.vs.glsl", ShaderType::Vertex},
       {"./Assets/Shaders/PBR/PBR.fs.glsl", ShaderType::Fragment}});
}

void ScenePBR::UpdateGUI() {
  GUI::NewFrame();

  ImGui::Begin("PBR Config");
  ImGui::ColorEdit3("Metal Specular",
                    reinterpret_cast<float *>(&param_.metalSpecular));
  ImGui::SliderFloat("Metal Roughness", &param_.metalRough, 0.0f, 1.0f);
  ImGui::ColorEdit3("Dielectric Base Color (Non-Metal Albedo)",
                    reinterpret_cast<float *>(&param_.dielectricBaseColor));
  ImGui::SliderFloat("Dielectric Roughness", &param_.dielectricRough, 0.0f,
                     1.0f);
  ImGui::End();
}

void ScenePBR::DrawScene() {
  DrawFloor();

  DrawMesh(glm::vec3(3.0f, 0.0f, 0.0f), param_.dielectricRough, 0,
           param_.dielectricBaseColor);
  DrawMesh(glm::vec3(-3.0, 0.0f, 0.0f), param_.metalRough, 1,
           param_.metalSpecular);
}

void ScenePBR::DrawFloor() {
  model_ = glm::mat4(1.0f);
  prog_.SetUniform("Material.Roughness", 0.9f);
  prog_.SetUniform("Material.Metallic", 0);
  prog_.SetUniform("Material.Color", glm::vec3(0.0f));
  model_ = glm::translate(model_, glm::vec3(0.0f, -3.0f, 0.0f));
  SetMatrices();

  plane_.Render();
}

void ScenePBR::DrawMesh(const glm::vec3 &pos, float rough, int metal,
                        const glm::vec3 &color) {
  prog_.SetUniform("Material.Roughness", rough);
  prog_.SetUniform("Material.Metallic", metal);
  prog_.SetUniform("Material.Color", color);
  model_ = glm::translate(glm::mat4(1.0f), pos);
  model_ =
      glm::rotate(model_, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
  model_ = glm::scale(model_, glm::vec3(3.0f));

  SetMatrices();

  mesh_->Render();
}
