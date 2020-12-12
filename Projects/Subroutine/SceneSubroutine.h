/**
 * @brief Subroutine Test
 */

#ifndef SCENE_SUBROUTINE_H
#define SCENE_SUBROUTINE_H

// ********************************************************************************
// Include files
// ********************************************************************************

#include <optional>
#include <string>

#include "Primitive/Teapot.h"
#include "Scene.h"
#include "Shader.h"

// ********************************************************************************
// Class
// ********************************************************************************

/**
 * @brief Scene Subroutine Class
 */
class SceneSubroutine : public Scene {
public:
  void OnInit() override;
  void OnUpdate(float) override;
  void OnRender() override;
  void OnResize(int, int) override;

private:
  std::optional<std::string> CompileAndLinkShader();
  void SetMatrices();

  Teapot teapot_{13, glm::mat4(1.0f)};
  ShaderProgram prog_;
};

#endif // SCENE_SUBROUTINE_H
