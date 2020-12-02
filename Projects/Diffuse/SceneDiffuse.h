/**
 * @brief Diffuse Reflection Model Test
 */

#ifndef SCENE_DIFFUSE_H
#define SCENE_DIFFUSE_H

// ********************************************************************************
// Include files
// ********************************************************************************

#include <optional>
#include <string>

#include "Primitive/Torus.h"
#include "Scene.h"
#include "Shader.hh"

// ********************************************************************************
// Class
// ********************************************************************************

/**
 * @brief Scene Diffuse Class
 */
class SceneDiffuse : public Scene {
public:
  SceneDiffuse() = default;
  ~SceneDiffuse() override = default;

  void OnInit() override;
  void OnUpdate(float) override;
  void OnRender() override;
  void OnResize(int, int) override;

private:
  std::optional<std::string> CompileAndLinkShader();

  Torus torus_{0.8f, 0.4f, 50, 50};
  ShaderProgram prog_;
};

#endif // SCENE_HELLO_TRIANGLE_H
