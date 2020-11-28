/**
 * @brief  Debug Utility
 * @date   2017/03/18
 */

#ifndef DEBUG_H
#define DEBUG_H

// ********************************************************************************
// Include files
// ********************************************************************************

#include <iostream>

#define FMT_HEADER_ONLY
#include <fmt/printf.h>

// ********************************************************************************
// Namespace
// ********************************************************************************

namespace Debug {

// ********************************************************************************
// Functions
// ********************************************************************************

[[maybe_unused]] static inline void APIENTRY
Callback(GLenum src, GLenum type, GLuint id, GLenum severity, GLsizei length,
         const GLchar *msg, const void *param) {

  UNUSED_VARIABLE(length);
  UNUSED_VARIABLE(param);

  std::string srcstr;
  switch (src) {
  case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
    srcstr = "WindowSystem";
    break;
  case GL_DEBUG_SOURCE_APPLICATION:
    srcstr = "Application";
    break;
  case GL_DEBUG_SOURCE_API:
    srcstr = "OpenGL";
    break;
  case GL_DEBUG_SOURCE_SHADER_COMPILER:
    srcstr = "Shader Compiler";
    break;
  case GL_DEBUG_SOURCE_THIRD_PARTY:
    srcstr = "3rd Party";
    break;
  case GL_DEBUG_SOURCE_OTHER:
    srcstr = "Other";
    break;
  default:
    srcstr = "Unknown";
    break;
  }

  std::string typestr;
  switch (type) {
  case GL_DEBUG_TYPE_ERROR:
    typestr = "Error";
    break;
  case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
    typestr = "Deprecated";
    break;
  case GL_DEBUG_TYPE_PORTABILITY:
    typestr = "Portability";
    break;
  case GL_DEBUG_TYPE_PERFORMANCE:
    typestr = "Performance";
    break;
  case GL_DEBUG_TYPE_MARKER:
    typestr = "Marker";
    break;
  case GL_DEBUG_TYPE_PUSH_GROUP:
    typestr = "PushGroup";
    break;
  case GL_DEBUG_TYPE_POP_GROUP:
    typestr = "PopGroup";
    break;
  case GL_DEBUG_TYPE_OTHER:
    typestr = "Other";
    break;
  default:
    typestr = "Unknown";
    break;
  }

  std::string sevstr;
  switch (severity) {
  case GL_DEBUG_SEVERITY_HIGH:
    sevstr = "High";
    break;
  case GL_DEBUG_SEVERITY_MEDIUM:
    sevstr = "Medium";
    break;
  case GL_DEBUG_SEVERITY_LOW:
    sevstr = "Low";
    break;
  case GL_DEBUG_SEVERITY_NOTIFICATION:
    sevstr = "Notify";
    break;
  default:
    sevstr = "Unknown";
    break;
  }

  fmt::printf("%s:%s[%s](%d): %s\n", srcstr.c_str(), typestr.c_str(),
              sevstr.c_str(), id, msg);
}

[[maybe_unused]] static inline int32_t CheckError(const char *filepath,
                                                  int32_t line) {

  GLenum glErr = glGetError();
  int32_t retcode = 0;
  while (glErr != GL_NO_ERROR) {
    std::string msg = "";
    switch (glErr) {
    case GL_INVALID_ENUM:
      msg = "Invalid enum.";
      break;
    case GL_INVALID_VALUE:
      msg = "Invalid value.";
      break;
    case GL_INVALID_OPERATION:
      msg = "Invalid operation.";
      break;
    case GL_INVALID_FRAMEBUFFER_OPERATION:
      msg = "Invalid framebuffer operation.";
      break;
    case GL_OUT_OF_MEMORY:
      msg = "Out of memory.";
      break;
    default:
      msg = "Unknown Error.";
      break;
    }

    fmt::printf("glError in file %s @line %d: %s\n", filepath, line, msg);
    retcode = 1;
    glErr = glGetError();
  }

  return retcode;
}

static inline void DumpInfo() {

  const GLubyte *renderer = glGetString(GL_RENDERER);
  const GLubyte *vendor = glGetString(GL_VENDOR);
  const GLubyte *version = glGetString(GL_VERSION);
  const GLubyte *glslver = glGetString(GL_SHADING_LANGUAGE_VERSION);

  GLint major, minor;
  glGetIntegerv(GL_MAJOR_VERSION, &major);
  glGetIntegerv(GL_MINOR_VERSION, &minor);

  std::cout
      << "------------------------------------------------------------------"
      << std::endl;
  fmt::printf("GL Renderer  : %s\n", renderer);
  fmt::printf("GL Vendor    : %s\n", vendor);
  fmt::printf("GL Version   : %s\n", version);
  fmt::printf("GL Version   : %d.%d\n", major, minor);
  fmt::printf("GLSL Version : %s\n", glslver);
  std::cout
      << "------------------------------------------------------------------"
      << std::endl;
}

static inline void SetupInfo() {
  // Dump GL Info
  Debug::DumpInfo();

  // Setup Debug Info
#ifndef __APPLE__
  glDebugMessageCallback(debug::callback, nullptr);
  glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr,
                        GL_TRUE);
  glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_MARKER, 0,
                       GL_DEBUG_SEVERITY_NOTIFICATION, -1, "start debugging..");
#endif
}

} // namespace Debug

#endif // DEBUG_H
