#include "ObjMesh.h"

#include <iostream>
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader/tiny_obj_loader.h>

namespace Impl {

/**
 * @brief 法線情報に関する実装
 * @ref https://github.com/tinyobjloader/tinyobjloader/blob/master/examples/viewer/viewer.cc
 */
namespace Normal {
static void Calc(float N[3], float v0[3], float v1[3], float v2[3]) {
  float v10[3];
  v10[0] = v1[0] - v0[0];
  v10[1] = v1[1] - v0[1];
  v10[2] = v1[2] - v0[2];

  float v20[3];
  v20[0] = v2[0] - v0[0];
  v20[1] = v2[1] - v0[1];
  v20[2] = v2[2] - v0[2];

  N[0] = v10[1] * v20[2] - v10[2] * v20[1];
  N[1] = v10[2] * v20[0] - v10[0] * v20[2];
  N[2] = v10[0] * v20[1] - v10[1] * v20[0];

  float len2 = N[0] * N[0] + N[1] * N[1] + N[2] * N[2];
  if (len2 > 0.0f) {
    float len = sqrtf(len2);

    N[0] /= len;
    N[1] /= len;
    N[2] /= len;
  }
}

static void Compute(const tinyobj::attrib_t &attrib,
                          const std::vector<tinyobj::shape_t> &shapes,
                          std::vector<GLfloat> &normals) {
  // Shapeの数だけループする。
  const size_t ssize = shapes.size();
  for (size_t s = 0; s < ssize; s++) {

    // 面f(ポリゴン)の数だけループする。
    for (size_t f = 0; f < shapes[s].mesh.indices.size() / 3; f++) {
      const tinyobj::index_t idx0 = shapes[s].mesh.indices[3 * f + 0];
      const tinyobj::index_t idx1 = shapes[s].mesh.indices[3 * f + 1];
      const tinyobj::index_t idx2 = shapes[s].mesh.indices[3 * f + 2];

      // 面fを構成する頂点の数だけループする。
      float v[3][3];
      for (int k = 0; k < 3; k++) {
        const int f0 = idx0.vertex_index;
        const int f1 = idx1.vertex_index;
        const int f2 = idx2.vertex_index;
        assert(f0 >= 0);
        assert(f1 >= 0);
        assert(f2 >= 0);

        v[0][k] = attrib.vertices[3 * f0 + k];
        v[1][k] = attrib.vertices[3 * f1 + k];
        v[2][k] = attrib.vertices[3 * f2 + k];
      }
      float n[3][3];
      Calc(n[0], v[0], v[1], v[2]);
      n[1][0] = n[0][0];
      n[1][1] = n[0][1];
      n[1][2] = n[0][2];
      n[2][0] = n[0][0];
      n[2][1] = n[0][1];
      n[2][2] = n[0][2];
      for (int k = 0; k < 3; k++) {
        normals.emplace_back(n[k][0]);
        normals.emplace_back(n[k][1]);
        normals.emplace_back(n[k][2]);
      }
    }
  }
}
} // namespace Normal
} // namespace Impl

ObjMesh::ObjMesh(const std::string &path) {
  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;
  std::string warn, err;

  const bool isSuccess =
      tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str());
  if (!warn.empty()) {
    std::cout << warn << std::endl;
  }
  if (!err.empty()) {
    std::cerr << err << std::endl;
  }
  if (!isSuccess) {
    assert(false);
    return;
  }

  std::vector<GLuint> indices;
  std::vector<GLfloat> positions;
  std::vector<GLfloat> normals;
  std::vector<GLfloat> texCoords;

  // Shapeの数だけループする。
  const size_t ssize = shapes.size();
  for (size_t s = 0; s < ssize; s++) {

    // 面f(ポリゴン)の数だけループする。
    const size_t fsize = shapes[s].mesh.num_face_vertices.size();
    size_t indexOffset = 0;
    for (size_t f = 0; f < fsize; f++) {

      // 面fを構成する頂点の数だけループする。
      const size_t fv = shapes[s].mesh.num_face_vertices[f];
      for (size_t v = 0; v < fv; v++) {
        const tinyobj::index_t idx = shapes[s].mesh.indices[indexOffset + v];
        indices.emplace_back(indexOffset + v);

        const tinyobj::real_t vx = attrib.vertices[3 * idx.vertex_index + 0];
        const tinyobj::real_t vy = attrib.vertices[3 * idx.vertex_index + 1];
        const tinyobj::real_t vz = attrib.vertices[3 * idx.vertex_index + 2];
        positions.emplace_back(vx);
        positions.emplace_back(vy);
        positions.emplace_back(vz);

        if (!attrib.normals.empty()) {
          const tinyobj::real_t nx = attrib.normals[3 * idx.normal_index + 0];
          const tinyobj::real_t ny = attrib.normals[3 * idx.normal_index + 1];
          const tinyobj::real_t nz = attrib.normals[3 * idx.normal_index + 2];
          normals.emplace_back(nx);
          normals.emplace_back(ny);
          normals.emplace_back(nz);
        }

        if (!attrib.texcoords.empty()) {
          const tinyobj::real_t tx =
              attrib.texcoords[2 * idx.texcoord_index + 0];
          const tinyobj::real_t ty =
              attrib.texcoords[2 * idx.texcoord_index + 1];
          texCoords.emplace_back(tx);
          texCoords.emplace_back(ty);
        }
      }
      indexOffset += fv;
    }
  }

  // 法線情報が取得できなかった場合は生成します。
  // もう一度ループを回すようにしてあるので時間かかるかもしれません。
  if (attrib.normals.empty()) {
    Impl::Normal::Compute(attrib, shapes, normals);
  }
  
  const auto optTexCoords =
      texCoords.empty() ? std::nullopt : std::make_optional(texCoords);
  InitBuffers(indices, positions, normals, optTexCoords);
}
