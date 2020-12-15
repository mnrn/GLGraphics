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

#include <freetype2/ft2build.h>
#include FT_FREETYPE_H

#include "Utils/Singleton.h"

// ********************************************************************************
// Classes
// ********************************************************************************

class Font : public Singleton<Font> {
public:
  Font();
  ~Font();

  bool OnInit();
  bool OnDestroy();
  
  std::optional<FT_Face> Load(const std::string &fontpath);

private:
  FT_Library ft_ = nullptr;
};
