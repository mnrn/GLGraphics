#include "ObjMesh.h"

#include <iostream>
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader/tiny_obj_loader.h>

// 法線情報の生成(TODO)
static void ComputeNormal(const tinyobj::attrib_t& attrib,
  const std::vector<tinyobj::shape_t>& shapes,
  std::vector<GLfloat>& normal) {

}

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
  if (attrib.normals.empty()) {
    ComputeNormal(attrib, shapes, normals);
  }
  
  const auto optTexCoords =
      texCoords.empty() ? std::nullopt : std::make_optional(texCoords);
  InitBuffers(indices, positions, normals, optTexCoords);
}
