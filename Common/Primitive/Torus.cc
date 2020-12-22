#include "Torus.h"

#include <glm/gtc/constants.hpp>

Torus::Torus(GLfloat outerRadius, GLfloat innerRadius, GLuint nsides,
             GLuint nrings) {
  const GLuint faces = nsides * nrings;
  const int nVerts =
      nsides * (nrings + 1); // One extra ring to duplicate first ring

  // Points
  std::vector<GLfloat> p(3 * nVerts);
  // Normals
  std::vector<GLfloat> n(3 * nVerts);
  // Tex coords
  std::vector<GLfloat> tex(2 * nVerts);
  // Elements
  std::vector<GLuint> el(6 * faces);

  // Generate the vertex data
  const float ringFactor = glm::two_pi<float>() / static_cast<float>(nrings);
  const float sideFactor = glm::two_pi<float>() / static_cast<float>(nsides);
  int idx = 0, tidx = 0;
  for (GLuint ring = 0; ring <= nrings; ring++) {
    const float u = static_cast<float>(ring) * ringFactor;
    const float cu = cos(u);
    const float su = sin(u);
    for (GLuint side = 0; side < nsides; side++) {
      const float v = static_cast<float>(side) * sideFactor;
      const float cv = cos(v);
      const float sv = sin(v);
      const float r = (outerRadius + innerRadius * cv);
      p[idx] = r * cu;
      p[idx + 1] = r * su;
      p[idx + 2] = innerRadius * sv;
      n[idx] = cv * cu * r;
      n[idx + 1] = cv * su * r;
      n[idx + 2] = sv * r;
      tex[tidx] = u / glm::two_pi<float>();
      tex[tidx + 1] = v / glm::two_pi<float>();
      tidx += 2;
      // Normalize
      float len = sqrt(n[idx] * n[idx] + n[idx + 1] * n[idx + 1] +
                       n[idx + 2] * n[idx + 2]);
      n[idx] /= len;
      n[idx + 1] /= len;
      n[idx + 2] /= len;
      idx += 3;
    }
  }

  idx = 0;
  for (GLuint ring = 0; ring < nrings; ring++) {
    const GLuint ringStart = ring * nsides;
    const GLuint nextRingStart = (ring + 1) * nsides;
    for (GLuint side = 0; side < nsides; side++) {
      const int nextSide = (side + 1) % nsides;
      // The quad
      el[idx] = (ringStart + side);
      el[idx + 1] = (nextRingStart + side);
      el[idx + 2] = (nextRingStart + nextSide);
      el[idx + 3] = ringStart + side;
      el[idx + 4] = nextRingStart + nextSide;
      el[idx + 5] = (ringStart + nextSide);
      idx += 6;
    }
  }

  InitBuffers(el, p, n, tex);
}
