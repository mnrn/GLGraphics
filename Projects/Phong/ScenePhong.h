/**
 * @brief Diffuse Reflection Model Test
 */

#ifndef SCENE_PHONG_H
#define SCENE_PHONG_H

// ********************************************************************************
// Include files
// ********************************************************************************

#include <optional>
#include <string>

#include "Primitive/Torus.h"
#include "Scene.h"
#include "Graphics/Shader.h"

// ********************************************************************************
// Class
// ********************************************************************************

/**
 * @brief Scene Phong Class
 */
class ScenePhong : public Scene {
public:
  void OnInit() override;
  void OnUpdate(float) override;
  void OnRender() override;
  void OnResize(int, int) override;

private:
  std::optional<std::string> CompileAndLinkShader();

  Torus torus_{0.7f, 0.3f, 50, 50};
  float angle_ = 0.0f;
  ShaderProgram prog_;
};

#endif // SCENE_HELLO_TRIANGLE_H
