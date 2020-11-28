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
  SceneBezier();
  ~SceneBezier() override = default;

  void OnUpdate(float d) override;
  void OnRender() const override;

private:
  std::optional<std::string> CompileAndLinkShader();
  void CreateVAO();
  void SetUniforms();
  void SetMatrices(const glm::mat4 &, const glm::mat4 &,
                   const glm::mat4 &) const;

  static constexpr float c = 3.5f;

  GLuint vao_ = 0;

  ShaderProgram bezier_;
  ShaderProgram solid_;

  const glm::mat4 proj_;
};

#endif // SCENE_BEZIER_H
