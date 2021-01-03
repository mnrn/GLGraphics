/**
 * @brief コンピュートシェーダー入門
 */

// ********************************************************************************
// Including files
// ********************************************************************************

#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>

#include "SceneParticles.h"

// ********************************************************************************
// Overrided functions
// ********************************************************************************

void SceneParticles::OnInit() {
  if (CompileAndLinkShader() == false) {
    std::exit(EXIT_FAILURE);
  }

  InitBuffer();

  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void SceneParticles::OnDestroy() {
  glDeleteVertexArrays(1, &hBlackHoleVAO_);
  glDeleteBuffers(1, &hBlackHoleBuffer_);
  glDeleteVertexArrays(1, &hParticlesVAO_);
  glDeleteBuffers(computeBuffer_.size(), computeBuffer_.data());
}

void SceneParticles::OnUpdate(float deltaSec) { static_cast<void>(deltaSec); }

void SceneParticles::OnRender() {
  // 重力場の回転
  const glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(angle_),
                                         glm::vec3(0.0f, 0.0f, 1.0f));
  const glm::vec3 blackHole1Pos = glm::vec3(rotation * blackHole1Pos_);
  const glm::vec3 blackHole2Pos = glm::vec3(rotation * blackHole2Pos_);

  // コンピュートシェーダーの実行
  compute_.Use();
  compute_.SetUniform("BlackHole1Pos", blackHole1Pos);
  compute_.SetUniform("BlackHole2Pos", blackHole2Pos);
  glDispatchCompute(totalParticlesNum_ / localSizeX_, 1, 1);
  glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

  // シーンの描画準備
  render_.Use();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  const glm::mat4 proj = glm::perspective(
      glm::radians(50.0f),
      static_cast<float>(width_) / static_cast<float>(height_), 1.0f, 100.0f);
  const glm::mat4 view =
      glm::lookAt(glm::vec3(2.0f, 0.0f, 20.0f), glm::vec3(0.0f, 0.0f, 0.0f),
                  glm::vec3(0.0f, 1.0f, 0.0f));
  const glm::mat4 model = glm::mat4(1.0f);
  render_.SetUniform("MVP", proj * view * model);

  // パーティクルの描画
  glPointSize(1.0f);
  render_.SetUniform("Color", glm::vec4(0.015f, 0.05f, 0.3f, 0.1f));
  glBindVertexArray(hParticlesVAO_);
  glDrawArrays(GL_POINTS, 0, totalParticlesNum_);
  glBindVertexArray(0);

  // ブラックホールの描画
  glPointSize(2.5f);
  GLfloat data[] = {blackHole1Pos.x, blackHole1Pos.y, blackHole1Pos.z,
                    blackHole1Pos.z, blackHole2Pos.x, blackHole2Pos.y,
                    blackHole2Pos.z, blackHole2Pos.z};
  glBindBuffer(GL_ARRAY_BUFFER, hBlackHoleVAO_);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * 8, data);
  render_.SetUniform("Color", glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
  glBindVertexArray(hBlackHoleVAO_);
  glDrawArrays(GL_POINTS, 0, 2);
  glBindVertexArray(0);
}

void SceneParticles::OnResize(int w, int h) {
  SetDimensions(w, h);
  glViewport(0, 0, w, h);
}

// ********************************************************************************
// Initialize
// ********************************************************************************

