/**
 * @brief Scene Bezier
 */

#ifndef SCENE_BEZIER_H
#define SCENE_BEZIER_H

// ********************************************************************************
// Include files
// ********************************************************************************

#include <optional>
#include <string>

#include <glm/glm.hpp>

#include "Scene.h"
#include "Graphics/Shader.h"

// ********************************************************************************
// Class
// ********************************************************************************

/**
 * @brief Scene Hello Triangle Class
 */
class SceneBezier : public Scene {
public:
  void OnInit() override;
  void OnDestroy() override;
  void OnUpdate(float) override;
  void OnRender() override;
  void OnResize(int, int) override;

private:
  std::optional<std::string> CompileAndLinkShader();
  void CreateVAO();
  void SetUniforms();
  void SetMatrices();

  ShaderProgram bezier_;
  ShaderProgram solid_;

  GLuint vbo_ = 0;
  GLuint vao_ = 0;
};

#endif // SCENE_BEZIER_H
