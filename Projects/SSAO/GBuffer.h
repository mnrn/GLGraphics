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

  [[nodiscard]] GLuint GetDeferredFBO() const { return fbuffers_[DeferredFBO]; }
  [[nodiscard]] GLuint GetSSAOFBO() const { return fbuffers_[SSAOFBO]; }
  [[nodiscard]] GLuint GetSSAOBlurFBO() const { return fbuffers_[SSAOBlurFBO]; }

  [[nodiscard]] GLuint GetPosTex() const { return textures_[PosTex]; }
  [[nodiscard]] GLuint GetNormTex() const { return textures_[NormTex]; }
  [[nodiscard]] GLuint GetColorTex() const { return textures_[ColorTex]; }
  [[nodiscard]] GLuint GetAOTex() const { return textures_[AOTex]; }
  [[nodiscard]] GLuint GetBlurAOTex() const { return textures_[BlurAOTex]; }

private:
  void InitDeferredFBO();
  void InitSSAOFBO();
  void InitSSAOBlurFBO();

  [[nodiscard]] GLuint CreateGBufferTexture(GLenum) const;

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

  int width_ = 0, height_ = 0;
  std::array<GLuint, FramebuffersNum> fbuffers_{};
  std::array<GLuint, RenderbuffersNum> rbuffers_{};
  std::array<GLuint, TexturesNum> textures_{};
};

#endif
