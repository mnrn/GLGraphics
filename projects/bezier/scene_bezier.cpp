/**
 * @brief SceneBezier Class
 */

// ********************************************************************************
// Include files
// ********************************************************************************
#include "scene_bezier.hpp"

#include <boost/assert.hpp>
#include <iostream>

#include <glm/gtc/matrix_transform.hpp>

// ********************************************************************************
// Special member functions
// ********************************************************************************

SceneBezier::SceneBezier()
    : proj_(
          glm::ortho(-0.4f * c, 0.4f * c, -0.3f * c, 0.3f * c, 0.1f, 100.0f)) {
  if (const auto msg = compileAndLinkShader()) {
    std::cerr << msg.value() << std::endl;
    BOOST_ASSERT_MSG(false, "failed to compile or link!");
  }

  glEnable(GL_DEPTH_TEST);
  glPointSize(10.0f);

  createVAO();
  // Set the number of vertices per patch.  IMPORTANT!!
  glPatchParameteri(GL_PATCH_VERTICES, 4);
  setUniforms();
}

// ********************************************************************************
// Override functions
// ********************************************************************************

void SceneBezier::update(float d) { static_cast<void>(d); }

void SceneBezier::render() const {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  const glm::vec3 eyePt(0.0f, 0.0f, 1.5f);
  const auto view = glm::lookAt(eyePt, glm::vec3(0.0f, 0.0f, 0.0f),
                                glm::vec3(0.0f, 1.0f, 0.0f));
  const auto model = glm::mat4(1.0f);

  glBindVertexArray(vao_);
  setMatrices(model, view, proj_);

  // Draw curve
  bezier_.use();
  glDrawArrays(GL_PATCHES, 0, 4);

  // Draw the control points
  solid_.use();
  glDrawArrays(GL_POINTS, 0, 4);

  glFinish();
}

// ********************************************************************************
// functions
// ********************************************************************************

std::optional<std::string> SceneBezier::compileAndLinkShader() {
  // compile and links
  if (!bezier_.compile("./res/shaders/bezier/bezier.vs.glsl",
                       ShaderType::Vertex) ||
      !bezier_.compile("./res/shaders/bezier/bezier.tcs.glsl",
                       ShaderType::TessControl) ||
      !bezier_.compile("./res/shaders/bezier/bezier.tes.glsl",
                       ShaderType::TessEvaluation) ||
      !bezier_.compile("./res/shaders/bezier/bezier.fs.glsl",
                       ShaderType::Fragment) ||
      !bezier_.link()) {
    return bezier_.log();
  }
  bezier_.use();

  if (!solid_.compile("./res/shaders/bezier/solid.vs.glsl",
                      ShaderType::Vertex) ||
      !solid_.compile("./res/shaders/bezier/solid.fs.glsl",
                      ShaderType::Fragment) ||
      !solid_.link()) {
    return solid_.log();
  }
  return std::nullopt;
}

void SceneBezier::createVAO() {
  // Set up patch VBO
  float v[] = {-1.0f, -1.0f, -0.5f, 1.0f, 0.5f, -1.0f, 1.0f, 1.0f};

  GLuint vbo;
  glGenBuffers(1, &vbo);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(v), v, GL_STATIC_DRAW);

  // Set up patch VAO
  glGenVertexArrays(1, &vao_);
  glBindVertexArray(vao_);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(0);

  glBindVertexArray(0);
}

void SceneBezier::setUniforms() {
  // Segments and strips may be inverted on NVIDIA
  bezier_.use();
  bezier_.setUniform("segments", 50);
  bezier_.setUniform("strips", 1);
  bezier_.setUniform("color", glm::vec4(1.0f, 1.0f, 0.5f, 1.0f));

  solid_.use();
  solid_.setUniform("color", glm::vec4(0.5f, 1.0f, 1.0f, 1.0f));
}

void SceneBezier::setMatrices(const glm::mat4 &model, const glm::mat4 &view,
                              const glm::mat4 &proj) const {
  const auto mv = view * model;
  const auto mvp = proj * mv;

  bezier_.use();
  bezier_.setUniform("MVP", mvp);

  solid_.use();
  solid_.setUniform("MVP", mvp);
}
