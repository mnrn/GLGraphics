/**
 * @brief Texture helper
 */

#include "texture.h"

#include <stb/stb_image.h>

static unsigned char *loadPixels(const std::string &name, int &w, int &h,
                                 bool flip = true) {
  int bytesPerPix;
  stbi_set_flip_vertically_on_load(flip);
  return stbi_load(name.c_str(), &w, &h, &bytesPerPix, 4);
}

static void freePixels(unsigned char *data) { stbi_image_free(data); }

GLuint loadTexture(const std::string &name) {
  int w, h;
  unsigned char *data = loadPixels(name, w, h);
  if (data == nullptr) {
    return 0;
  }
  GLuint tex = 0;
  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_2D, tex);
  glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, w, h);
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE,
                  data);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  freePixels(data);
}
