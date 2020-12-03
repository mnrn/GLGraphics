/**
 * @brief  Shader Relations
 * @date   2017/03/18
 */

// ********************************************************************************
// Include guard
// ********************************************************************************

#ifndef SHADER_HH
#define SHADER_HH

// ********************************************************************************
// Including file
// ********************************************************************************

#include "GLInclude.h"

#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <utility>

#ifdef __APPLE__
#include <boost/filesystem.hpp>
#else
#include <filesystem>
#endif
#include <boost/noncopyable.hpp>

// ********************************************************************************
// Class(es)
// ********************************************************************************

/**
 * @brief Shader type
 */
enum class ShaderType {
  Vertex,
  Fragment,
  Geometry,
  TessControl,
  TessEvaluation,
#ifndef __APPLE__
  Compute,
#endif
};

/**
 * @brief GLSL Shader Program Class
 */
class ShaderProgram : private boost::noncopyable {
public:
  ~ShaderProgram() {
    if (handle_ == 0) {
      return;
    }

    // Query the number of attached shaders
    GLint num = 0;
    glGetProgramiv(handle_, GL_ATTACHED_SHADERS, &num);

    // Get the shader names
    GLuint *names = new GLuint[static_cast<std::size_t>(num)];
    glGetAttachedShaders(handle_, num, nullptr, names);

    // Delete
    for (GLint i = 0; i < num; i++) {
      glDeleteShader(names[i]);
    }
    delete[] names;
    glDeleteProgram(handle_);
  }

  //*--------------------------------------------------------------------------------
  // Compile & Link
  //*--------------------------------------------------------------------------------

  bool Compile(const char *filepath, ShaderType type) {

    if (!IsFileExists(filepath)) {
      log_ = std::string("File Not Found : ") + filepath;
      return false;
    }

    if (handle_ <= 0) {

      handle_ = glCreateProgram();
      if (handle_ == 0) {
        log_ = "Unable to create shader program.";
        return false;
      }
    }

    std::ifstream infile(filepath, std::ios::in);
    if (!infile) {
      log_ = std::string("Can't open file : ") + filepath;
      return false;
    }

    std::stringstream code;
    code << infile.rdbuf();
    infile.close();

    return Compile(code.str(), type);
  }

  bool Link() {

    if (isLinked_) {
      return true;
    }
    if (handle_ == 0) {
      return false;
    }

    glLinkProgram(handle_);

    // Check for errors
    int status = GL_FALSE;
    glGetProgramiv(handle_, GL_LINK_STATUS, &status);
    if (GL_FALSE == status) {
      StoreLog(handle_);
      return false;
    } else {
      isLinked_ = true;
      return isLinked_;
    }
  }

  //*--------------------------------------------------------------------------------
  // Use Shader Program
  //*--------------------------------------------------------------------------------

  void Use() const {
    if (handle_ > 0 && isLinked_) {
      glUseProgram(handle_);
    }
  }

  //*--------------------------------------------------------------------------------
  // Bind Location
  //*--------------------------------------------------------------------------------

  void BindAttribLocation(GLuint location, const char *name) const {
    glBindAttribLocation(handle_, location, name);
  }

  void BindFragDataLocation(GLuint location, const char *name) const {
    glBindFragDataLocation(handle_, location, name);
  }

  //*--------------------------------------------------------------------------------
  // Accessor
  //*--------------------------------------------------------------------------------

  const std::string &GetLog() const { return log_; }
  GLuint GetHandle() const { return handle_; }
  bool IsLinked() const { return isLinked_; }

  //*--------------------------------------------------------------------------------
  // Setting Uniform Variable(s)
  //*--------------------------------------------------------------------------------

  void SetUniform(const char *name, float x, float y, float z) const {
    SetUniform(name, glUniform3f, x, y, z);
  }
  void SetUniform(const char *name, const glm::vec3 &v) const {
    SetUniform(name, v.x, v.y, v.z);
  }
  void SetUniform(const char *name, const glm::vec4 &v) const {
    SetUniform(name, glUniform4f, v.x, v.y, v.z, v.w);
  }
  void SetUniform(const char *name, const glm::mat3 &m) const {
    SetUniform(name, glUniformMatrix3fv, 1,
               static_cast<unsigned char>(GL_FALSE), std::addressof(m[0][0]));
  }
  void SetUniform(const char *name, const glm::mat4 &m) const {
    SetUniform(name, glUniformMatrix4fv, 1,
               static_cast<unsigned char>(GL_FALSE), std::addressof(m[0][0]));
  }
  void SetUniform(const char *name, float f) const {
    SetUniform(name, glUniform1f, f);
  }
  void SetUniform(const char *name, int i) const {
    SetUniform(name, glUniform1i, i);
  }
  void SetUniform(const char *name, bool b) const {
    SetUniform(name, static_cast<int>(b));
  }

private:
  //*--------------------------------------------------------------------------------
  // Private functions
  //*--------------------------------------------------------------------------------

  template <typename F, typename... Args>
  void SetUniform(const char *name, F f, Args &&... args) const {
    auto location = GetUniformLocation(name);
    if (location >= 0) {
      f(location, std::forward<Args>(args)...);
    }
  }

  int GetUniformLocation(const char *name) const {
    return glGetUniformLocation(handle_, name);
  }

  bool IsFileExists(const char *filepath) const {
#ifdef __APPLE__
    boost::system::error_code error;
    const bool result = boost::filesystem::exists(filepath, error);
    return !error && result;
#else
    std::error_code ec;
    const bool result = std::filesystem::exists(filepath, ec);
    return !ec && result;
#endif
  }

  bool Compile(const std::string &src, ShaderType type) {
    GLuint handle = 0;
    switch (type) {
    case ShaderType::Vertex:
      handle = glCreateShader(GL_VERTEX_SHADER);
      break;
    case ShaderType::Fragment:
      handle = glCreateShader(GL_FRAGMENT_SHADER);
      break;
    case ShaderType::Geometry:
      handle = glCreateShader(GL_GEOMETRY_SHADER);
      break;
    case ShaderType::TessControl:
      handle = glCreateShader(GL_TESS_CONTROL_SHADER);
      break;
    case ShaderType::TessEvaluation:
      handle = glCreateShader(GL_TESS_EVALUATION_SHADER);
      break;
#ifndef __APPLE__
    case ShaderType::Compute:
      handle = glCreateShader(GL_COMPUTE_SHADER);
      break;
#endif
    default:
      return false;
    }

    const char *code = src.c_str();
    glShaderSource(handle, 1, std::addressof(code), nullptr);
    glCompileShader(handle);

    // Check for errors
    int res = GL_FALSE;
    glGetShaderiv(handle, GL_COMPILE_STATUS, &res);
    if (GL_FALSE == res) {
      StoreLog(handle);
      return false;
    } else {
      glAttachShader(handle_, handle);
      return true;
    }
  }

  void StoreLog(GLuint handle) {
    int length = 0;
    log_ = "";

    glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &length);
    if (length <= 0) {
      return;
    }

    std::string log(length, ' ');
    int written = 0;
    glGetProgramInfoLog(handle, length, &written, &log[0]);
    log_ = log;
  }

  //*--------------------------------------------------------------------------------
  // Member Variable(s)
  //*--------------------------------------------------------------------------------

  GLuint handle_ = 0;
  bool isLinked_ = false;
  std::string log_;
};

#endif
