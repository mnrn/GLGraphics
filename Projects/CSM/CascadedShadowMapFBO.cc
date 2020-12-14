#include "CascadedShadowMapFBO.h"

#include <boost/assert.hpp>
#include <iostream>
#include <vector>

#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

CascadedMapFBO::~CascadedMapFBO() { OnDestroy(); }

bool CascadedMapFBO::OnInit(int split, int w, int h) {
  // シャドウマップの生成を行います。
  shadowMaps_.resize(split);
  glGenTextures(static_cast<GLsizei>(shadowMaps_.size()), shadowMaps_.data());
  for (int i = 0; i < split; i++) {
    glBindTexture(GL_TEXTURE_2D, shadowMaps_[i]);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT24, w, h);

    // テクスチャの拡大・縮小の方法を指定します。
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // テクスチャの繰り返しの方法を指定します。
    const GLfloat border[] = {1.0f, 0.0f, 0.0f, 0.0f};
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border);

    // 書き込むポリゴンのテスクチャ座標のR値とテクスチャとの比較を行うように設定します。
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE,
                    GL_COMPARE_REF_TO_TEXTURE);
    // R値がテクスチャの値よりも小さい場合に真となります。(つまり日向となります。)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LESS);
  }

  // シャドウマップ用のFBOを生成し、デプステクスチャをアタッチします。
  glGenFramebuffers(1, &shadowFBO_);
  glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO_);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
                         shadowMaps_[0], 0);

  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  const GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  return result == GL_FRAMEBUFFER_COMPLETE;
}

void CascadedMapFBO::OnDestroy() {
  if (shadowFBO_ != 0) {
    glDeleteFramebuffers(1, &shadowFBO_);
  }
  if (!shadowMaps_.empty()) {
    glDeleteTextures(static_cast<GLsizei>(shadowMaps_.size()),
                     shadowMaps_.data());
  }
}


void CascadedMapFBO::BindForReading(const std::vector<GLuint>& texUnits) {
  BOOST_ASSERT_MSG(texUnits.size() == shadowMaps_.size(), "シャドウマップの枚数を確認してください。");
  for (std::size_t i = 0; i < texUnits.size(); i++) {
    glActiveTexture(texUnits[i]);
    glBindTexture(GL_TEXTURE_2D, shadowMaps_[i]);
  }
}

void CascadedMapFBO::BindForWriting() { 
  glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO_);

}

void CascadedMapFBO::AttachFramebuffer(int index) {
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
                         shadowMaps_[index], 0);
}
