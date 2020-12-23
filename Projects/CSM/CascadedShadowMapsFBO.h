#ifndef CASCADED_SHADOW_MAPS_FBO_H
#define CASCADED_SHADOW_MAPS_FBO_H

#include "GLInclude.h"

#include <vector>
#include <glm/gtc/constants.hpp>
#include <optional>
#include <string>

class CascadedShadowMapsFBO {
public:
  ~CascadedShadowMapsFBO();

  bool OnInit(int cascades, int w, int h);
  void OnDestroy();

  void BindForWriting(int index);
  void BindForReading(const std::vector<GLuint>& texUnits);

  GLuint GetShadowFBO() const { return shadowFBO_; }

private:
  std::vector<GLuint> shadowMaps_{};
  GLuint shadowFBO_ = 0;
};

#endif
