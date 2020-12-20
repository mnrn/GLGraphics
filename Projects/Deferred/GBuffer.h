#ifndef G_BUFFER_H
#define G_BUFFER_H

#include "GLInclude.h"

#include <array>
#include <vector>

class GBuffer {
public:
  ~GBuffer();

  void OnInit(int w, int h);
  void OnPreRender() const;
  void OnDestroy();

  GLuint GetDeferredFBO() const { return buffers_[DeferredFBO]; }

private:
  GLuint CreateGBufferTexture(GLenum, GLenum);

  enum Buffer {
    DeferredFBO,
    DepthBuf,
    BufferNum,
  };
  enum Texture {
    PosTex,
    NormTex,
    ColorTex,
    TextureNum,
  };

  int width_, height_;
  std::array<GLuint, BufferNum> buffers_;   // GBuffer render views
  std::array<GLuint, TextureNum> textures_; // GBuffer textures
};

#endif
