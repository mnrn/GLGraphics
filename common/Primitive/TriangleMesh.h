#pragma once

#include "GLInclude.h"
#include <optional>
#include <vector>

#include "Primitive/Drawable.h"

class TriangleMesh : public Drawable {
public:
  virtual ~TriangleMesh() override;
  virtual void Render() const override;
  GLuint GetVAO() const { return vao_; }
  GLuint GetElementBuffer() const { return buffers_[0]; }
  GLuint GetPositionBuffer() const { return buffers_[1]; }
  GLuint GetNormalBuffer() const { return buffers_[2]; }
  GLuint GetTcBuffer() const { return buffers_.size() > 3 ? buffers_[3] : 0; }
  GLuint GetNumVers() const { return nVerts_; }

protected:
  virtual void InitBuffers(const std::vector<GLuint> &indices,
                           const std::vector<GLfloat> &points,
                           const std::vector<GLfloat> &normals,
                           const std::optional<std::vector<GLfloat>> &texCoords,
                           const std::optional<std::vector<GLfloat>> &tangents);
  virtual void DestroyBuffers();

  GLuint vao_;                  // The vertex array object;
  GLuint nVerts_;               // 頂点数
  std::vector<GLuint> buffers_; // Vertex buffers;
};