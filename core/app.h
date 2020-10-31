/**
 * @brief アプリに関するクラスを扱います。
 */

#ifndef APP_H
#define APP_H

// ********************************************************************************
// Include files
// ********************************************************************************

#include <boost/noncopyable.hpp>

#include "common.h"
#include "gl_include.h"

#include "debug.h"
#include "window.h"

// ********************************************************************************
// Class
// ********************************************************************************

class App : private boost::noncopyable {
public:
  App(const char *appName) {
    glfwInit();

    window_ = window::create(1280, 720, appName);

    initGlew();
    debug::setupInfo();
  }

  ~App() {

    window::destroy(window_);

    glfwTerminate();
  }

  template <typename F> void run(F callback) {
    window::loop(window_, callback);
  }

private:
  GLFWwindow *window_ = nullptr;

  static void initGlew() {

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
