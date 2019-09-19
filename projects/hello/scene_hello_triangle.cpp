/**
 * @brief Scene HelloTriangle Class
 */

// ********************************************************************************
// Include files
// ********************************************************************************

#include <boost/assert.hpp>
#include <iostream>

#include "scene_hello_triangle.hpp"

// ********************************************************************************
// Namespace
// ********************************************************************************

namespace scene {

// ********************************************************************************
// Special member functions
// ********************************************************************************

hello_triangle::hello_triangle() {
  if (compile_and_link_shader() == false) {
    BOOST_ASSERT_MSG(false, "failed to compile or link!");
  }

  create_vbo();
}

// ********************************************************************************
// Override functions
// ********************************************************************************

void hello_triangle::update(float d) { static_cast<void>(d); }

void hello_triangle::render() const {
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  glBindVertexArray(hvbo_);
  glDrawArrays(GL_TRIANGLES, 0, 3);
}

// ********************************************************************************
// Shader prepare
// ********************************************************************************

bool hello_triangle::compile_and_link_shader() {
  // compile and links
  if (prog_.compile("./data/shaders/basic/basic.vert",
                        shader::type::vertex) == false) {
    std::cerr << "vertex shader failed to compile" << std::endl;
    std::cerr << prog_.get_log() << std::endl;
    return false;
  }
  if (prog_.compile("./data/shaders/basic/basic.frag",
                        shader::type::fragment) == false) {
    std::cerr << "fragment shader failed to compile" << std::endl;
    std::cerr << prog_.get_log() << std::endl;
    return false;
  }
  if (prog_.link() == false) {
    std::cerr << "shader program failed to link" << std::endl;
    std::cerr << prog_.get_log() << std::endl;
    return false;
  }

  prog_.use();
  return true;
}

void hello_triangle::create_vbo() {
  float position[] = {
      -0.8f, -0.8f, 0.0f, 0.8f, -0.8f, 0.0f, 0.0f, 0.8f, 0.0f,
  };

  float color[] = {
      1.0f, 0.0f, 0.0, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
  };

  GLuint hvbo[2]{};
  glGenBuffers(2, hvbo);

  GLuint vbo_position = hvbo[0];
  GLuint vbo_color = hvbo[1];

  glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
  glBufferData(GL_ARRAY_BUFFER, sizeof(position), position, GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
  glBufferData(GL_ARRAY_BUFFER, sizeof(color), color, GL_STATIC_DRAW);

  glGenVertexArrays(1, &hvbo_);
  glBindVertexArray(hvbo_);

  glEnableVertexAttribArray(0); // vertex position
  glEnableVertexAttribArray(1); // vertex color

  glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
  glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, nullptr );

  glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
  glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, nullptr );

  glBindVertexArray(0);
}

} // namespace Scene
