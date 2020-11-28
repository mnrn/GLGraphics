/**
 * @brief  Framework Window
 * @date   2017/03/19
 */

#ifndef WINDOW_H
#define WINDOW_H

// ********************************************************************************
// Include files
// ********************************************************************************

#include <boost/assert.hpp>
#include <thread>

#include "Debug.h"
#include "Timer.hh"

// ********************************************************************************
// Namespace
// ********************************************************************************

namespace Window {

static inline GLFWwindow *Create(int w, int h, const char *title,
                                 GLFWmonitor *monitor = nullptr,
                                 GLFWwindow *share = nullptr) {

  // Select OpenGL version
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
#ifdef __APPLE__
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
#else
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
#endif
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

  // create window handle
  GLFWwindow *handle = glfwCreateWindow(w, h, title, monitor, share);
  if (handle == nullptr) {
    glfwTerminate();
    BOOST_ASSERT_MSG(false, "failed to create window!");
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
