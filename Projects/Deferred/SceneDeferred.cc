/**
 * @brief 遅延レンダリングのテストシーン
 */

#include "SceneDeferred.h"

#include <boost/assert.hpp>
#include <iostream>

#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

// ********************************************************************************
// Override functions
// ********************************************************************************

void SceneDeferred::OnInit() {
  if (const auto msg = CompileAndLinkShader()) {
    std::cerr << msg.value() << std::endl;
    BOOST_ASSERT_MSG(false, "failed to compile or link!");
  }

  glEnable(GL_DEPTH_TEST);

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

  gbuffer_.OnInit(width_, height_);

  prog_.SetUniform("Light.Ld", glm::vec3(1.0f));

  prog_.SetUniform("PositionTex", 0);
  prog_.SetUniform("NormalTex", 1);
  prog_.SetUniform("ColorTex", 2);

}

void SceneDeferred::OnDestroy() {
  glDeleteVertexArrays(1, &quad_);
  glDeleteBuffers(vbo_.size(), vbo_.data());
}

void SceneDeferred::OnUpdate(float t) {
  const float deltaT = tPrev_ == 0.0f ? 0.0f : t - tPrev_;
  tPrev_ = t;

  angle_ += rotSpeed_ * deltaT;
  if (angle_ > glm::two_pi<float>()) {
    angle_ -= glm::two_pi<float>();
  }
}

void SceneDeferred::OnRender() {
  Pass1();
  Pass2();
}

void SceneDeferred::OnResize(int w, int h) {
  SetDimensions(w, h);
  glViewport(0, 0, w, h);
}

// ********************************************************************************
// Member functions
// ********************************************************************************

void SceneDeferred::SetMatrices() {
  const glm::mat4 mv = view_ * model_;
  prog_.SetUniform("ModelViewMatrix", mv);
  prog_.SetUniform("NormalMatrix", glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]),
                                             glm::vec3(mv[2])));
  prog_.SetUniform("MVP", proj_ * mv);
}

std::optional<std::string> SceneDeferred::CompileAndLinkShader() {
  // Compile and links
  if (prog_.Compile("./Assets/Shaders/Deferred/Deferred.vs.glsl",
                    ShaderType::Vertex) &&
      prog_.Compile("./Assets/Shaders/Deferred/Deferred.fs.glsl",
                    ShaderType::Fragment) &&
      prog_.Link()) {
    prog_.Use();
    return std::nullopt;
  } else {
    return prog_.GetLog();
  }
}

void SceneDeferred::Pass1() {
  prog_.SetUniform("Pass", 1);

  glBindFramebuffer(GL_FRAMEBUFFER, gbuffer_.GetDeferredFBO());
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);

  view_ = glm::lookAt(glm::vec3(7.0f * cos(angle_), 4.0f, 7.0f * sin(angle_)),
                      glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
  proj_ = glm::perspective(glm::radians(60.0f),
                           static_cast<float>(width_) / height_, 0.3f, 100.0f);

  // ティーポットの描画
  prog_.SetUniform("Light.Position", glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
  prog_.SetUniform("Matrial.Kd", 0.9f, 0.9f, 0.9f);
  model_ = glm::mat4(1.0f);
  model_ = glm::translate(model_, glm::vec3(0.0f, 0.0f, 0.0f));
  model_ =
      glm::rotate(model_, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
  SetMatrices();
  teapot_.Render();

  // 平面の描画
  prog_.SetUniform("Material.Kd", 0.4f, 0.4f, 0.4f);
  model_ = glm::mat4(1.0f);
  model_ = glm::translate(model_, glm::vec3(0.0f, -0.75f, 0.0f));
  SetMatrices();
  plane_.Render();

  // トーラスの描画
  prog_.SetUniform("Light.Position", glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
  prog_.SetUniform("Material.Kd", 0.9f, 0.5f, 0.2f);
  model_ = glm::mat4(1.0f);
  model_ = glm::translate(model_, glm::vec3(3.0f, 1.0f, 3.0f));
  model_ =
      glm::rotate(model_, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
  SetMatrices();
  torus_.Render();

  glFinish();
}

void SceneDeferred::Pass2() {
  prog_.SetUniform("Pass", 2);

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
