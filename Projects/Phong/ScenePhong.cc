/**
 * @brief Phong Reflection Model Test
 */

// ********************************************************************************
// Including files
// ********************************************************************************

#include "ScenePhong.h"

#include <boost/assert.hpp>
#include <iostream>

#include <glm/gtc/matrix_transform.hpp>

void ScenePhong::OnInit() {
  if (const auto msg = CompileAndLinkShader()) {
    std::cerr << msg.value() << std::endl;
    BOOST_ASSERT_MSG(false, "failed to compile or link!");
  }

  glEnable(GL_DEPTH_TEST);

  view_ = glm::lookAt(glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f),
                      glm::vec3(0.0f, 1.0f, 0.0f));
  proj_ = glm::mat4(1.0f);
  const glm::vec4 worldLight = glm::vec4(5.0f, 5.0f, 2.0f, 1.0f);

  prog_.SetUniform("Material.Kd", 0.9f, 0.5f, 0.3f);
  prog_.SetUniform("Light.Ld", 1.0f, 1.0f, 1.0f);

  prog_.SetUniform("Light.Position", view_ * worldLight);

  prog_.SetUniform("Material.Ka", 0.9f, 0.5f, 0.3f);
  prog_.SetUniform("Light.La", 0.4f, 0.4f, 0.4f);

  prog_.SetUniform("Material.Ks", 0.8f, 0.8f, 0.8f);
  prog_.SetUniform("Light.Ls", 1.0f, 1.0f, 1.0f);

  prog_.SetUniform("Material.Shininess", 100.0f);
}

void ScenePhong::OnUpdate(float) {}

void ScenePhong::OnRender() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  model_ = glm::mat4(1.0f);
  model_ =
      glm::rotate(model_, glm::radians(angle_), glm::vec3(0.0f, 1.0f, 0.0f));
  model_ =
      glm::rotate(model_, glm::radians(-35.0f), glm::vec3(1.0f, 0.0f, 0.0f));
  model_ =
      glm::rotate(model_, glm::radians(35.0f), glm::vec3(0.0f, 1.0f, 0.0f));

  const glm::mat4 mv = view_ * model_;
  prog_.SetUniform("ModelViewMatrix", mv);
  prog_.SetUniform("NormalMatrix", glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]),
                                             glm::vec3(mv[2])));
  prog_.SetUniform("MVP", proj_ * mv);
  torus_.Render();
}

void ScenePhong::OnResize(int w, int h) {
  SetDimensions(w, h);
  glViewport(0, 0, w, h);
  proj_ = glm::perspective(glm::radians(70.0f),
                           static_cast<float>(w) / static_cast<float>(h), 0.3f,
                           100.0f);
}

std::optional<std::string> ScenePhong::CompileAndLinkShader() {
  if (prog_.Compile("./Assets/Shaders/Phong/Phong.vs.glsl",
                    ShaderType::Vertex) &&
      prog_.Compile("./Assets/Shaders/Phong/Phong.fs.glsl",
                    ShaderType::Fragment) &&
      prog_.Link()) {
    prog_.Use();
    return std::nullopt;
  } else {
    return prog_.GetLog();
  }
}
