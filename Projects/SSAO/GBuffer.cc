#include "GBuffer.h"

#include <iostream>
#include <boost/assert.hpp>

GBuffer::~GBuffer() { OnDestroy(); }

void GBuffer::OnInit(int w, int h) {
  width_ = w;
  height_ = h;

  // 位置情報、法線情報、色情報、AO情報を格納するためのテクスチャを生成
  textures_[PosTex] = CreateGBufferTexture(GL_TEXTURE0, GL_RGB32F);
  textures_[NormTex] = CreateGBufferTexture(GL_TEXTURE1, GL_RGB32F);
  textures_[ColorTex] = CreateGBufferTexture(GL_TEXTURE2, GL_RGB8);
  textures_[AOTex] = CreateGBufferTexture(GL_TEXTURE3, GL_R16F);
  textures_[BlurAOTex] = CreateGBufferTexture(GL_TEXTURE3, GL_R16F);

  // 遅延シェーディング用のFBOの生成とバインド
  glGenFramebuffers(1, &fbo_[Deferred]);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo_[Deferred]);

  // 深度バッファの生成とバインド
  glGenRenderbuffers(1, &renders_[DepthBuffer]);
  glBindRenderbuffer(GL_RENDERBUFFER, renders_[DepthBuffer]);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, w, h);

  // テクスチャをFramebufferにアタッチします。
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                            GL_RENDERBUFFER, renders_[DepthBuffer]);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         textures_[PosTex], 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D,
                         textures_[NormTex], 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D,
                         textures_[ColorTex], 0);

  const std::vector<GLenum> drawBuffers = {GL_NONE, GL_COLOR_ATTACHMENT0,
                                           GL_COLOR_ATTACHMENT1,
                                           GL_COLOR_ATTACHMENT2, GL_NONE};
  glDrawBuffers(static_cast<GLsizei>(drawBuffers.size()), drawBuffers.data());
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // AO用のFBOの生成とバインド
  glGenFramebuffers(1, &fbo_[SSAO]);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo_[SSAO]);

  // テクスチャをAO用のFramebufferにアタッチします。
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         textures_[AOTex], 0);
  const std::vector<GLenum> aoDrawBuffers = {GL_NONE, GL_NONE, GL_NONE, GL_NONE,
                                             GL_COLOR_ATTACHMENT0};
  glDrawBuffers(static_cast<GLsizei>(aoDrawBuffers.size()),
                aoDrawBuffers.data());

  BOOST_ASSERT_MSG(glCheckFramebufferStatus(GL_FRAMEBUFFER) ==
                       GL_FRAMEBUFFER_COMPLETE,
                   "Framebuffer is not complete.");
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GBuffer::OnDestroy() {
  glDeleteTextures(static_cast<GLsizei>(textures_.size()), textures_.data());
  glDeleteRenderbuffers(static_cast<GLsizei>(renders_.size()), renders_.data());
  glDeleteFramebuffers(static_cast<GLsizei>(fbo_.size()), fbo_.data());
}

void GBuffer::OnPreRender() { 
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, textures_[PosTex]);

  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, textures_[NormTex]);

  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, textures_[ColorTex]);
}

GLuint GBuffer::CreateGBufferTexture(GLenum texUnit, GLenum format) {
  GLuint texId;

  glActiveTexture(texUnit);
  glGenTextures(1, &texId);
  glBindTexture(GL_TEXTURE_2D, texId);

  glTexStorage2D(GL_TEXTURE_2D, 1, format, width_, height_);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

  glBindTexture(GL_TEXTURE_2D, 0);

  return texId;
}
