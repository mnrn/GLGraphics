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
  FontObj() = default;
  explicit FontObj(FT_Face face) : face_(face) {}
  ~FontObj() { OnDestroy(); }
  FontObj(const FontObj &) = default;

  static constexpr inline std::size_t kDefaultSize = 48;
  bool SetupWithSize(std::size_t size);
  bool SetupDefault() { return SetupWithSize(kDefaultSize); }
  bool LoadChar(char32_t c);

  const FontChar &GetChar(char32_t c) const { return chars_.at(c); }
  bool IsLoaded(char32_t c) const { return chars_.count(c) > 0; }

private:
  void OnDestroy();
  FT_Face face_ = nullptr;
  std::map<char32_t, FontChar> chars_{};
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
