/**
 * @brief Scene HelloTriangle Class
 */

// ********************************************************************************
// Including files
// ********************************************************************************

#include <boost/assert.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include "SceneMSAA.h"

#include "GUI/GUI.h"
#include "UI/Text.h"

// ********************************************************************************
// Override functions
// ********************************************************************************

void SceneMSAA::OnInit() {
  if (const auto msg = CompileAndLinkShader()) {
    std::cerr << msg.value() << std::endl;
    BOOST_ASSERT_MSG(false, "failed to compile or link!");
  }

  CreateVAO();

#if !defined(NDEBUG)
  GLint bufs, samples;
  glGetIntegerv(GL_SAMPLE_BUFFERS, &bufs);
  glGetIntegerv(GL_SAMPLES, &samples);
  std::cout << "MSAA Buffers = " << bufs << ", Samples = " << samples
            << std::endl;
#endif
}

void SceneMSAA::OnDestroy() {
  glDeleteVertexArrays(1, &quad_);
  glDeleteBuffers(static_cast<GLsizei>(vbo_.size()), vbo_.data());
}

void SceneMSAA::OnUpdate(float t) {
  UpdateGUI();

  const float deltaT = tPrev_ == 0.0f ? 0.0f : t - tPrev_;
  tPrev_ = t;

  angle_ += param_.rotSpeed * deltaT;
  if (angle_ > glm::two_pi<float>()) {
    angle_ -= glm::two_pi<float>();
  }
}

void SceneMSAA::OnRender() {
  DrawQuad();
  GUI::Render();
}

void SceneMSAA::OnResize(int w, int h) {
  SetDimensions(w, h);
  glViewport(0, 0, w, h);
}

// ********************************************************************************
// Shader prepare
// ********************************************************************************

std::optional<std::string> SceneMSAA::CompileAndLinkShader() {
  // compile and links
  if (const auto msg = centroid_.CompileAndLink(
          {{"./Assets/Shaders/MSAA/Centroid.vs.glsl", ShaderType::Vertex},
           {"./Assets/Shaders/MSAA/Centroid.fs.glsl", ShaderType::Fragment}})) {
    return msg;
  }
  if (const auto msg = noCentroid_.CompileAndLink(
          {{"./Assets/Shaders/MSAA/NoCentroid.vs.glsl", ShaderType::Vertex},
           {"./Assets/Shaders/MSAA/NoCentroid.fs.glsl",
            ShaderType::Fragment}})) {
    return msg;
  }
  return std::nullopt;
}

void SceneMSAA::CreateVAO() {
  const float pos[] = {-1.0f, -1.0f, 0.0f, 1.0f,  -1.0f, 0.0f,
                       1.0f,  1.0f,  0.0f, -1.0f, -1.0f, 0.0f,
                       1.0f,  1.0f,  0.0f, -1.0f, 1.0f,  0.0f};
  const float tc[] = {0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
                      0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f};

  glGenBuffers(static_cast<GLsizei>(vbo_.size()), vbo_.data());

  glBindBuffer(GL_ARRAY_BUFFER, vbo_[Position]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(pos), pos, GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, vbo_[TexCoord]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(tc), tc, GL_STATIC_DRAW);

  glGenVertexArrays(1, &quad_);
  glBindVertexArray(quad_);

  glBindBuffer(GL_ARRAY_BUFFER, vbo_[Position]);
  glVertexAttribPointer(Position, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
  glEnableVertexAttribArray(Position);

  glBindBuffer(GL_ARRAY_BUFFER, vbo_[TexCoord]);
  glVertexAttribPointer(TexCoord, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
  glEnableVertexAttribArray(TexCoord);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void SceneMSAA::UpdateGUI() {
  GUI::NewFrame();

  ImGui::Begin("MSAA Config");
  ImGui::Checkbox("MSAA ON", &param_.isEnabledMSAA);
  ImGui::Checkbox("Centroid ON", &param_.isEnabledCentroid);
  ImGui::SliderFloat("Rotate Speed", &param_.rotSpeed, 0.0f, 1.0f);
  ImGui::End();
}

// ********************************************************************************
// Drawing
// ********************************************************************************

void SceneMSAA::DrawQuad() {
  glEnable(GL_DEPTH_TEST);
  if (param_.isEnabledMSAA) {
    glEnable(GL_MULTISAMPLE);
  }

  glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  static constexpr float kCenter = 5.0f;
  proj_ = glm::ortho(-0.4f * kCenter, 0.4f * kCenter, -0.3f * kCenter,
                     0.3f * kCenter, 0.1f, 100.0f);
  view_ = glm::lookAt(glm::vec3(3.0f * cos(angle_), 0.0f, 3.0f * sin(angle_)),
                      glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
  model_ = glm::rotate(glm::mat4(1.0f), glm::radians(30.0f),
                       glm::vec3(0.0f, 0.0f, 1.0f));

  if (param_.isEnabledCentroid) {
    centroid_.Use();
    centroid_.SetUniform("MVP", proj_ * view_ * model_);
  } else {
    noCentroid_.Use();
    noCentroid_.SetUniform("MVP", proj_ * view_ * model_);
  }

  glBindVertexArray(quad_);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glBindVertexArray(0);

  glDisable(GL_MULTISAMPLE);
  glDisable(GL_DEPTH_TEST);
}
