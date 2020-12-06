#include "TriangleMesh.h"

#include <iostream>

TriangleMesh::~TriangleMesh() { DestroyBuffers(); }

void TriangleMesh::InitBuffers(
    const std::vector<GLuint> &indices, const std::vector<GLfloat> &points,
    const std::optional<std::vector<GLfloat>> &normals,
    const std::optional<std::vector<GLfloat>> &texCoords,
    const std::optional<std::vector<GLfloat>> &tangents) {
  if (!buffers_.empty()) {
    DestroyBuffers();
  }

  nVerts_ = static_cast<GLuint>(indices.size());
  GLuint indexBuf = 0, posBuf = 0, normBuf = 0, tcBuf = 0, tangentBuf = 0;

  glGenBuffers(1, &indexBuf);
  buffers_.emplace_back(indexBuf);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuf);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint),
               indices.data(), GL_STATIC_DRAW);

  glGenBuffers(1, &posBuf);
  buffers_.emplace_back(posBuf);
  glBindBuffer(GL_ARRAY_BUFFER, posBuf);
  glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(GLfloat), points.data(),
               GL_STATIC_DRAW);

  if (normals) {
    glGenBuffers(1, &normBuf);
    buffers_.emplace_back(normBuf);
    glBindBuffer(GL_ARRAY_BUFFER, normBuf);
    glBufferData(GL_ARRAY_BUFFER, normals.value().size() * sizeof(GLfloat),
                 normals.value().data(), GL_STATIC_DRAW);
  }


  if (texCoords) {
    glGenBuffers(1, &tcBuf);
    buffers_.emplace_back(tcBuf);
    glBindBuffer(GL_ARRAY_BUFFER, tcBuf);
    glBufferData(GL_ARRAY_BUFFER, texCoords.value().size() * sizeof(GLfloat),
                 texCoords.value().data(), GL_STATIC_DRAW);
  }

  if (tangents) {
    glGenBuffers(1, &tangentBuf);
    buffers_.emplace_back(tangentBuf);
    glBindBuffer(GL_ARRAY_BUFFER, tangentBuf);
    glBufferData(GL_ARRAY_BUFFER, tangents.value().size() * sizeof(GLfloat),
                 tangents.value().data(), GL_STATIC_DRAW);
  }

  glGenVertexArrays(1, &vao_);
  glBindVertexArray(vao_);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuf);

  // Position
  glBindBuffer(GL_ARRAY_BUFFER, posBuf);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(0); // Vertex position

  // Normal
  if (normals) {
    glBindBuffer(GL_ARRAY_BUFFER, normBuf);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1); // Normal
  }

  // Tex Coord
  if (texCoords) {
    glBindBuffer(GL_ARRAY_BUFFER, tcBuf);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2); // Tex Coord
  }

  // Tangent
  if (tangents) {
    glBindBuffer(GL_ARRAY_BUFFER, tangentBuf);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(3); // Tangents
  }

  glBindVertexArray(0);
}

void TriangleMesh::DestroyBuffers() {
  if (!buffers_.empty()) {
    glDeleteBuffers(static_cast<GLsizei>(buffers_.size()), buffers_.data());
    buffers_.clear();
  }

  if (vao_ != 0) {
    glDeleteVertexArrays(1, &vao_);
    vao_ = 0;
  }
}

void TriangleMesh::Render() const {
  if (vao_ == 0) {
    return;
  }
  glBindVertexArray(vao_);
  glDrawElements(GL_TRIANGLES, nVerts_, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}
