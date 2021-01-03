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

#include "HID/KeyInput.h"
#include "UI/Text.h"

// ********************************************************************************
// Override functions
// ********************************************************************************

void SceneMSAA::OnInit() {
  KeyInput::Create();
  Text::Create();
  Font::Create();

  if ((fontObj_ = Font::Get().Entry(
           "./Assets/Fonts/UbuntuMono/UbuntuMono-Regular.ttf"))) {
    fontObj_->SetupWithSize(20);
  }

  if (const auto msg = CompileAndLinkShader()) {
    std::cerr << msg.value() << std::endl;
    BOOST_ASSERT_MSG(false, "failed to compile or link!");
  }

  CreateVAO();

#if !NDEBUG
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
  const float deltaT = tPrev_ == 0.0f ? 0.0f : t - tPrev_;
  tPrev_ = t;

  angle_ += kRotSpeed * deltaT;
  if (angle_ > glm::two_pi<float>()) {
    angle_ -= glm::two_pi<float>();
  }

  if (KeyInput::Get().IsTrg(Key::Left) || KeyInput::Get().IsTrg(Key::Right)) {
    isEnabledMSAA_ = !isEnabledMSAA_;
  }
  if (KeyInput::Get().IsTrg(Key::Up) || KeyInput::Get().IsTrg(Key::Down)) {
    isEnabledCentroid_ = !isEnabledCentroid_;
  }
}

void SceneMSAA::OnRender() {
  DrawQuad();
  DrawText();
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

// ********************************************************************************
// Drawing
// ********************************************************************************

void SceneMSAA::DrawQuad() {
  glEnable(GL_DEPTH_TEST);
  if (isEnabledMSAA_) {
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

  if (isEnabledCentroid_) {
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

void SceneMSAA::DrawText() {
  if (!fontObj_) {
    return;
  }
  const float kBaseX = 25.0f;
  const float kBaseY = static_cast<float>(height_) - 30.0f;
  const float kOffsetY = -30.0f;
  const float kOffsetYL2 = -50.0f;

  Text::Get().Begin(width_, height_);
  {
    if (isEnabledMSAA_) {
      Text::Get().Render("MSAA: ON", kBaseX, kBaseY, fontObj_);
      Text::Get().Render("Press left or right arrow key: Disable MSAA", kBaseX,
                         kBaseY + kOffsetY, fontObj_);
    } else {
      Text::Get().Render("MSAA: OFF", kBaseX, kBaseY, fontObj_);
      Text::Get().Render("Press left or right arrow key: Enable MSAA", kBaseX,
                         kBaseY + kOffsetY, fontObj_);
    }
    const float kCentroidBaseX = kBaseX + 120.0f;
    if (isEnabledCentroid_) {
      Text::Get().Render("Centroid: ON", kCentroidBaseX, kBaseY, fontObj_);
      Text::Get().Render("Press up or down arrow key: Disable Centroid", kBaseX,
                         kBaseY + kOffsetYL2, fontObj_);
    } else {
      Text::Get().Render("Centroid: OFF", kCentroidBaseX, kBaseY, fontObj_);
      Text::Get().Render("Press up or down arrow key: Enable Centroid", kBaseX,
                         kBaseY + kOffsetYL2, fontObj_);
    }
  }
  Text::Get().End();
}
