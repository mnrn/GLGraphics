/**
 * @brief Scene Texture Class
 */

// ********************************************************************************
// Include files
// ********************************************************************************

#include <boost/assert.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include "SceneTexture.h"
#include "Texture.h"

// ********************************************************************************
// Override functions
// ********************************************************************************

void SceneTexture::OnInit() {
  if (const auto msg = CompileAndLinkShader()) {
    std::cerr << msg.value() << std::endl;
    BOOST_ASSERT_MSG(false, "failed to compile or link!");
  }

  glEnable(GL_DEPTH_TEST);
  view_ = glm::lookAt(glm::vec3(1.0f, 1.25f, 1.25f),
                      glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
  proj_ = glm::mat4(1.0f);

  angle_ = 0.0f;

  prog_.SetUniform("Light.L", glm::vec3(1.0f));
  prog_.SetUniform("Light.La", glm::vec3(0.15f));

  // テクスチャのロード
  tex_ = Texture::Load("./Assets/Textures/Wood/hp_wood.png");
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, tex_);

#ifdef __APPLE__
  prog_.SetUniform("Tex1", 0);
#endif
}

void SceneTexture::OnDestroy() { glDeleteTextures(1, &tex_); }

void SceneTexture::OnUpdate(float) {}

void SceneTexture::OnRender() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  prog_.SetUniform("Light.Position", glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
  prog_.SetUniform("Material.Ks", 0.05f, 0.05f, 0.05f);
  prog_.SetUniform("Material.Shininess", 1.0f);

  model_ = glm::mat4(1.0f);
  const glm::mat4 mv = view_ * model_;
  prog_.SetUniform("ModelViewMatrix", mv);
  prog_.SetUniform("NormalMatrix", glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]),
                                             glm::vec3(mv[2])));
  prog_.SetUniform("MVP", proj_ * mv);
  cube_.Render();
}

void SceneTexture::OnResize(int w, int h) {
  SetDimensions(w, h);
  glViewport(0, 0, w, h);
  proj_ = glm::perspective(glm::radians(60.0f),
                           static_cast<float>(w) / static_cast<float>(h), 0.3f,
                           100.0f);
}

// ********************************************************************************
// Shader prepare
// ********************************************************************************

std::optional<std::string> SceneTexture::CompileAndLinkShader() {
  if (prog_.Compile("./Assets/Shaders/Texture/Texture.vs.glsl",
                    ShaderType::Vertex) &&
      prog_.Compile("./Assets/Shaders/Texture/Texture.fs.glsl",
                    ShaderType::Fragment) &&
      prog_.Link()) {
    prog_.Use();
    return std::nullopt;
  } else {
    return prog_.GetLog();
  }
}
