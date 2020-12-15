/**
 * @brief  テキスト
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

#include "Primitive/Drawable.h"
#include "Shader.h"
#include "Utils/Singleton.h"
#include "UI/Font.h"

// ********************************************************************************
// Classes
// ********************************************************************************

class Text : public Singleton<Text> {
public:
  Text();
  ~Text();
  void Render(const std::string &text, 
    float x, float y, float scale,
    float winWidth, float winHeight, const glm::vec4& color, 
    const std::unique_ptr<FontObj>& obj) const;

private:
  std::optional<std::string> CompileAndLinkShader();

  ShaderProgram prog_{};
  GLuint vao_ = 0;
  GLuint vbo_ = 0;
};
