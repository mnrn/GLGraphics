/**
 * @brief 物理ベースレンダリングのテストシーン
 */

#include "ScenePBR.h"

#include <boost/assert.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <map>

#include "GUI/GUI.h"

// ********************************************************************************
// Constant expressions
// ********************************************************************************

static constexpr float kFOVY = 60.0f;
static const std::map<MetalColor, glm::vec3> kMetalLinearRGB{
    {MetalColor::Nil, glm::vec3(0.0f, 0.0f, 0.0f)},
    {MetalColor::Iron, glm::vec3(0.560f, 0.570f, 0.580f)},
    {MetalColor::Silver, glm::vec3(0.972f, 0.960f, 0.915f)},
    {MetalColor::Aluminum, glm::vec3(0.913f, 0.921f, 0.925f)},
    {MetalColor::Gold, glm::vec3(1.000f, 0.766f, 0.336f)},
    {MetalColor::Copper, glm::vec3(0.955f, 0.637f, 0.538f)},
    {MetalColor::Chromium, glm::vec3(0.550f, 0.556f, 0.556f)},
    {MetalColor::Nickel, glm::vec3(0.660f, 0.609f, 0.526f)},
    {MetalColor::Titanium, glm::vec3(0.542f, 0.497f, 0.449f)},
    {MetalColor::Cobalt, glm::vec3(0.662f, 0.665f, 0.634f)},
    {MetalColor::Platinum, glm::vec3(0.672f, 0.637f, 0.585f)},
};

// ********************************************************************************
// Special member functions
// ********************************************************************************

ScenePBR::ScenePBR()
    : lightPositions_{glm::vec4(7.0f, 3.0f, 0.0f, 1.0f),
                      glm::vec4(0.0f, 1.5f, 0.0f, 0.0f),
                      glm::vec4(0.0f, 1.5f, 3.0f, 1.0f)} {}

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
    prog_.SetUniform("Light[1].L", glm::vec3(5.0f));
    prog_.SetUniform("Light[1].Position", lightPositions_[1]);
    prog_.SetUniform("Light[2].L", glm::vec3(45.0f));
    prog_.SetUniform("Light[2].Position", view_ * lightPositions_[2]);
  }

  glEnable(GL_DEPTH_TEST);
}

void ScenePBR::OnUpdate(float t) {
  UpdateGUI();

  if (param_.metalColor != MetalColor::Nil) {
    param_.metalSpecular = kMetalLinearRGB.at(param_.metalColor);
  }

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
  static const std::map<MetalColor, const char *> kMetalNames{
      {MetalColor::Nil, "Origin"},         {MetalColor::Silver, "Silver"},
      {MetalColor::Aluminum, "Aluminium"}, {MetalColor::Titanium, "Titanium"},
      {MetalColor::Iron, "Iron"},          {MetalColor::Platinum, "Platinum"},
      {MetalColor::Gold, "Gold"},          {MetalColor::Nickel, "Nickel"},
      {MetalColor::Copper, "Copper"},      {MetalColor::Chromium, "Chromium"},
      {MetalColor::Cobalt, "Cobalt"},
  };
  GUI::NewFrame();

  ImGui::Begin("PBR Config");
  if (ImGui::ColorEdit3("Metal Specular",
                        reinterpret_cast<float *>(&param_.metalSpecular))) {
    param_.metalColor = MetalColor::Nil;
  }
  ImGui::Text("Select Metal Specular");
  for (size_t i = 0; i < kMetalLinearRGB.size(); i++) {
    ImGui::RadioButton(kMetalNames.at(static_cast<MetalColor>(i)),
                       reinterpret_cast<int *>(&param_.metalColor), i);
    if (i + 1 != kMetalLinearRGB.size()) {
      ImGui::SameLine();
    }
  }
  ImGui::SliderFloat("Metal Roughness", &param_.metalRough, 0.0f, 1.0f);
  ImGui::ColorEdit3("Dielectric Base Color (Non-Metal Diffuse Albedo)",
                    reinterpret_cast<float *>(&param_.dielectricBaseColor));
  ImGui::SliderFloat("Dielectric Roughness", &param_.dielectricRough, 0.0f,
                     1.0f);
  ImGui::SliderFloat("Dielectric Reflectance", &param_.dielectricReflectance,
                     0.0f, 1.0f);
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
  prog_.SetUniform("Material.Roughness", 1.0f);
  prog_.SetUniform("Material.Metallic", 0.0f);
  prog_.SetUniform("Material.Reflectance", 1.0f);
  prog_.SetUniform("Material.BaseColor", glm::vec3(0.0f));
  model_ = glm::translate(model_, glm::vec3(0.0f, -3.0f, 0.0f));
  SetMatrices();

  plane_.Render();
}

void ScenePBR::DrawMesh(const glm::vec3 &pos, float rough, int metal,
                        const glm::vec3 &color) {
  prog_.SetUniform("Material.Roughness", rough);
  prog_.SetUniform("Material.Metallic", static_cast<float>(metal));
  prog_.SetUniform("Material.Reflectance", param_.dielectricReflectance);
  prog_.SetUniform("Material.BaseColor", color);
  model_ = glm::translate(glm::mat4(1.0f), pos);
  model_ =
      glm::rotate(model_, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
  model_ = glm::scale(model_, glm::vec3(3.0f));

  SetMatrices();

  mesh_->Render();
}
