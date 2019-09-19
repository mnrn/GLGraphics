/**
 * @brief  Debug Utility
 * @date   2017/03/18
 */

#ifndef DEBUG_HPP
#define DEBUG_HPP

// ********************************************************************************
// Include files
// ********************************************************************************

#include <boost/format.hpp>
#include <iostream>

// ********************************************************************************
// Namespace
// ********************************************************************************

namespace debug {

// ********************************************************************************
// Functions
// ********************************************************************************

[[maybe_unused]] static void APIENTRY callback(GLenum src, GLenum type, GLuint id,
                              GLenum severity, GLsizei length,
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

  std::cout << boost::format("%s:%s[%s](%d): %s") % srcstr.c_str() %
                   typestr.c_str() % sevstr.c_str() % id % msg
            << std::endl;
}

[[maybe_unused]] static int32_t check_error(const char *filepath, int32_t line) {

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

    std::cout << boost::format("glError in file %s @line %d: %s\n") % filepath %
                     line % msg
              << std::endl;
    retcode = 1;
    glErr = glGetError();
  }

  return retcode;
}

static void dump_info() {

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
  std::cout << boost::format("GL Renderer  : %s") % renderer << std::endl;
  std::cout << boost::format("GL Vendor    : %s") % vendor << std::endl;
  std::cout << boost::format("GL Version   : %s") % version << std::endl;
  std::cout << boost::format("GL Version   : %d.%d") % major % minor
            << std::endl;
  std::cout << boost::format("GLSL Version : %s") % glslver << std::endl;
  std::cout
      << "------------------------------------------------------------------"
      << std::endl;
}

static void setup_info() {
  // Dump GL Info
  debug::dump_info();

  // Setup Debug Info
#ifndef __APPLE__
  glDebugMessageCallback(debug::callback, nullptr);
  glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr,
                        GL_TRUE);
  glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_MARKER, 0,
                       GL_DEBUG_SEVERITY_NOTIFICATION, -1, "start debugging..");
#endif
}

} // end namespace debug


#endif // DEBUG_HPP
