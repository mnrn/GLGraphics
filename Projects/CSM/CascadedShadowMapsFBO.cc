#include "CascadedShadowMapsFBO.h"

#include <boost/assert.hpp>
#include <iostream>
#include <vector>

#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

CascadedShadowMapsFBO::~CascadedShadowMapsFBO() { OnDestroy(); }

bool CascadedShadowMapsFBO::OnInit(int cascades, int w, int h) {
  // シャドウマップの生成を行います。
  shadowMaps_.resize(cascades);

  glGenTextures(static_cast<GLsizei>(shadowMaps_.size()), shadowMaps_.data());
  for (int i = 0; i < cascades; i++) {
    glBindTexture(GL_TEXTURE_2D, shadowMaps_[i]);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32, w, h);

    // テクスチャの拡大・縮小の方法を指定します。
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // テクスチャの繰り返しの方法を指定します。
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // テクスチャとフラグメントの比較はしません。
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
  }

  // シャドウマップ用のFBOを生成し、テクスチャをアタッチします。
  glGenFramebuffers(1, &shadowFBO_);
  glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO_);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
                         shadowMaps_[0], 0);

  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);

  glBindTexture(GL_TEXTURE_2D, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  const GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  return result == GL_FRAMEBUFFER_COMPLETE;
}

void CascadedShadowMapsFBO::OnDestroy() {
  if (shadowFBO_ != 0) {
    glDeleteFramebuffers(1, &shadowFBO_);
  }
  if (!shadowMaps_.empty()) {
    glDeleteTextures(static_cast<GLsizei>(shadowMaps_.size()),
                     shadowMaps_.data());
  }
}


void CascadedShadowMapsFBO::BindForReading(const std::vector<GLuint>& texUnits) {
  BOOST_ASSERT_MSG(texUnits.size() == shadowMaps_.size(), "シャドウマップの枚数を確認してください。");
  for (std::size_t i = 0; i < texUnits.size(); i++) {
    glActiveTexture(texUnits[i]);
    glBindTexture(GL_TEXTURE_2D, shadowMaps_[i]);
  }
}

void CascadedShadowMapsFBO::BindForWriting(int index) { 
  glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO_);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
                         shadowMaps_[index], 0);
}
