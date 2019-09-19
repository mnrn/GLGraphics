/**
 * @brief Scene HelloTriangle Class
 */

// ********************************************************************************
// Include files
// ********************************************************************************

#include <boost/assert.hpp>
#include <iostream>

#include "SceneHelloTriangle.hpp"

// ********************************************************************************
// Namespace
// ********************************************************************************

namespace Scene {

// ********************************************************************************
// Special member functions
// ********************************************************************************

HelloTriangle::HelloTriangle() {
  if (compileAndLinkShader() == false) {
    BOOST_ASSERT_MSG(false, "failed to compile or link!");
  }

  createVBO();
}

// ********************************************************************************
// Override functions
// ********************************************************************************

void HelloTriangle::update(float d) { static_cast<void>(d); }

void HelloTriangle::render() const {
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  glBindVertexArray(hVBO_);
  glDrawArrays(GL_TRIANGLES, 0, 3);
}

// ********************************************************************************
// Shader prepare
// ********************************************************************************

bool HelloTriangle::compileAndLinkShader() {
  // compile and links
  if (program_.Compile("./data/shaders/basic/basic.vert",
                        Shader::Type::Vertex) == false) {
    std::cerr << "vertex shader failed to compile" << std::endl;
    std::cerr << program_.GetLog() << std::endl;
    return false;
  }
  if (program_.Compile("./data/shaders/basic/basic.frag",
                        Shader::Type::Fragment) == false) {
    std::cerr << "fragment shader failed to compile" << std::endl;
    std::cerr << program_.GetLog() << std::endl;
    return false;
  }
  if (program_.Link() == false) {
    std::cerr << "shader program failed to link" << std::endl;
    std::cerr << program_.GetLog() << std::endl;
    return false;
  }

  program_.Use();
  return true;
}

void HelloTriangle::createVBO() {
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

  glGenVertexArrays(1, &hVBO_);
  glBindVertexArray(hVBO_);

  glEnableVertexAttribArray(0); // vertex position
  glEnableVertexAttribArray(1); // vertex color

  glBindBuffer(GL_ARRAY_BUFFER, vboPosition);
  glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, nullptr );

  glBindBuffer(GL_ARRAY_BUFFER, vboColor);
  glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, nullptr );

  glBindVertexArray(0);
}

} // namespace Scene
