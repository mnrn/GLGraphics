/**
 * @brief Subroutine Test
 * @note SubroutineはSPIR-Vでは使用できないのでご注意ください。
 */

// ********************************************************************************
// Including files
// ********************************************************************************

#include "SceneSubroutine.h"

#include <boost/assert.hpp>
#include <iostream>

#include <glm/gtc/matrix_transform.hpp>

// ********************************************************************************
// Override functions
// ********************************************************************************

void SceneSubroutine::OnInit() {
  if (const auto msg = CompileAndLinkShader()) {
    std::cerr << msg.value() << std::endl;
    BOOST_ASSERT_MSG(false, "failed to compile or link!");
  }

  glEnable(GL_DEPTH_TEST);

  view_ = glm::lookAt(glm::vec3(0.0f, 0.0f, 10.0f), glm::vec3(0.0f, 0.0f, 0.0f),
                      glm::vec3(0.0f, 1.0f, 0.0f));
  proj_ = glm::mat4(1.0f);

  prog_.SetUniform("Material.Kd", 0.9f, 0.5f, 0.3f);
  prog_.SetUniform("Light.Ld", 1.0f, 1.0f, 1.0f);

  prog_.SetUniform("Material.Ka", 0.9f, 0.5f, 0.3f);
  prog_.SetUniform("Light.La", 0.4f, 0.4f, 0.4f);

  prog_.SetUniform("Material.Ks", 0.8f, 0.8f, 0.8f);
  prog_.SetUniform("Light.Ls", 1.0f, 1.0f, 1.0f);

  prog_.SetUniform("Material.Shininess", 100.0f);
}

void SceneSubroutine::OnUpdate(float) {}

void SceneSubroutine::OnRender() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  const glm::vec4 lightPos = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
  prog_.SetUniform("LightPosition", lightPos);

  const GLuint hProg = prog_.GetHandle();
  const GLuint adsIndex =
      glGetSubroutineIndex(hProg, GL_VERTEX_SHADER, "PhongModel");
  const GLuint diffIndex =
      glGetSubroutineIndex(hProg, GL_VERTEX_SHADER, "DiffuseModel");

  // 左のティーポットを描画
  glUniformSubroutinesuiv(GL_VERTEX_SHADER, 1, &adsIndex);
  model_ = glm::mat4(1.0f);
  model_ = glm::translate(model_, glm::vec3(-3.0f, -1.5f, 0.0f));
  model_ =
      glm::rotate(model_, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
  SetMatrices();
  teapot_.Render();

  // 右のティーポットを描画
  glUniformSubroutinesuiv(GL_VERTEX_SHADER, 1, &diffIndex);
  model_ = glm::mat4(1.0f);
  model_ = glm::translate(model_, glm::vec3(3.0f, -1.5f, 0.0f));
  model_ =
      glm::rotate(model_, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
  SetMatrices();
  teapot_.Render();
}

void SceneSubroutine::OnResize(int w, int h) {
  SetDimensions(w, h);
  glViewport(0, 0, w, h);
  proj_ = glm::perspective(glm::radians(50.0f),
                           static_cast<float>(w) / static_cast<float>(h), 0.3f,
                           100.0f);
}

// ********************************************************************************
// Shader prepare
// ********************************************************************************

std::optional<std::string> SceneSubroutine::CompileAndLinkShader() {
  if (prog_.Compile("./Assets/Shaders/Subroutine/Subroutine.vs.glsl",
                    ShaderType::Vertex) &&
      prog_.Compile("./Assets/Shaders/Subroutine/Subroutine.fs.glsl",
                    ShaderType::Fragment) &&
      prog_.Link()) {
    prog_.Use();
    return std::nullopt;
  } else {
    return prog_.GetLog();
  }
}

void SceneSubroutine::SetMatrices() {
  const glm::mat4 mv = view_ * model_;
  prog_.SetUniform("ModelViewMatrix", mv);
  prog_.SetUniform("NormalMatrix", glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]),
                                             glm::vec3(mv[2])));
  prog_.SetUniform("MVP", proj_ * mv);
}
