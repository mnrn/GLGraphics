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

  GLuint GetDeferredFBO() const { return fbuffers_[DeferredFBO]; }
  GLuint GetSSAOFBO() const { return fbuffers_[SSAOFBO]; }
  GLuint GetSSAOBlurFBO() const { return fbuffers_[SSAOBlurFBO]; }

  GLuint GetPosTex() const { return textures_[PosTex]; }
  GLuint GetNormTex() const { return textures_[NormTex]; }
  GLuint GetColorTex() const { return textures_[ColorTex]; }
  GLuint GetAOTex() const { return textures_[AOTex]; }
  GLuint GetAOBlurTex() const { return textures_[BlurAOTex]; }

private:
  void InitDeferredFBO();
  void InitSSAOFBO();
  void InitSSAOBlurFBO();

  GLuint CreateGBufferTexture(GLenum);

  enum Framebuffers {
    DeferredFBO,
    SSAOFBO,
    SSAOBlurFBO,
    FramebuffersNum,
  };
  enum Renderbuffers {
    Depth,
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
  std::array<GLuint, FramebuffersNum> fbuffers_{};
  std::array<GLuint, RenderbuffersNum> rbuffers_{};

  std::array<GLuint, TexturesNum> textures_{};
};

#endif
