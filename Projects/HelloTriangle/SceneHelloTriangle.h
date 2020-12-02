/**
 * @brief Scene Triangle
 * @date 2017/
 */

#ifndef SCENE_HELLO_TRIANGLE_H
#define SCENE_HELLO_TRIANGLE_H

// ********************************************************************************
// Include files
// ********************************************************************************

#include <optional>
#include <string>

#include "Scene.h"
#include "Shader.hh"

// ********************************************************************************
// Class
// ********************************************************************************

/**
 * @brief Scene Hello Triangle Class
 */
class SceneHelloTriangle : public Scene {
public:
  SceneHelloTriangle() = default;
  ~SceneHelloTriangle() override = default;

  void OnInit() override;
  void OnUpdate(float) override;
  void OnRender() override;
  void OnResize(int, int) override;

private:
  std::optional<std::string> CompileAndLinkShader();
  void CreateVBO();

  GLuint vbo_ = 0;
  ShaderProgram prog_;
};

#endif // SCENE_HELLO_TRIANGLE_H
