#pragma once

#include "GLInclude.h"

#include <optional>

enum struct LightType {
  Directional,
  Point,
  /*
  Spot,
  Capsule,
  */
  Ambient,
  Num,
};

struct Light {
  LightType type = LightType::Directional;

  glm::vec3 pos{0.0f};
  glm::vec3 color{0.0f};

  std::optional<glm::vec3> dir{};
  std::optional<glm::vec3> range{};
  std::optional<glm::vec3> amb{};
  std::optional<glm::vec3> spec{};

  Light(LightType t, const glm::vec3 &p, const glm::vec3 &c)
      : type(t), pos(p), color(c) {}
  Light(LightType t, const glm::vec3 &p, const glm::vec3 &c, const glm::vec3 &d)
      : type(t), pos(p), color(c), dir(d) {}
};
