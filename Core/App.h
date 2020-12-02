/**
 * @brief アプリに関するクラスを扱います。
 */

#ifndef APP_H
#define APP_H

// ********************************************************************************
// Include files
// ********************************************************************************

#include <boost/noncopyable.hpp>

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
    glfwInit();

    window_ = Window::Create(w, h, appName);
    width_ = w;
    height_ = h;

    InitGlew();
    Debug::SetupInfo();
  }

  ~App() {
    Window::Destroy(window_);
    glfwTerminate();
  }

  template <typename Init, typename Update, typename Render>
  int Run(Init onInit, Update onUpdate, Render onRender) {
    if (window_ == nullptr) {
      return EXIT_FAILURE;
    }

    onInit(width_, height_);

    // using precision_t = double;
    // static constexpr double periodic = 1.0 / 60.0;

    Timer timer;
    while (glfwWindowShouldClose(window_) == false &&
           glfwGetKey(window_, GLFW_KEY_ESCAPE) == false) {

      timer.Start();

      onUpdate(static_cast<float>(glfwGetTime()));
      onRender();

      glfwSwapBuffers(window_);
      glfwPollEvents();

      timer.End();

      // precision_t elapsed = timer_.elapsed();
      // if (elapsed < periodic_) {
      //     precision_t sleeptime = periodic_ - elapsed;
      //     std::this_thread::sleep_for(std::chrono::duration<precision_t>(sleeptime));
      // }
    }
    return EXIT_SUCCESS;
  }

private:
  GLFWwindow *window_ = nullptr;
  int width_;
  int height_;

  static void InitGlew() {

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    const GLenum err = glewInit();
    if (err == GLEW_OK) {
      std::cout << "GLEW_OK: Glew Ver." << glewGetString(GLEW_VERSION)
                << std::endl;
    } else {
      std::cerr << "GLEW Error:" << glewGetErrorString(err) << std::endl;
    }
    if (!GLEW_ARB_vertex_array_object) {
      std::cerr << "ARB_vertex_array_object not avaliable" << std::endl;
    }
  }
};

#endif // APP_HPP
