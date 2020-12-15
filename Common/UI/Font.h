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
#include <optional>
#include <string>
#include <vector>
#include <map>

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
  ~FontObj() { OnDestroy();
  }

  bool Load(unsigned int size);
  const FontChar &GetChar(GLchar c) const { 
    return chars_.at(c);
  }

private:
  void OnDestroy();

  static constexpr inline unsigned int kDefaultSize = 48;
  FT_Face face_ = nullptr;
  unsigned int size_ = kDefaultSize;
  std::map<GLchar, FontChar> chars_{};
};

class Font : public Singleton<Font> {
public:
  Font();
  ~Font();

  bool OnInit();
  bool OnDestroy();
  
  std::optional<FontObj> Entry(const std::string &fontpath);

private:
  FT_Library ft_ = nullptr;
};
