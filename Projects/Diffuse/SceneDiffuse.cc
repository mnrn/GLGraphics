/**
 * @brief Diffuse Reflection Model Test
 */

// ********************************************************************************
// Include files
// ********************************************************************************

#include "SceneDiffuse.h"

#include <boost/assert.hpp>
#include <iostream>

#include <glm/gtc/matrix_transform.hpp>

void SceneDiffuse::OnInit() {
  if (const auto msg = CompileAndLinkShader()) {
    std::cerr << msg.value() << std::endl;
    BOOST_ASSERT_MSG(false, "failed to compile or link!");
  }

  glEnable(GL_DEPTH_TEST);

  model_ = glm::mat4(1.0f);
  model_ =
      glm::rotate(model_, glm::radians(-35.0f), glm::vec3(1.0f, 0.0f, 0.0f));
  model_ =
      glm::rotate(model_, glm::radians(35.0f), glm::vec3(0.0f, 1.0f, 0.0f));
  view_ = glm::lookAt(glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f),
                      glm::vec3(0.0f, 1.0f, 0.0f));
  proj_ = glm::mat4(1.0f);

  prog_.SetUniform("Kd", 0.9f, 0.5f, 0.3f);
  prog_.SetUniform("Ld", 1.0f, 1.0f, 1.0f);
  prog_.SetUniform("LightPosition", view_ * glm::vec4(5.0f, 5.0f, 2.0f, 1.0f));
}

void SceneDiffuse::OnUpdate(float) {}

void SceneDiffuse::OnRender() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  const glm::mat4 mv = view_ * model_;
  prog_.SetUniform("ModelViewMatrix", mv);
  prog_.SetUniform("NormalMatrix", glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]),
                                             glm::vec3(mv[2])));
  prog_.SetUniform("MVP", proj_ * mv);
  torus_.Render();
}

void SceneDiffuse::OnResize(int w, int h) {
  SetDimensions(w, h);
  glViewport(0, 0, w, h);
  proj_ = glm::perspective(glm::radians(70.0f), static_cast<float>(w) / h, 0.3f,
                           100.0f);
}

std::optional<std::string> SceneDiffuse::CompileAndLinkShader() {
  if (prog_.Compile("./Assets/Shaders/Diffuse/Diffuse.vs.glsl",
                    ShaderType::Vertex) &&
      prog_.Compile("./Assets/Shaders/Diffuse/Diffuse.fs.glsl",
                    ShaderType::Fragment) &&
      prog_.Link()) {
    prog_.Use();
    return std::nullopt;
  } else {
    return prog_.GetLog();
  }
}
