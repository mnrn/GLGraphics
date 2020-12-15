/**
 * @brief  フォント
 * @date   2020/12/14
 */

// ********************************************************************************
// Include files
// ********************************************************************************

#include "UI/Font.h"

#include <boost/assert.hpp>

Font::Font() { OnInit(); }

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

std::optional<FT_Face> Font::Load(const std::string &fontpath) {
  FT_Face face;
  if (FT_New_Face(ft_, fontpath.c_str(), 0, &face)) {
    BOOST_ASSERT_MSG(false, "Could not open font");
    return std::nullopt;
  }
  return face;
}
