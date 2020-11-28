/**
 * @brief SceneBezier Class
 */

// ********************************************************************************
// Include files
// ********************************************************************************

#include <boost/assert.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include "SceneBezier.h"

// ********************************************************************************
// Special member functions
// ********************************************************************************

SceneBezier::SceneBezier()
    : proj_(
          glm::ortho(-0.4f * c, 0.4f * c, -0.3f * c, 0.3f * c, 0.1f, 100.0f)) {
  if (const auto msg = CompileAndLinkShader()) {
    std::cerr << msg.value() << std::endl;
    BOOST_ASSERT_MSG(false, "failed to compile or link!");
  }

  glEnable(GL_DEPTH_TEST);
  glPointSize(10.0f);

  CreateVAO();
  // Set the number of vertices per patch.  IMPORTANT!!
  glPatchParameteri(GL_PATCH_VERTICES, 4);
  SetUniforms();
}

// ********************************************************************************
// Override functions
// ********************************************************************************

void SceneBezier::OnUpdate(float d) { static_cast<void>(d); }

void SceneBezier::OnRender() const {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  const glm::vec3 eyePt(0.0f, 0.0f, 1.5f);
  const auto view = glm::lookAt(eyePt, glm::vec3(0.0f, 0.0f, 0.0f),
                                glm::vec3(0.0f, 1.0f, 0.0f));
  const auto model = glm::mat4(1.0f);

  glBindVertexArray(vao_);
  SetMatrices(model, view, proj_);

  // Draw curve
  bezier_.Use();
  glDrawArrays(GL_PATCHES, 0, 4);

  // Draw the control points
  solid_.Use();
  glDrawArrays(GL_POINTS, 0, 4);

  glFinish();
}

// ********************************************************************************
// functions
// ********************************************************************************

std::optional<std::string> SceneBezier::CompileAndLinkShader() {
  // compile and links
  if (!bezier_.Compile("./Res/Shaders/Bezier/bezier.vs.glsl",
                       ShaderType::Vertex) ||
      !bezier_.Compile("./Res/Shaders/Bezier/bezier.tcs.glsl",
                       ShaderType::TessControl) ||
      !bezier_.Compile("./Res/Shaders/Bezier/bezier.tes.glsl",
                       ShaderType::TessEvaluation) ||
      !bezier_.Compile("./Res/Shaders/Bezier/bezier.fs.glsl",
                       ShaderType::Fragment) ||
      !bezier_.Link()) {
    return bezier_.Log();
  }
  bezier_.Use();

  if (!solid_.Compile("./Res/Shaders/Bezier/solid.vs.glsl",
                      ShaderType::Vertex) ||
      !solid_.Compile("./Res/Shaders/Bezier/solid.fs.glsl",
                      ShaderType::Fragment) ||
      !solid_.Link()) {
    return solid_.Log();
  }
  return std::nullopt;
}

void SceneBezier::CreateVAO() {
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

void SceneBezier::SetUniforms() {
  // Segments and strips may be inverted on NVIDIA
  bezier_.Use();
  bezier_.SetUniform("segments", 50);
  bezier_.SetUniform("strips", 1);
  bezier_.SetUniform("color", glm::vec4(1.0f, 1.0f, 0.5f, 1.0f));

  solid_.Use();
  solid_.SetUniform("color", glm::vec4(0.5f, 1.0f, 1.0f, 1.0f));
}

void SceneBezier::SetMatrices(const glm::mat4 &model, const glm::mat4 &view,
                              const glm::mat4 &proj) const {
  const auto mv = view * model;
  const auto mvp = proj * mv;

  bezier_.Use();
  bezier_.SetUniform("MVP", mvp);

  solid_.Use();
  solid_.SetUniform("MVP", mvp);
}
