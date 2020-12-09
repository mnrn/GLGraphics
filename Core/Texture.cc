/**
 * @brief Texture helper
 */

#include "Texture.h"

#include <boost/assert.hpp>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

namespace Pixels {
  static unsigned char *Load(const std::string &path, int &w, int &h,
                                   bool flip = true) {
    int bytesPerPix;
    stbi_set_flip_vertically_on_load(flip);
    return stbi_load(path.c_str(), &w, &h, &bytesPerPix, 4);
  }
  
  static void Free(unsigned char *data) { stbi_image_free(data); }
}

namespace Texture {
  GLuint Load(const std::string &path) {
    int w, h;
    unsigned char *data = Pixels::Load(path, w, h);
    if (data == nullptr) {
      std::cerr << "Failed to load " << path << std::endl;
      BOOST_ASSERT_MSG(false, "Failed to load texture.");
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
    Pixels::Free(data);
    return tex;
  }
}
