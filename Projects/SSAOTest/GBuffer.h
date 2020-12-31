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

  GLuint GetDeferredFBO() const { return buffers_[DeferredFBO]; }

  GLuint GetPosTex() const { return textures_[PosTex]; }
  GLuint GetNormTex() const { return textures_[NormTex]; }
  GLuint GetColorTex() const { return textures_[ColorTex]; }

private:
  GLuint CreateGBufferTexture(GLenum, GLenum);

  enum Buffer {
    DeferredFBO,
    DepthBuf,
    BufferNum,
  };
  enum Textures {
    PosTex,
    NormTex,
    ColorTex,
    TexturesNum,
  };

  int width_, height_;
  std::array<GLuint, BufferNum> buffers_;    // GBuffer render views
  std::array<GLuint, TexturesNum> textures_; // GBuffer textures
};

#endif
