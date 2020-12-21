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
#include <utility>
#include <vector>

#include <freetype2/ft2build.h>
#include FT_FREETYPE_H

#include "Graphics/Shader.h"
#include "Primitive/Drawable.h"
#include "UI/Font.h"
#include "Utils/Singleton.h"

// ********************************************************************************
// Classes
// ********************************************************************************

struct TextConfig {
  std::optional<float> windowWidth = std::nullopt;
  std::optional<float> windowHeight = std::nullopt;
  std::optional<float> fontScale = std::nullopt;
  std::optional<glm::vec4> textColor = std::nullopt;

  TextConfig() = default;
  TextConfig(const TextConfig &config) = default;
  TextConfig &operator=(const TextConfig &) = default;
  TextConfig(int w, int h)
      : TextConfig(static_cast<float>(w), static_cast<float>(h)) {}
  TextConfig(float w, float h) { Setup(w, h, std::nullopt, std::nullopt); }

  void Setup(float w, float h, std::optional<float> s,
             std::optional<glm::vec4> c) {
    windowWidth = w;
    windowHeight = h;
    fontScale = s;
    textColor = c;
  }
  void Cleanup() {
    textColor = std::nullopt;
    fontScale = std::nullopt;
    windowHeight = std::nullopt;
    windowWidth = std::nullopt;
  }
};

class Text : public Singleton<Text> {
public:
  Text();
  ~Text();

  void Begin(int w, int h) { Begin({w, h}); }
  void Begin(const TextConfig &);
  void End();

  void Render(const std::string &text, float x, float y,
              const std::unique_ptr<FontObj> &obj) {
    const float scale = config_.fontScale.value_or(1.0f);
    Render(text, x, y, scale, obj);
  }
  void Render(const std::string &text, float x, float y, const glm::vec4 &color,
              const std::unique_ptr<FontObj> &obj) {
    const float scale = config_.fontScale.value_or(1.0f);
    Render(text, x, y, scale, color, obj);
  }
  void Render(const std::string &text, float x, float y, float scale,
              const std::unique_ptr<FontObj> &obj) {
    const glm::vec4 color =
        config_.textColor.value_or(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    Render(text, x, y, scale, color, obj);
  }
  void Render(const std::string &text, float x, float y, float scale,
              const glm::vec4 &color, const std::unique_ptr<FontObj> &obj);

private:
  std::optional<std::string> CompileAndLinkShader();

  TextConfig config_{};
  ShaderProgram prog_{};
  GLuint vao_ = 0;
  GLuint vbo_ = 0;
};
