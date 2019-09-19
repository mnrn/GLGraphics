/**
 * @brief  Framework Window
 * @date   2017/03/19
 */

#ifndef WINDOW_HPP
#define WINDOW_HPP

// ********************************************************************************
// Include files
// ********************************************************************************

#include <thread>

#include "timer.hpp"
#include "debug.hpp"

// ********************************************************************************
// Namespace
// ********************************************************************************

namespace window {

GLFWwindow *create(int w, int h, const char *title,
                   GLFWmonitor *monitor = nullptr,
                   GLFWwindow *share = nullptr) {

  // Select OpenGL version
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
#ifdef __APPLE__
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
#else
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
#endif
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

  // create window handle
  GLFWwindow *handle_ = glfwCreateWindow(w, h, title, monitor, share);
  if (handle_ == nullptr) {
    glfwTerminate();
    BOOST_ASSERT_MSG(false, "failed to create window!");
  }

  glfwMakeContextCurrent(handle_);
  return handle_;
}

GLFWwindow *destroy(GLFWwindow *handle) {
  if (handle != nullptr) {
    glfwDestroyWindow(handle);
  }
  return nullptr;
}

template <typename F> void loop(GLFWwindow *handle, F callback) {

  if (handle == nullptr) {
    return;
  }

  // using precision_t = double;
  // static constexpr double periodic = 1.0 / 60.0;

  timer timer;
  while (glfwWindowShouldClose(handle) == false &&
         glfwGetKey(handle, GLFW_KEY_ESCAPE) == false) {

    timer.start();

    callback();
    glfwSwapBuffers(handle);
    glfwPollEvents();

    timer.end();

    // precision_t elapsed = timer_.elapsed();
    // if (elapsed < periodic_) {
    //     precision_t sleeptime = periodic_ - elapsed;
    //     std::this_thread::sleep_for(std::chrono::duration<precision_t>(sleeptime));
    // }
  }
}

} // end namespace window

#endif // WINDOW_HPP
