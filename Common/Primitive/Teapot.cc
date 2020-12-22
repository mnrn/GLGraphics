#include "Teapot.h"
#include "TeapotData.h"

#include <glm/gtc/matrix_transform.hpp>
using glm::mat3;
using glm::mat4;
using glm::vec3;
using glm::vec4;

Teapot::Teapot(int grid, const mat4 &lidTransform) {
  const int verts = 32 * (grid + 1) * (grid + 1);
  const int faces = grid * grid * 32;
  std::vector<GLfloat> p(verts * 3);
  std::vector<GLfloat> n(verts * 3);
  std::vector<GLfloat> tc(verts * 2);
  std::vector<GLuint> el(faces * 6);

  GeneratePatches(p, n, tc, el, grid);
  MoveLid(grid, p, lidTransform);

  InitBuffers(el, p, n, tc);
}

void Teapot::GeneratePatches(std::vector<GLfloat> &p, std::vector<GLfloat> &n,
                             std::vector<GLfloat> &tc, std::vector<GLuint> &el,
                             int grid) {
  std::vector<GLfloat> B(4 *
                         (grid + 1)); // Pre-computed Bernstein basis functions
  std::vector<GLfloat> dB(
      4 * (grid + 1)); // Pre-computed derivitives of basis functions

  int idx = 0, elIndex = 0, tcIndex = 0;

  // Pre-compute the basis functions  (Bernstein polynomials)
  // and their derivatives
  ComputeBasisFunctions(B, dB, grid);

  // Build each patch
  // The rim
  BuildPatchReflect(0, B, dB, p, n, tc, el, idx, elIndex, tcIndex, grid, true,
                    true);
  // The body
  BuildPatchReflect(1, B, dB, p, n, tc, el, idx, elIndex, tcIndex, grid, true,
                    true);
  BuildPatchReflect(2, B, dB, p, n, tc, el, idx, elIndex, tcIndex, grid, true,
                    true);
  // The lid
  BuildPatchReflect(3, B, dB, p, n, tc, el, idx, elIndex, tcIndex, grid, true,
                    true);
  BuildPatchReflect(4, B, dB, p, n, tc, el, idx, elIndex, tcIndex, grid, true,
                    true);
  // The bottom
  BuildPatchReflect(5, B, dB, p, n, tc, el, idx, elIndex, tcIndex, grid, true,
                    true);
  // The handle
  BuildPatchReflect(6, B, dB, p, n, tc, el, idx, elIndex, tcIndex, grid, false,
                    true);
  BuildPatchReflect(7, B, dB, p, n, tc, el, idx, elIndex, tcIndex, grid, false,
                    true);
  // The spout
  BuildPatchReflect(8, B, dB, p, n, tc, el, idx, elIndex, tcIndex, grid, false,
                    true);
  BuildPatchReflect(9, B, dB, p, n, tc, el, idx, elIndex, tcIndex, grid, false,
                    true);
}

void Teapot::MoveLid(int grid, std::vector<GLfloat> &p,
                     const mat4 &lidTransform) {
  const int start = 3 * 12 * (grid + 1) * (grid + 1);
  const int end = 3 * 20 * (grid + 1) * (grid + 1);

  for (int i = start; i < end; i += 3) {
    vec4 vert = vec4(p[i], p[i + 1], p[i + 2], 1.0f);
    vert = lidTransform * vert;
    p[i] = vert.x;
    p[i + 1] = vert.y;
    p[i + 2] = vert.z;
  }
}

void Teapot::BuildPatchReflect(int patchNum, std::vector<GLfloat> &B,
                               std::vector<GLfloat> &dB,
                               std::vector<GLfloat> &v, std::vector<GLfloat> &n,
                               std::vector<GLfloat> &tc,
                               std::vector<GLuint> &el, int &index,
                               int &elIndex, int &tcIndex, int grid,
                               bool reflectX, bool reflectY) {
  vec3 patch[4][4];
  vec3 patchRevV[4][4];
  GetPatch(patchNum, patch, false);
  GetPatch(patchNum, patchRevV, true);

  // Patch without modification
  BuildPatch(patch, B, dB, v, n, tc, el, index, elIndex, tcIndex, grid,
             mat3(1.0f), true);

  // Patch reflected in x
  if (reflectX) {
    BuildPatch(patchRevV, B, dB, v, n, tc, el, index, elIndex, tcIndex, grid,
               mat3(vec3(-1.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f),
                    vec3(0.0f, 0.0f, 1.0f)),
               false);
  }

  // Patch reflected in y
  if (reflectY) {
    BuildPatch(patchRevV, B, dB, v, n, tc, el, index, elIndex, tcIndex, grid,
               mat3(vec3(1.0f, 0.0f, 0.0f), vec3(0.0f, -1.0f, 0.0f),
                    vec3(0.0f, 0.0f, 1.0f)),
               false);
  }

  // Patch reflected in x and y
  if (reflectX && reflectY) {
    BuildPatch(patch, B, dB, v, n, tc, el, index, elIndex, tcIndex, grid,
               mat3(vec3(-1.0f, 0.0f, 0.0f), vec3(0.0f, -1.0f, 0.0f),
                    vec3(0.0f, 0.0f, 1.0f)),
               true);
  }
}

