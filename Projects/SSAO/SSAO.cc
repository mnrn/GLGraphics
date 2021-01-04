/**
 * @brief スクリーンスペースアンビエントオクルージョン
 */

#include "SSAO.h"

#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/integer.hpp>
#include <iostream>

// ********************************************************************************
// Sampling for SSAO
// ********************************************************************************

std::vector<float> SSAO::BuildKernel(std::size_t kernelSize) {
  std::vector<float> kern(3 * kernelSize);
  for (size_t i = 0; i < kernelSize; i++) {
    glm::vec3 randDir = dist_.OnHemisphere(engine_);
    const float kScale = static_cast<float>(i * i) / (kernelSize * kernelSize);
    randDir *= glm::mix(0.1f, 1.0f, kScale);

    kern[3 * i + 0] = randDir.x;
    kern[3 * i + 1] = randDir.y;
    kern[3 * i + 2] = randDir.z;
  }
  return kern;
}

std::vector<float> SSAO::BuildRandRot(std::size_t rotTexSize) {
  std::vector<float> randDir(3 * rotTexSize * rotTexSize);
  for (size_t i = 0; i < rotTexSize * rotTexSize; i++) {
    glm::vec2 v = dist_.OnCircle(engine_);
    randDir[3 * i + 0] = v.x;
    randDir[3 * i + 1] = v.y;
    randDir[3 * i + 2] = 0.0f;
  }
  return randDir;
}
