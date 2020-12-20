/**
 * @brief  Shader Relations
 * @date   2020/12/20
 */

//*--------------------------------------------------------------------------------
// Including files
//*--------------------------------------------------------------------------------

#include "Graphics/Shader.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <utility>

#include <boost/noncopyable.hpp>


//*--------------------------------------------------------------------------------
// Special member functions
//*--------------------------------------------------------------------------------

ShaderProgram::~ShaderProgram() {
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

bool ShaderProgram::Compile(const std::string& filepath, ShaderType type) {

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

  return CompileShader(code.str(), type);
}

bool ShaderProgram::Link() {

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

bool ShaderProgram::CompileShader(const std::string &src, ShaderType type) {
  GLuint handle = CreateShader(type);

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

GLuint ShaderProgram::CreateShader(ShaderType type) const {
  switch (type) {
  case ShaderType::Vertex:
    return glCreateShader(GL_VERTEX_SHADER);
  case ShaderType::Fragment:
    return glCreateShader(GL_FRAGMENT_SHADER);
  case ShaderType::Geometry:
    return glCreateShader(GL_GEOMETRY_SHADER);
  case ShaderType::TessControl:
    return glCreateShader(GL_TESS_CONTROL_SHADER);
  case ShaderType::TessEvaluation:
    return glCreateShader(GL_TESS_EVALUATION_SHADER);
  case ShaderType::Compute:
    return glCreateShader(GL_COMPUTE_SHADER);
  default:
    return 0;
  }
  return 0;
}

//*--------------------------------------------------------------------------------
// Use Shader Program
//*--------------------------------------------------------------------------------

void ShaderProgram::Use() const {
  if (handle_ > 0 && isLinked_) {
    glUseProgram(handle_);
  }
}

//*--------------------------------------------------------------------------------
// Logging
//*--------------------------------------------------------------------------------

void ShaderProgram::StoreLog(GLuint handle) {
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
// Utility
//*--------------------------------------------------------------------------------

std::optional<std::string> ShaderProgram::CompileAndLink(
  const std::vector<std::pair<std::string, ShaderType>>& shaders) {
  for (const auto &info : shaders) {
    if (!Compile(info.first, info.second)) {
      return std::make_optional(log_);
    }
  }
  return Link() ? std::nullopt : std::make_optional(log_);
}
