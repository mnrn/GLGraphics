#include "GBuffer.h"

GBuffer::~GBuffer() { Destroy(); }

void GBuffer::Init(int w, int h) {
  width_ = w;
  height_ = h;

  // FBOの生成とバインド
  glGenFramebuffers(1, &buffers_[DeferredFBO]);
  glBindFramebuffer(GL_FRAMEBUFFER, buffers_[DeferredFBO]);

  // 深度バッファの生成とバインド
  glGenRenderbuffers(1, &buffers_[DepthBuf]);
  glBindRenderbuffer(GL_RENDERBUFFER, buffers_[DepthBuf]);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, w, h);

  // 位置情報、法線情報、色情報を格納するためのテクスチャを生成
  textures_[PosTex] = CreateGBufferTexture(GL_TEXTURE0, GL_RGB32F);
  textures_[NormTex] = CreateGBufferTexture(GL_TEXTURE1, GL_RGB32F);
  textures_[ColorTex] = CreateGBufferTexture(GL_TEXTURE2, GL_RGB8);

  // テクスチャをFramebufferにアタッチします。
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                            GL_RENDERBUFFER, buffers_[DepthBuf]);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         textures_[PosTex], 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D,
                         textures_[NormTex], 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D,
                         textures_[ColorTex], 0);

  const GLenum drawBuffers[] = {GL_NONE, GL_COLOR_ATTACHMENT0,
                                GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
  glDrawBuffers(4, drawBuffers);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GBuffer::PrepareRender() const {
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, textures_[PosTex]);

  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, textures_[NormTex]);

  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, textures_[ColorTex]);
}

void GBuffer::Destroy() {
  glDeleteTextures(textures_.size(), textures_.data());
  glDeleteRenderbuffers(1, &buffers_[DepthBuf]);
  glDeleteFramebuffers(1, &buffers_[DeferredFBO]);
}

GLuint GBuffer::CreateGBufferTexture(GLenum texUnit, GLenum format) const {
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