bool SceneParticles::CompileAndLinkShader() {

  // シーン描画用シェーダープログラム
  if (render_.Compile("./Assets/Shaders/Particles/Particles.vs.glsl",
                      ShaderType::Vertex) == false) {
    std::cerr << "vertex shader failed to Compile." << std::endl;
    std::cerr << render_.GetLog() << std::endl;
    return false;
  }
  if (render_.Compile("./Assets/Shaders/Particles/Particles.fs.glsl",
                      ShaderType::Fragment) == false) {
    std::cerr << "fragment shader failed to Compile." << std::endl;
    std::cerr << render_.GetLog() << std::endl;
    return false;
  }
  if (render_.Link() == false) {
    std::cerr << "render program failed to Link." << std::endl;
    std::cerr << render_.GetLog() << std::endl;
    return false;
  }
#ifndef __APPLE__
  // コンピュートーシェーダープログラム
  if (compute_.Compile("./Assets/Shaders/Particles/Particles.cs.glsl",
                       ShaderType::Compute) == false) {
    std::cerr << "compute shader failed to Compile." << std::endl;
    std::cerr << compute_.GetLog() << std::endl;
    return false;
  }
  if (compute_.Link() == false) {
    std::cerr << "compute program failed to Link." << std::endl;
    std::cerr << compute_.GetLog() << std::endl;
    return false;
  }
#endif
  return true;
}

void SceneParticles::InitBuffer() {

  // 変数の初期化
  std::vector<GLfloat> initPos;
  std::vector<GLfloat> initVel(totalParticlesNum_ * 4, 0.0f);
  glm::vec4 p(0.0f, 0.0f, 0.0f, 1.0f);
  const GLfloat dx = 2.0f / (particlesXNum_ - 1);
  const GLfloat dy = 2.0f / (particlesYNum_ - 1);
  const GLfloat dz = 2.0f / (particlesZNum_ - 1);
  const glm::mat4 transform = glm::translate(
      glm::mat4(1.0f), // Identity matrix
      glm::vec3(
          -1.0f, -1.0f,
          -1.0f)); // 中央のパーティクルが(0, 0, 0)になるように設定します。

  // パーティクルの初期位置を設定します。
  for (int32_t xi = 0; xi < particlesXNum_; xi++) {
    for (int32_t yi = 0; yi < particlesYNum_; yi++) {
      for (int32_t zi = 0; zi < particlesZNum_; zi++) {

        p.x = dx * static_cast<float>(xi);
        p.y = dy * static_cast<float>(yi);
        p.z = dz * static_cast<float>(zi);
        p.w = 1.0f;
        p = transform * p;

        initPos.emplace_back(p.x);
        initPos.emplace_back(p.y);
        initPos.emplace_back(p.z);
        initPos.emplace_back(p.w);
      }
    }
  }

  // コンピュートシェーダー用のバッファを生成し設定します。
  glGenBuffers(computeBuffer_.size(), computeBuffer_.data());
  GLuint bufPos = computeBuffer_[0];
  GLuint bufVel = computeBuffer_[1];

  const GLuint bufSize = totalParticlesNum_ * sizeof(GLfloat) * 4;

  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, bufPos);
  glBufferData(GL_SHADER_STORAGE_BUFFER, bufSize, &initPos[0], GL_DYNAMIC_DRAW);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, bufVel);
  glBufferData(GL_SHADER_STORAGE_BUFFER, bufSize, &initVel[0], GL_DYNAMIC_COPY);

  // パーティクルのVAOを生成し設定します。
  glGenVertexArrays(1, &hParticlesVAO_);
  glBindVertexArray(hParticlesVAO_);
  glBindBuffer(GL_ARRAY_BUFFER, bufPos);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
  glEnableVertexAttribArray(0);
  glBindVertexArray(0);

  // ブラックホール用のVBOとVAOを生成し設定します。
  glGenBuffers(1, &hBlackHoleBuffer_);
  glBindBuffer(GL_ARRAY_BUFFER, hBlackHoleBuffer_);
  GLfloat data[] = {blackHole1Pos_.x, blackHole1Pos_.y, blackHole1Pos_.z,
                    blackHole1Pos_.w, blackHole2Pos_.x, blackHole2Pos_.y,
                    blackHole2Pos_.z, blackHole2Pos_.w};
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 8, data, GL_DYNAMIC_DRAW);
  glGenVertexArrays(1, &hBlackHoleVAO_);
  glBindVertexArray(hBlackHoleVAO_);
  glBindBuffer(GL_ARRAY_BUFFER, hBlackHoleBuffer_);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
  glEnableVertexAttribArray(0);
  glBindVertexArray(0);
}
