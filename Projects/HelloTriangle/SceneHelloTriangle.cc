/**
 * @brief Scene HelloTriangle Class
 */

// ********************************************************************************
// Include files
// ********************************************************************************

#include <boost/assert.hpp>
#include <iostream>

#include "SceneHelloTriangle.h"

// ********************************************************************************
// Override functions
// ********************************************************************************

void SceneHelloTriangle::OnInit() {
  if (const auto msg = CompileAndLinkShader()) {
    std::cerr << msg.value() << std::endl;
    BOOST_ASSERT_MSG(false, "failed to compile or link!");
  }

  CreateVAO();
}

void SceneHelloTriangle::OnDestroy() {
  glDeleteVertexArrays(1, &vao_);
  glDeleteBuffers(static_cast<GLsizei>(vbo_.size()), vbo_.data());
}

void SceneHelloTriangle::OnUpdate(float d) { static_cast<void>(d); }

void SceneHelloTriangle::OnRender() {
  prog_.Use();

  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  glBindVertexArray(vao_);
  glDrawArrays(GL_TRIANGLES, 0, 3);
  glBindVertexArray(0);
}

void SceneHelloTriangle::OnResize(int w, int h) {
  SetDimensions(w, h);
  glViewport(0, 0, w, h);
}

// ********************************************************************************
// Shader prepare
// ********************************************************************************

std::optional<std::string> SceneHelloTriangle::CompileAndLinkShader() {
  // compile and links
  if (prog_.Compile("./Assets/Shaders/Basic/Basic.vs.glsl",
                     ShaderType::Vertex) &&
      prog_.Compile("./Assets/Shaders/Basic/Basic.fs.glsl",
                     ShaderType::Fragment) &&
      prog_.Link()) {
    return std::nullopt;
  } else {
    return prog_.GetLog();
  }
}

void SceneHelloTriangle::CreateVAO() {
  float position[] = {
      -0.8f, -0.8f, 0.0f, 0.8f, -0.8f, 0.0f, 0.0f, 0.8f, 0.0f,
  };

  float color[] = {
      1.0f, 0.0f, 0.0, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
  };

  glGenBuffers(vbo_.size(), vbo_.data());

  glBindBuffer(GL_ARRAY_BUFFER, vbo_[Position]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(position), position, GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, vbo_[Color]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(color), color, GL_STATIC_DRAW);

  glGenVertexArrays(1, &vao_);
  glBindVertexArray(vao_);

  glEnableVertexAttribArray(0); // Vertex position
  glEnableVertexAttribArray(1); // Vertex color

  glBindBuffer(GL_ARRAY_BUFFER, vbo_[Position]);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

  glBindBuffer(GL_ARRAY_BUFFER, vbo_[Color]);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}
