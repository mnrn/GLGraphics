#ifndef G_BUFFER_H
#define G_BUFFER_H

#include "GLInclude.h"

#include <array>
#include <vector>

class GBuffer {
public:
  ~GBuffer();

  void OnInit(int w, int h);
  void OnPreRender();
  void OnDestroy();

  GLuint GetDeferredFBO() const { return fbo_[Deferred]; }
  GLuint GetSSAOFBO() const { return fbo_[SSAO]; }

  GLuint GetPosTex() const { return textures_[PosTex]; }
  GLuint GetNormTex() const { return textures_[NormTex]; }
  GLuint GetColorTex() const { return textures_[ColorTex]; }
  GLuint GetAOTex() const { return textures_[AOTex]; }
  GLuint GetBlurAOTex() const { return textures_[BlurAOTex]; }

private:
  GLuint CreateGBufferTexture(GLenum texUnit, GLenum format);

  enum FramebufferObjects {
    Deferred,
    SSAO,
    FramebufferObjectsNum,
  };
  enum Renderbuffers {
    DepthBuffer,
    RenderbuffersNum,
  };
  enum Textures {
    PosTex,
    NormTex,
    ColorTex,
    AOTex,
    BlurAOTex,
    TexturesNum,
  };

  int width_, height_;
  std::array<GLuint, FramebufferObjectsNum> fbo_;
  std::array<GLuint, RenderbuffersNum> renders_;
  std::array<GLuint, TexturesNum> textures_; // GBuffer textures
};

#endif
