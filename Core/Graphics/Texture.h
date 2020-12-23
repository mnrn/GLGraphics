/**
 * @brief Texture helper
 */

#ifndef TEXTURE_H
#define TEXTURE_H

#include "GLInclude.h"
#include <string>

namespace Texture {
  GLuint Load(const std::string &path);
// GLuint LoadCubeMap(const std::string &name);
}

#endif
