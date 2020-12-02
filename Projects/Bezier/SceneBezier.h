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
#include "Shader.hh"

// ********************************************************************************
// Class
// ********************************************************************************

/**
 * @brief Scene Hello Triangle Class
 */
class SceneBezier : public Scene {
public:
  SceneBezier() = default;
  ~SceneBezier() override = default;

  void OnInit() override;
  void OnUpdate(float) override;
  void OnRender() override;
  void OnResize(int, int) override;

private:
  std::optional<std::string> CompileAndLinkShader();
  void CreateVAO();
  void SetUniforms();
  void SetMatrices(const glm::mat4 &, const glm::mat4 &,
                   const glm::mat4 &) const;

  static constexpr float c = 3.5f;

  ShaderProgram bezier_;
  ShaderProgram solid_;
  GLuint vao_ = 0;
};

#endif // SCENE_BEZIER_H
