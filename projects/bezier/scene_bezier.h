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

#include "scene_base.hpp"
#include "shader_program.hpp"

// ********************************************************************************
// Class
// ********************************************************************************

/**
 * @brief Scene Hello Triangle Class
 */
class SceneBezier : public SceneBase {
public:
  SceneBezier();
  ~SceneBezier() override = default;

  void update(float d) override;
  void render() const override;

private:
  std::optional<std::string> compileAndLinkShader();
  void createVAO();
  void setUniforms();
  void setMatrices(const glm::mat4 &, const glm::mat4 &,
                   const glm::mat4 &) const;

  static constexpr float c = 3.5f;

  GLuint vao_ = 0;

  ShaderProgram bezier_;
  ShaderProgram solid_;

  const glm::mat4 proj_;
};

#endif // SCENE_BEZIER_H
