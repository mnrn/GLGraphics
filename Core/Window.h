/**
 * @brief  Framework Window
 * @date   2017/03/19
 */

#ifndef WINDOW_H
#define WINDOW_H

// ********************************************************************************
// Including files
// ********************************************************************************

#include <boost/assert.hpp>
#include <thread>

#include "Debug.h"

// ********************************************************************************
// Namespace
// ********************************************************************************

namespace Window {

static inline GLFWwindow *Create(int w, int h, const char *title, int samples,
                                 GLFWmonitor *monitor = nullptr,
                                 GLFWwindow *share = nullptr) {

  // Select OpenGL version
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
#ifdef __APPLE__
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
#else
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
#endif
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
#if !defined(NDEBUG)
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif
  if (samples > 0) {
    glfwWindowHint(GLFW_SAMPLES, samples);
  }

  // create window handle
  GLFWwindow *handle = glfwCreateWindow(w, h, title, monitor, share);
  if (handle == nullptr) {
    glfwTerminate();
    BOOST_ASSERT_MSG(false, "failed to create window!");
    return nullptr;
  }

  glfwMakeContextCurrent(handle);
  return handle;
}

static inline GLFWwindow *Destroy(GLFWwindow *handle) {
  if (handle != nullptr) {
    glfwDestroyWindow(handle);
  }
  return nullptr;
}

} // namespace Window

#endif // WINDOW_H
