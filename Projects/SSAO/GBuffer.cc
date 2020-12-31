#include "GBuffer.h"

#include <boost/assert.hpp>
#include <iostream>

GBuffer::~GBuffer() { OnDestroy(); }

void GBuffer::OnInit(int w, int h) {
  width_ = w;
  height_ = h;

  InitDeferredFBO();
}

void GBuffer::OnDestroy() {
  glDeleteTextures(static_cast<GLsizei>(textures_.size()), textures_.data());
  glDeleteRenderbuffers(static_cast<GLsizei>(renders_.size()), renders_.data());
  glDeleteFramebuffers(static_cast<GLsizei>(fbo_.size()), fbo_.data());
}

void GBuffer::InitDeferredFBO() {
  // 遅延シェーディング用のFBOの生成とバインド
  glGenFramebuffers(1, &fbo_[to_i(FBO::Deferred)]);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo_[to_i(FBO::Deferred)]);

  // 深度バッファを生成してアタッチします。
  glGenRenderbuffers(1, &renders_[to_i(Renderbuffers::DepthBuffer)]);
  glBindRenderbuffer(GL_RENDERBUFFER,
                     renders_[to_i(Renderbuffers::DepthBuffer)]);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width_, height_);

  // 位置情報、法線情報、色情報、AO情報を格納するためのテクスチャを生成
  textures_[to_i(Textures::Pos)] = CreateGBufferTexture(GL_RGB32F);
  textures_[to_i(Textures::Norm)] = CreateGBufferTexture(GL_RGB32F);
  textures_[to_i(Textures::Color)] = CreateGBufferTexture(GL_RGB8);

  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                            GL_RENDERBUFFER, to_i(Renderbuffers::DepthBuffer));
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         textures_[to_i(Textures::Pos)], 0);

  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D,
                         textures_[to_i(Textures::Norm)], 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D,
                         textures_[to_i(Textures::Color)], 0);

  // 描画に使用するカラーアタッチメントをGLに通知します。
  const std::vector<GLenum> drawBuffers = {
      GL_COLOR_ATTACHMENT0,
      GL_COLOR_ATTACHMENT1,
      GL_COLOR_ATTACHMENT2,
  };
  glDrawBuffers(static_cast<GLsizei>(drawBuffers.size()), drawBuffers.data());

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    std::cerr << "Framebuffer not complete." << std::endl;
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GBuffer::InitSSAOFBO() {
  // AO用のFBOの生成とバインド
  glGenFramebuffers(1, &fbo_[to_i(FBO::SSAO)]);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo_[to_i(FBO::SSAO)]);

  // AO用のテクスチャの生成とアタッチ
  textures_[to_i(Textures::AO)] = CreateGBufferTexture(GL_R16F);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         textures_[to_i(Textures::AO)], 0);

  BOOST_ASSERT_MSG(glCheckFramebufferStatus(GL_FRAMEBUFFER) ==
                       GL_FRAMEBUFFER_COMPLETE,
                   "SSAO Framebuffer not complete.");
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GBuffer::InitSSAOBlurFBO() {
  // BlurAO用のFBOの生成とバインド
  glGenFramebuffers(1, &fbo_[to_i(FBO::SSAOBlur)]);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo_[to_i(FBO::SSAOBlur)]);

  // BlurAO用のテクスチャの生成とアタッチ
  textures_[to_i(Textures::BlurAO)] = CreateGBufferTexture(GL_R16F);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         textures_[to_i(Textures::BlurAO)], 0);

  BOOST_ASSERT_MSG(glCheckFramebufferStatus(GL_FRAMEBUFFER) ==
                       GL_FRAMEBUFFER_COMPLETE,
                   "SSAO Blur Framebuffer is not complete.");
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLuint GBuffer::CreateGBufferTexture(GLenum format) {
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
