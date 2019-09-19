/**
 * @brief Scene Triangle
 * @date 2017/
 */

#ifndef SCENE_HELLO_TRIANGLE_HPP
#define SCENE_HELLO_TRIANGLE_HPP

// ********************************************************************************
// Include files
// ********************************************************************************

#include "scene_base.hpp"
#include "shader_program.hpp"

// ********************************************************************************
// Namespace
// ********************************************************************************

namespace scene {

// ********************************************************************************
// Class
// ********************************************************************************

/**
 * @brief Scene Hello Triangle Class
 */
class hello_triangle : public base {
public:
  hello_triangle();
  ~hello_triangle() override = default;

  void update(float d) override;
  void render() const override;

private:
  bool compile_and_link_shader();
  void create_vbo();

  GLuint hvbo_ = 0;
  shader::program prog_;
};

} // namespace Scene

#endif // SCENE_HELLO_TRIANGLE_HPP
