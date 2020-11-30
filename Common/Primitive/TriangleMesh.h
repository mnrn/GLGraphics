#pragma once

#include "GLInclude.h"
#include <optional>
#include <vector>

#include "Drawable.h"

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
  virtual void InitBuffers(
      const std::vector<GLuint> &indices, const std::vector<GLfloat> &points,
      const std::vector<GLfloat> &normals,
      const std::optional<std::vector<GLfloat>> &texCoords = std::nullopt,
      const std::optional<std::vector<GLfloat>> &tangents = std::nullopt);
  virtual void DestroyBuffers();

  GLuint vao_;                  // 頂点配列オブジェクト
  GLuint nVerts_;               // 頂点数
  std::vector<GLuint> buffers_; // 頂点バッファ
};
