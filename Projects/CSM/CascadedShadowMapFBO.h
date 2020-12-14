#ifndef CASCADED_SHADOW_MAP_FBO_H
#define CASCADED_SHADOW_MAP_FBO_H

#include "GLInclude.h"

#include <vector>
#include <glm/gtc/constants.hpp>
#include <optional>
#include <string>

class CascadedMapFBO {
public:
  ~CascadedMapFBO();

  bool OnInit(int split, int w, int h);
  void OnDestroy();

  void BindForWriting();
  void AttachFramebuffer(int index);
  void BindForReading(const std::vector<GLuint>& texUnits);

  GLuint GetShadowFBO() const { return shadowFBO_; }

private:
  std::vector<GLuint> shadowMaps_{};
  GLuint shadowFBO_ = 0;
};

#endif
