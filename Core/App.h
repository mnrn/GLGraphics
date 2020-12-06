/**
 * @brief アプリに関するクラスを扱います。
 */

#ifndef APP_H
#define APP_H

// ********************************************************************************
// Include files
// ********************************************************************************

#include <boost/assert.hpp>
#include <boost/noncopyable.hpp>
#include <iostream>

#include "Common.h"
#include "GLInclude.h"

#include "Debug.h"
#include "Window.h"

// ********************************************************************************
// Class
// ********************************************************************************

class App : private boost::noncopyable {
public:
  App(const char *appName, int w = 1280, int h = 720) {
    if (glfwInit() == GL_FALSE) {
      BOOST_ASSERT_MSG(false, "glfw Initialization failed!");
    }

    window_ = Window::Create(w, h, appName);
    glfwGetFramebufferSize(window_, &width_, &height_);

    InitGlad();
#if (!NDEBUG)
    Debug::SetupInfo();
#endif
  }

  ~App() {
    Window::Destroy(window_);
    glfwTerminate();
  }

  template <typename Initialize, typename Update, typename Render>
  int Run(Initialize onInit, Update onUpdate, Render onRender) {
    if (window_ == nullptr) {
      return EXIT_FAILURE;
    }

    onInit(width_, height_);

    while (!glfwWindowShouldClose(window_) &&
           !glfwGetKey(window_, GLFW_KEY_ESCAPE)) {

#if (!NDEBUG)
      Debug::CheckForOpenGLError(__FILE__, __LINE__);
#endif
      onUpdate(static_cast<float>(glfwGetTime()));
      onRender();

      glfwSwapBuffers(window_);
      glfwPollEvents();
    }

#if (!NDEBUG)
    Debug::CleanupInfo();
#endif
    return EXIT_SUCCESS;
  }

private:
  GLFWwindow *window_ = nullptr;
  int width_;
  int height_;

  static void InitGlad() {
    // Initialize GLAD
    if (!gladLoadGL()) {
      BOOST_ASSERT_MSG(false, "Something went wrong!");
    }
  }
};

#endif // APP_HPP