void Teapot::BuildPatch(vec3 patch[][4], std::vector<GLfloat> &B,
                        std::vector<GLfloat> &dB, std::vector<GLfloat> &v,
                        std::vector<GLfloat> &n, std::vector<GLfloat> &tc,
                        std::vector<GLuint> &el, int &index, int &elIndex,
                        int &tcIndex, int grid, mat3 reflect,
                        bool invertNormal) {
  const int startIndex = index / 3;
  const float tcFactor = 1.0f / static_cast<float>(grid);

  for (int i = 0; i <= grid; i++) {
    for (int j = 0; j <= grid; j++) {
      const vec3 pt = reflect * Evaluate(i, j, B, patch);
      vec3 norm = reflect * EvaluateNormal(i, j, B, dB, patch);
      if (invertNormal)
        norm = -norm;

      v[index] = pt.x;
      v[index + 1] = pt.y;
      v[index + 2] = pt.z;

      n[index] = norm.x;
      n[index + 1] = norm.y;
      n[index + 2] = norm.z;

      tc[tcIndex] = static_cast<float>(i) * tcFactor;
      tc[tcIndex + 1] = static_cast<float>(j) * tcFactor;

      index += 3;
      tcIndex += 2;
    }
  }

  for (int i = 0; i < grid; i++) {
    const int iStart = i * (grid + 1) + startIndex;
    const int nextiStart = (i + 1) * (grid + 1) + startIndex;
    for (int j = 0; j < grid; j++) {
      el[elIndex] = iStart + j;
      el[elIndex + 1] = nextiStart + j + 1;
      el[elIndex + 2] = nextiStart + j;

      el[elIndex + 3] = iStart + j;
      el[elIndex + 4] = iStart + j + 1;
      el[elIndex + 5] = nextiStart + j + 1;

      elIndex += 6;
    }
  }
}

void Teapot::GetPatch(int patchNum, vec3 patch[][4], bool reverseV) {
  for (int u = 0; u < 4; u++) {   // Loop in u direction
    for (int v = 0; v < 4; v++) { // Loop in v direction
      if (reverseV) {
        patch[u][v] = vec3(
            TeapotData::cpdata[TeapotData::patchdata[patchNum][u * 4 + (3 - v)]]
                              [0],
            TeapotData::cpdata[TeapotData::patchdata[patchNum][u * 4 + (3 - v)]]
                              [1],
            TeapotData::cpdata[TeapotData::patchdata[patchNum][u * 4 + (3 - v)]]
                              [2]);
      } else {
        patch[u][v] = vec3(
            TeapotData::cpdata[TeapotData::patchdata[patchNum][u * 4 + v]][0],
            TeapotData::cpdata[TeapotData::patchdata[patchNum][u * 4 + v]][1],
            TeapotData::cpdata[TeapotData::patchdata[patchNum][u * 4 + v]][2]);
      }
    }
  }
}

void Teapot::ComputeBasisFunctions(std::vector<GLfloat> &B,
                                   std::vector<GLfloat> &dB, int grid) {
  const float inc = 1.0f / static_cast<float>(grid);
  for (int i = 0; i <= grid; i++) {
    const float t = static_cast<float>(i) * inc;
    const float tSqr = t * t;
    const float oneMinusT = (1.0f - t);
    const float oneMinusT2 = oneMinusT * oneMinusT;

    B[i * 4 + 0] = oneMinusT * oneMinusT2;
    B[i * 4 + 1] = 3.0f * oneMinusT2 * t;
    B[i * 4 + 2] = 3.0f * oneMinusT * tSqr;
    B[i * 4 + 3] = t * tSqr;

    dB[i * 4 + 0] = -3.0f * oneMinusT2;
    dB[i * 4 + 1] = -6.0f * t * oneMinusT + 3.0f * oneMinusT2;
    dB[i * 4 + 2] = -3.0f * tSqr + 6.0f * t * oneMinusT;
    dB[i * 4 + 3] = 3.0f * tSqr;
  }
}

vec3 Teapot::Evaluate(int gridU, int gridV, std::vector<GLfloat> &B,
                      vec3 patch[][4]) {
  vec3 p(0.0f, 0.0f, 0.0f);
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      p += patch[i][j] * B[gridU * 4 + i] * B[gridV * 4 + j];
    }
  }
  return p;
}

vec3 Teapot::EvaluateNormal(int gridU, int gridV, std::vector<GLfloat> &B,
                            std::vector<GLfloat> &dB, vec3 patch[][4]) {
  vec3 du(0.0f, 0.0f, 0.0f);
  vec3 dv(0.0f, 0.0f, 0.0f);

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      du += patch[i][j] * dB[gridU * 4 + i] * B[gridV * 4 + j];
      dv += patch[i][j] * B[gridU * 4 + i] * dB[gridV * 4 + j];
    }
  }

  vec3 norm = glm::cross(du, dv);
  if (glm::length(norm) != 0.0f) {
    norm = glm::normalize(norm);
  }

  return norm;
}
