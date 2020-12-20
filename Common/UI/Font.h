/**
 * @brief  フォント
 * @date   2020/12/14
 */

#pragma once

// ********************************************************************************
// Include files
// ********************************************************************************

#include "GLInclude.h"

#include <cstdint>
#include <map>
#include <optional>
#include <string>
#include <vector>

#include <freetype2/ft2build.h>
#include FT_FREETYPE_H

#include "Utils/Singleton.h"

// ********************************************************************************
// Classes
// ********************************************************************************

struct FontChar {
  GLuint texID;
  glm::ivec2 size;
  glm::ivec2 bearing;
  GLuint advance;
};

class FontObj {
public:
  explicit FontObj(FT_Face face) : face_(face) {}
  ~FontObj() { OnDestroy(); }
  FontObj(const FontObj &) = default;

  static constexpr inline unsigned int kDefaultSize = 48;
  bool SetupWithSize(unsigned int size);
  bool SetupDefault() { return SetupWithSize(kDefaultSize); }
  const FontChar &GetChar(GLchar c) const { return chars_.at(c); }

private:
  void OnDestroy();
  FT_Face face_ = nullptr;
  std::map<GLchar, FontChar> chars_{};
};

class Font : public Singleton<Font> {
public:
  Font();
  ~Font();

  bool OnInit();
  bool OnDestroy();

  std::unique_ptr<FontObj> Entry(const std::string &fontpath);

private:
  FT_Library ft_ = nullptr;
};
