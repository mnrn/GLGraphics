/**
 * @brief Scene HelloTriangle Class
 */

// ********************************************************************************
// Include files
// ********************************************************************************

#include <boost/assert.hpp>
#include <iostream>

#include "scene_hello_triangle.h"

// ********************************************************************************
// Special member functions
// ********************************************************************************

SceneHelloTriangle::SceneHelloTriangle() {
  if (const auto msg = compileAndLinkShader()) {
    std::cerr << msg.value() << std::endl;
    BOOST_ASSERT_MSG(false, "failed to compile or link!");
  }

  createVBO();
}

// ********************************************************************************
// Override functions
// ********************************************************************************

void SceneHelloTriangle::update(float d) { static_cast<void>(d); }

void SceneHelloTriangle::render() const {
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  glBindVertexArray(vbo_);
  glDrawArrays(GL_TRIANGLES, 0, 3);
}

// ********************************************************************************
// Shader prepare
// ********************************************************************************

std::optional<std::string> SceneHelloTriangle::compileAndLinkShader() {
  // compile and links
  if (!prog_.compile("./res/shaders/basic/basic.vs.glsl", ShaderType::Vertex) ||
      !prog_.compile("./res/shaders/basic/basic.fs.glsl",
                     ShaderType::Fragment) ||
      !prog_.link()) {
    return prog_.log();
  }

  prog_.use();
  return std::nullopt;
}

void SceneHelloTriangle::createVBO() {
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
