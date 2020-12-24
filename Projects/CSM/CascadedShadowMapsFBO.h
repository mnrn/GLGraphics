#ifndef CASCADED_SHADOW_MAPS_FBO_H
#define CASCADED_SHADOW_MAPS_FBO_H

#include "GLInclude.h"

#include <vector>
#include <glm/gtc/constants.hpp>
#include <optional>
#include <string>

class CascadedShadowMapsFBO {
public:
  CascadedShadowMapsFBO() :depthTexAry_(0), shadowFBO_(0) {}
  ~CascadedShadowMapsFBO();

  bool OnInit(int cascades, int w, int h);
  void OnDestroy();

  GLuint GetShadowFBO() const { return shadowFBO_; }
  GLuint GetDepthTextureArray() const { return depthTexAry_; }

private:
  GLuint depthTexAry_ = 0;
  GLuint shadowFBO_ = 0;
};

#endif
