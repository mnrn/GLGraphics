/**
 * @brief  テキスト
 * @date   2020/12/14
 */

// ********************************************************************************
// Include files
// ********************************************************************************

#include "UI/Text.h"

#include <boost/assert.hpp>
#include <iostream>

Text::Text() {
  if (const auto msg = CompileAndLinkShader()) {
    std::cerr << msg.value() << std::endl;
    BOOST_ASSERT_MSG(false, "Could not complie or link shader");
  }

  glGenBuffers(1, &fbo_);

  glActiveTexture(GL_TEXTURE0);
  glGenTextures(1, &tex_);
  glBindTexture(GL_TEXTURE_2D, tex_);

  // 1byte アライメント
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  // アーティファクトを防ぐためのクランプ
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  // リニアフィルタリング
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glBindTexture(GL_TEXTURE_2D, 0);
}

Text::~Text() {
  glDeleteTextures(1, &tex_);
  glDeleteBuffers(1, &fbo_);
}

void Text::Render(const std::string &text, float x, float y, float sx, float sy,
                  FT_Face face) const {

  glBindTexture(GL_TEXTURE_2D, tex_);
  prog_.Use();
  prog_.SetUniform("Tex", 0);

  // 頂点情報のためにFBOを設定する。
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, fbo_);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

  const FT_GlyphSlot g = face->glyph;
  for (const auto &p : text) {
    if (FT_Load_Char(face, p, FT_LOAD_RENDER)) {
      continue;
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, g->bitmap.width, g->bitmap.rows, 0,
                 GL_ALPHA, GL_UNSIGNED_BYTE, g->bitmap.buffer);

    const float x2 = x + g->bitmap_left * sx;
    const float y2 = -y - g->bitmap_top * sy;
    const float w = g->bitmap.width * sx;
    const float h = g->bitmap.rows * sy;

    const float box[16] = {
        x2, -y2,     0, 0, x2 + w, -y2,     1, 0,
        x2, -y2 - h, 0, 1, x2 + w, -y2 - h, 1, 1,
    };

    glBufferData(GL_ARRAY_BUFFER, sizeof(box), box, GL_DYNAMIC_DRAW);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    x += (g->advance.x / 64) * sx;
    y += (g->advance.y / 64) * sy;
  }

  glDisableVertexAttribArray(0);
}

std::optional<std::string> Text::CompileAndLinkShader() {
  if (prog_.Compile("./Assets/Shaders/UI/Text.vs.glsl", ShaderType::Vertex) &&
      prog_.Compile("./Assets/Shaders/UI/Text.fs.glsl", ShaderType::Fragment) &&
      prog_.Link()) {
    return std::nullopt;
  } else {
    return prog_.GetLog();
  }
}
