/**
 * @brief スクリーンスペースアンビエントオクルージョン */

#ifndef SSAO_H
#define SSAO_H

#include <random>
#include <vector>

#include "Math/UniformDistribution.h"

class SSAO {
public:
  SSAO() {
    std::random_device rd;
    *this = SSAO(rd());
  }
  explicit SSAO(std::uint32_t seed) : engine_(seed) {}

  std::vector<float> BuildKernel(std::size_t kernelSize);
  std::vector<float> BuildRandRot(std::size_t rotTexSize);

private:
  std::mt19937 engine_;
  UniformDistribution dist_;
};

#endif
