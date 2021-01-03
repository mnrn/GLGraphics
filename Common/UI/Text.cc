/**
 * @brief  テキスト
 * @date   2020/12/14
 */

// ********************************************************************************
// Including files
// ********************************************************************************

#include "UI/Text.h"

#include <boost/assert.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

Text::Text() {
  if (const auto msg = CompileAndLinkShader()) {
    std::cerr << msg.value() << std::endl;
    BOOST_ASSERT_MSG(false, "Could not complie or link shader");
  }

  glGenVertexArrays(1, &vao_);
  glBindVertexArray(vao_);

  glGenBuffers(1, &vbo_);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_);

  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, nullptr,
               GL_DYNAMIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

Text::~Text() {
  if (vbo_ != 0) {
    glDeleteBuffers(1, &vbo_);
  }
  if (vao_ != 0) {
    glDeleteVertexArrays(1, &vao_);
  }
}

std::optional<std::string> Text::CompileAndLinkShader() {
  return prog_.CompileAndLink(
      {{"./Assets/Shaders/UI/Text.vs.glsl", ShaderType::Vertex},
       {"./Assets/Shaders/UI/Text.fs.glsl", ShaderType::Fragment}});
}

void Text::Begin(const TextConfig &config) {
  config_ = config;
  prog_.Use();
  if (config.windowWidth && config.windowHeight) {
    const glm::mat4 proj = glm::ortho(0.0f, config.windowWidth.value(), 0.0f,
                                      config.windowHeight.value());
    prog_.SetUniform("ProjMatrix", proj);
  } else {
    BOOST_ASSERT_MSG(false, "Need to set Window Width And Window Height.");
  }

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glActiveTexture(GL_TEXTURE0);
  glBindVertexArray(vao_);
}

void Text::End() {
  config_.Cleanup();
  glBindVertexArray(0);
  glBindTexture(GL_TEXTURE_2D, 0);
  glDisable(GL_BLEND);
}

void Text::Render(const std::string &text, float x, float y, float scale,
                  const glm::vec4 &color, std::unique_ptr<FontObj> &obj) {

  prog_.SetUniform("Color", color);

  for (const auto &c : text) {
    if (!obj->IsLoaded(c)) {
      obj->LoadChar(c);
    }
    const auto &ch = obj->GetChar(c);

    const float x2 = x + static_cast<float>(ch.bearing.x) * scale;
    const float y2 = y - static_cast<float>(ch.size.y - ch.bearing.y) * scale;
    const float w = static_cast<float>(ch.size.x) * scale;
    const float h = static_cast<float>(ch.size.y) * scale;

    // VBOの更新
    GLfloat box[6 * 4] = {x2,   y2 + h, 0.0f,   0.0f,   x2,     y2,
                          0.0f, 1.0f,   x2 + w, y2,     1.0f,   1.0f,

                          x2,   y2 + h, 0.0f,   0.0f,   x2 + w, y2,
                          1.0f, 1.0f,   x2 + w, y2 + h, 1.0f,   0.0f};

    glBindTexture(GL_TEXTURE_2D, ch.texID);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(box), box);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glDrawArrays(GL_TRIANGLES, 0, 6);
    x += static_cast<float>(ch.advance >> 6) * scale;
  }
}
