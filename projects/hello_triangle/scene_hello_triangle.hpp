/**
 * @brief Scene Triangle
 * @date 2017/
 */

#ifndef SCENE_HELLO_TRIANGLE_HPP
#define SCENE_HELLO_TRIANGLE_HPP

// ********************************************************************************
// Include files
// ********************************************************************************

#include <optional>
#include <string>

#include "scene_base.hpp"
#include "shader_program.hpp"

// ********************************************************************************
// Class
// ********************************************************************************

/**
 * @brief Scene Hello Triangle Class
 */
class SceneHelloTriangle : public SceneBase {
public:
  SceneHelloTriangle();
  ~SceneHelloTriangle() override = default;

  void update(float d) override;
  void render() const override;

private:
  std::optional<std::string> compileAndLinkShader();
  void createVBO();

  GLuint vbo_ = 0;
  ShaderProgram prog_;
};

#endif // SCENE_HELLO_TRIANGLE_HPP
