#pragma once
#include <array>
#include <glm/glm.hpp>
#include <vector>

namespace mahou {
using Triangle = std::array<glm::vec3, 3>;

using TriangleSoup = std::vector<glm::vec3>;

struct PointOnMesh {
  int triangle;
  float u, v;
};
} // namespace mahou