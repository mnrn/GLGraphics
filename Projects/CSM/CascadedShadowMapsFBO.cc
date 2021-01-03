#include "CascadedShadowMapsFBO.h"

#include <boost/assert.hpp>
#include <iostream>
#include <vector>

#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

CascadedShadowMapsFBO::~CascadedShadowMapsFBO() { OnDestroy(); }

bool CascadedShadowMapsFBO::OnInit(int cascades, int w, int h) {
  // シャドウマップ用のFBOを生成します。
  if (shadowFBO_ != 0) {
    glDeleteFramebuffers(1, &shadowFBO_);
  }
  glGenFramebuffers(1, &shadowFBO_);
  glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO_);
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // シャドウマップに使用する深度テクスチャ配列を生成します。
  if (depthTexAry_ != 0) {
    glDeleteTextures(1, &depthTexAry_);
  }
  glGenTextures(1, &depthTexAry_);
  glBindTexture(GL_TEXTURE_2D_ARRAY, depthTexAry_);
  glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_DEPTH_COMPONENT32F, w, h, cascades);
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_COMPARE_MODE,
                  GL_COMPARE_REF_TO_TEXTURE);

  glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

  const GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  return result == GL_FRAMEBUFFER_COMPLETE;
}

void CascadedShadowMapsFBO::OnDestroy() {
  if (depthTexAry_ != 0) {
    glDeleteTextures(1, &depthTexAry_);
  }
  if (shadowFBO_ != 0) {
    glDeleteFramebuffers(1, &shadowFBO_);
  }
}
