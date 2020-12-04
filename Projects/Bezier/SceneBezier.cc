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
// Override functions
// ********************************************************************************

void SceneBezier::OnInit() {
  proj_ = glm::ortho(-0.4f * c, 0.4f * c, -0.3f * c, 0.3f * c, 0.1f, 100.0f);
  if (const auto msg = CompileAndLinkShader()) {
    std::cerr << msg.value() << std::endl;
    BOOST_ASSERT_MSG(false, "failed to compile or link!");
  }

  glEnable(GL_DEPTH_TEST);
  glPointSize(10.0f);

  CreateVAO();
  // パッチごとの頂点数(制御点)を設定しておきます。  IMPORTANT!!
  glPatchParameteri(GL_PATCH_VERTICES, 4);
  SetUniforms();
}

void SceneBezier::OnDestroy() {
  glDeleteVertexArrays(1, &vao_);
  glDeleteBuffers(1, &vbo_);
}

void SceneBezier::OnUpdate(float d) { static_cast<void>(d); }

void SceneBezier::OnRender() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  const glm::vec3 eyePt(0.0f, 0.0f, 1.5f);
  const auto view = glm::lookAt(eyePt, glm::vec3(0.0f, 0.0f, 0.0f),
                                glm::vec3(0.0f, 1.0f, 0.0f));
  const auto model = glm::mat4(1.0f);

  glBindVertexArray(vao_);
  SetMatrices(model, view, proj_);

  // ベジェ曲線の描画
  bezier_.Use();
  glDrawArrays(GL_PATCHES, 0, 4);

  // 制御点の描画
  solid_.Use();
  glDrawArrays(GL_POINTS, 0, 4);

  glFinish();
}

void SceneBezier::OnResize(int w, int h) {
  SetDimensions(w, h);
  glViewport(0, 0, w, h);
}

// ********************************************************************************
// functions
// ********************************************************************************

std::optional<std::string> SceneBezier::CompileAndLinkShader() {
  // compile and links
  if (!(bezier_.Compile("./Assets/Shaders/Bezier/bezier.vs.glsl",
                        ShaderType::Vertex) &&
        bezier_.Compile("./Assets/Shaders/Bezier/bezier.tcs.glsl",
                        ShaderType::TessControl) &&
        bezier_.Compile("./Assets/Shaders/Bezier/bezier.tes.glsl",
                        ShaderType::TessEvaluation) &&
        bezier_.Compile("./Assets/Shaders/Bezier/bezier.fs.glsl",
                        ShaderType::Fragment) &&
        bezier_.Link())) {
    return bezier_.GetLog();
  }
  bezier_.Use();

  if (!(solid_.Compile("./Assets/Shaders/Bezier/solid.vs.glsl",
                       ShaderType::Vertex) &&
        solid_.Compile("./Assets/Shaders/Bezier/solid.fs.glsl",
                       ShaderType::Fragment) &&
        solid_.Link())) {
    return solid_.GetLog();
  }
  return std::nullopt;
}

void SceneBezier::CreateVAO() {
  // 制御点用のVBOを設定します。
  float v[] = {-1.0f, -1.0f, -0.5f, 1.0f, 0.5f, -1.0f, 1.0f, 1.0f};

  glGenBuffers(1, &vbo_);

  glBindBuffer(GL_ARRAY_BUFFER, vbo_);
  glBufferData(GL_ARRAY_BUFFER, sizeof(v), v, GL_STATIC_DRAW);

  // 制御点用のVAOを設定します。
  glGenVertexArrays(1, &vao_);
  glBindVertexArray(vao_);

  glBindBuffer(GL_ARRAY_BUFFER, vbo_);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(0);

  glBindVertexArray(0);
}

void SceneBezier::SetUniforms() {
  // NVIDIA環境ではセグメントとストリップが反転する場合もあるようです。
  bezier_.Use();
  bezier_.SetUniform("Segments", 50);
  bezier_.SetUniform("Strips", 1);
  bezier_.SetUniform("Color", glm::vec4(1.0f, 1.0f, 0.5f, 1.0f));

  solid_.Use();
  solid_.SetUniform("Color", glm::vec4(0.5f, 1.0f, 1.0f, 1.0f));
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
