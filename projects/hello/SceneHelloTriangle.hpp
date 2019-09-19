/**
 * @brief Scene Triangle
 * @date 2017/
 */

#ifndef SCENE_HELLO_TRIANGLE_HPP
#define SCENE_HELLO_TRIANGLE_HPP

// ********************************************************************************
// Include files
// ********************************************************************************

#include "SceneBase.hpp"
#include "ShaderProgram.hpp"

// ********************************************************************************
// Namespace
// ********************************************************************************

namespace Scene {

// ********************************************************************************
// Class
// ********************************************************************************

/**
 * @brief Scene Hello Triangle Class
 */
class HelloTriangle : public Base {
public:
  HelloTriangle();
  ~HelloTriangle() override = default;

  void update(float d) override;
  void render() const override;

private:
  bool compileAndLinkShader();
  void createVBO();

  GLuint hVBO_ = 0;
  Shader::Program program_;
};

} // namespace Scene

#endif // SCENE_HELLO_TRIANGLE_HPP
