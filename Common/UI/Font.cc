/**
 * @brief  フォント
 * @date   2020/12/14
 */

// ********************************************************************************
// Include files
// ********************************************************************************

#include "UI/Font.h"

#include <boost/assert.hpp>

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

std::optional<FontObj> Font::Entry(const std::string &fontpath) {
  FT_Face face;
  if (FT_New_Face(ft_, fontpath.c_str(), 0, &face)) {
    BOOST_ASSERT_MSG(false, "Could not open font");
    return std::nullopt;
  }
  return FontObj(face);
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

bool FontObj::Load(unsigned int size) {
  if (face_ == nullptr) {
    return false;
  }

  FT_Set_Pixel_Sizes(face_, 0, size);

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  // とりあえずasciiのみに絞っておきます。
  for (GLubyte c = 0; c < 128; c++) {
    if (FT_Load_Char(face_, c, FT_LOAD_RENDER)) {
      continue;
    }

    GLuint tex;
    glCreateTextures(GL_TEXTURE_2D, 1, &tex);
    glTextureStorage2D(tex, 1, GL_R8, face_->glyph->bitmap.width,
                       face_->glyph->bitmap.rows);
    glTextureSubImage2D(tex, 0, 0, 0, face_->glyph->bitmap.width,
                        face_->glyph->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE,
                        face_->glyph->bitmap.buffer);

    glBindTexture(GL_TEXTURE_2D, tex);

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
