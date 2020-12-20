/**
 * @brief  フォント
 * @date   2020/12/14
 */

// ********************************************************************************
// Include files
// ********************************************************************************

#include "UI/Font.h"

#include <boost/assert.hpp>
#include <iostream>

// ********************************************************************************
// Font class
// ********************************************************************************

Font::Font() { OnInit(); }
Font::~Font() { OnDestroy(); }

bool Font::OnInit() {
  if (ft_ != nullptr) {
    BOOST_ASSERT_MSG(false, "FT_Library handle may be already initialized.");
    return false;
  }
  if (FT_Init_FreeType(&ft_)) {
    BOOST_ASSERT_MSG(false, "Could not init freetype library.");
    return false;
  }
  return true;
}

bool Font::OnDestroy() {
  if (ft_ == nullptr) {
    return true;
  }
  return FT_Done_FreeType(ft_) == 0;
}

std::unique_ptr<FontObj> Font::Entry(const std::string &fontpath) {
  FT_Face face;
  if (FT_New_Face(ft_, fontpath.c_str(), 0, &face)) {
    BOOST_ASSERT_MSG(false, "Could not open font");
    return nullptr;
  }
  return std::make_unique<FontObj>(face);
}

// ********************************************************************************
// FontObj class
// ********************************************************************************

void FontObj::OnDestroy() {

  for (auto [k, v] : chars_) {
    glDeleteTextures(1, &v.texID);
  }

  if (face_ != nullptr) {
    FT_Done_Face(face_);
  }
}

bool FontObj::SetupWithSize(unsigned int size) {
  if (face_ == nullptr) {
    return false;
  }

  FT_Set_Pixel_Sizes(face_, 0, size);

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  // とりあえずasciiのみに絞っておきます。
  for (unsigned int c = 0; c < 256; c++) {
    if (FT_Load_Char(face_, c, FT_LOAD_RENDER)) {
      continue;
    }

    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face_->glyph->bitmap.width,
                 face_->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE,
                 face_->glyph->bitmap.buffer);

    // アーティファクトを防ぐためのクランプ
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // リニアフィルタリング
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);

    FontChar ch{
        tex, glm::ivec2(face_->glyph->bitmap.width, face_->glyph->bitmap.rows),
        glm::ivec2(face_->glyph->bitmap_left, face_->glyph->bitmap_top),
        static_cast<GLuint>(face_->glyph->advance.x)};
    chars_.emplace(c, ch);
  }
  return true;
}
