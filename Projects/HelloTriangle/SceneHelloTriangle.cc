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
// Special member functions
// ********************************************************************************

SceneHelloTriangle::SceneHelloTriangle() {
  if (const auto msg = CompileAndLinkShader()) {
    std::cerr << msg.value() << std::endl;
    BOOST_ASSERT_MSG(false, "failed to compile or link!");
  }

  CreateVBO();
}

// ********************************************************************************
// Override functions
// ********************************************************************************

void SceneHelloTriangle::OnUpdate(float d) { static_cast<void>(d); }

void SceneHelloTriangle::OnRender() {
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  glBindVertexArray(vbo_);
  glDrawArrays(GL_TRIANGLES, 0, 3);
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
  if (!prog_.Compile("./Assets/Shaders/Basic/basic.vs.glsl",
                     ShaderType::Vertex) ||
      !prog_.Compile("./Assets/Shaders/Basic/basic.fs.glsl",
                     ShaderType::Fragment) ||
      !prog_.Link()) {
    return prog_.Log();
  }

  prog_.Use();
  return std::nullopt;
}

void SceneHelloTriangle::CreateVBO() {
  float position[] = {
      -0.8f, -0.8f, 0.0f, 0.8f, -0.8f, 0.0f, 0.0f, 0.8f, 0.0f,
  };

  float color[] = {
      1.0f, 0.0f, 0.0, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
  };

  GLuint hVBO[2]{};
  glGenBuffers(2, hVBO);

  GLuint vboPosition = hVBO[0];
  GLuint vboColor = hVBO[1];

  glBindBuffer(GL_ARRAY_BUFFER, vboPosition);
  glBufferData(GL_ARRAY_BUFFER, sizeof(position), position, GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, vboColor);
  glBufferData(GL_ARRAY_BUFFER, sizeof(color), color, GL_STATIC_DRAW);

  glGenVertexArrays(1, &vbo_);
  glBindVertexArray(vbo_);

  glEnableVertexAttribArray(0); // vertex position
  glEnableVertexAttribArray(1); // vertex color

  glBindBuffer(GL_ARRAY_BUFFER, vboPosition);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

  glBindBuffer(GL_ARRAY_BUFFER, vboColor);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

  glBindVertexArray(0);
}
