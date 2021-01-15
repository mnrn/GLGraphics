#include "GBuffer.h"

#include <iostream>

GBuffer::~GBuffer() { OnDestroy(); }

void GBuffer::OnInit(int w, int h) {
  width_ = w;
  height_ = h;

  InitDeferredFBO();
  InitSSAOFBO();
  InitSSAOBlurFBO();
}

void GBuffer::OnDestroy() {
  glDeleteTextures(static_cast<GLsizei>(textures_.size()), textures_.data());
  glDeleteRenderbuffers(static_cast<GLsizei>(rbuffers_.size()),
                        rbuffers_.data());
  glDeleteFramebuffers(static_cast<GLsizei>(fbuffers_.size()),
                       fbuffers_.data());
}

void GBuffer::InitDeferredFBO() {
  // FBOの生成とバインド
  glGenFramebuffers(1, &fbuffers_[DeferredFBO]);
  glBindFramebuffer(GL_FRAMEBUFFER, fbuffers_[DeferredFBO]);

  // 深度バッファの生成とバインド
  glGenRenderbuffers(1, &rbuffers_[Depth]);
  glBindRenderbuffer(GL_RENDERBUFFER, rbuffers_[Depth]);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width_, height_);

  // 位置情報、法線情報、色情報を格納するためのテクスチャを生成
  textures_[PosTex] = CreateGBufferTexture(GL_RGB32F);
  textures_[NormTex] = CreateGBufferTexture(GL_RGB32F);
  textures_[ColorTex] = CreateGBufferTexture(GL_RGB8);

  // テクスチャをFramebufferにアタッチします。
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                            GL_RENDERBUFFER, rbuffers_[Depth]);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         textures_[PosTex], 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D,
                         textures_[NormTex], 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D,
                         textures_[ColorTex], 0);

  const GLenum drawBuffers[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1,
                                GL_COLOR_ATTACHMENT2};
  glDrawBuffers(3, drawBuffers);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    std::cerr << "Framebuffer not complete." << std::endl;
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GBuffer::InitSSAOFBO() {
  // AO用のFBOの生成とバインド
  glGenFramebuffers(1, &fbuffers_[SSAOFBO]);
  glBindFramebuffer(GL_FRAMEBUFFER, fbuffers_[SSAOFBO]);

  // AO用のテクスチャの生成とアタッチ
  textures_[AOTex] = CreateGBufferTexture(GL_R16F);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         textures_[AOTex], 0);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    std::cerr << "SSAO Framebuffer not complete." << std::endl;
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GBuffer::InitSSAOBlurFBO() {
  // BlurAO用のFBOの生成とバインド
  glGenFramebuffers(1, &fbuffers_[SSAOBlurFBO]);
  glBindFramebuffer(GL_FRAMEBUFFER, fbuffers_[SSAOBlurFBO]);

  // BlurAO用のテクスチャの生成とアタッチ
  textures_[BlurAOTex] = CreateGBufferTexture(GL_R16F);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         textures_[BlurAOTex], 0);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    std::cerr << "SSAO blur Framebuffer not complete." << std::endl;
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLuint GBuffer::CreateGBufferTexture(GLenum format) const {
  GLuint texId;

  glGenTextures(1, &texId);
  glBindTexture(GL_TEXTURE_2D, texId);

  glTexStorage2D(GL_TEXTURE_2D, 1, format, width_, height_);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

  glBindTexture(GL_TEXTURE_2D, 0);

  return texId;
}
