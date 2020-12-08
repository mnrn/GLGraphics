#ifndef G_BUFFER_H
#define G_BUFFER_H

#include "GLInclude.h"

#include <array>
#include <vector>

class GBuffer {
public:
  ~GBuffer();

  void OnInit(int w, int h);
  void OnDestroy();

  GLuint GetDeferredFBO() const { return fbo_[Deferred]; }
  GLuint GetSSAOFBO() const { return fbo_[SSAO]; }
  GLuint GetAOTex() const { return textures_[AOTex]; }
  GLuint GetBlurAOTex() const { return textures_[BlurAOTex]; }

private:
  GLuint CreateGBufferTexture(GLenum texUnit, GLenum format);

  enum FramebufferObject {
    Deferred,
    SSAO,
    FramebufferObjectsNum,
  };
  enum Renderbuffer {
    DepthBuffer,
    RenderbuffersNum,
  };
  enum Texture {
    PosTex,
    NormTex,
    ColorTex,
    AOTex,
    BlurAOTex,
    TextureNum,
  };

  int width_, height_;
  std::array<GLuint, FramebufferObjectsNum> fbo_;
  std::array<GLuint, RenderbuffersNum> renders_;
  std::array<GLuint, TextureNum> textures_; // GBuffer textures
};

#endif
