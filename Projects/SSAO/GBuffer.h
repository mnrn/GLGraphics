#ifndef G_BUFFER_H
#define G_BUFFER_H

#include "GLInclude.h"

#include "SSAOCommon.h"

#include <array>
#include <vector>

class GBuffer {
public:
  ~GBuffer();

  void OnInit(int w, int h);
  void OnDestroy();

  GLuint GetDeferredFBO() const { return fbo_[to_i(FBO::Deferred)]; }
  GLuint GetSSAOFBO() const { return fbo_[to_i(FBO::SSAO)]; }
  GLuint GetSSAOBlurFBO() const { return fbo_[to_i(FBO::SSAOBlur)]; }

  GLuint GetPosTex() const { return textures_[to_i(Textures::Pos)]; }
  GLuint GetNormTex() const { return textures_[to_i(Textures::Norm)]; }
  GLuint GetColorTex() const { return textures_[to_i(Textures::Color)]; }
  GLuint GetAOTex() const { return textures_[to_i(Textures::AO)]; }
  GLuint GetBlurAOTex() const { return textures_[to_i(Textures::BlurAO)]; }

private:
  void InitDeferredFBO();
  void InitSSAOFBO();
  void InitSSAOBlurFBO();

  GLuint CreateGBufferTexture(GLenum format);

  enum struct FBO {
    Deferred,
    SSAO,
    SSAOBlur,
    Num,
  };
  enum struct Renderbuffers {
    DepthBuffer,
    Num,
  };
  enum struct Textures {
    Pos,
    Norm,
    Color,
    AO,
    BlurAO,
    Num,
  };

  int width_, height_;
  std::array<GLuint, to_i(FBO::Num)> fbo_;
  std::array<GLuint, to_i(Renderbuffers::Num)> renders_;
  std::array<GLuint, to_i(Textures::Num)> textures_; // GBuffer textures
};

#endif
