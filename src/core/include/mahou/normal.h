#pragma once
#include "mesh.h"

namespace mahou {
class PolygonNormal {
  std::vector<glm::vec3> normal;

  explicit PolygonNormal(std::vector<glm::vec3> normal) : normal(normal) {}

public:
  glm::vec3 at(unsigned face) const { return normal[face]; }

  static PolygonNormal build(const Mesh &mesh);
};

class TriangleNormal {
  std::vector<glm::vec3> normal;

  explicit TriangleNormal(std::vector<glm::vec3> normal) : normal(normal) {}

public:
  glm::vec3 at(unsigned tri) const { return normal[tri]; }

  static TriangleNormal build(Mesh &mesh);
};

class VertexNormal {
  std::vector<glm::vec3> normal;

  explicit VertexNormal(std::vector<glm::vec3> normal) : normal(normal) {}

public:
  glm::vec3 at(unsigned vtx) const { return normal[vtx]; }

  static VertexNormal build(Mesh &mesh, bool angleWeighted = true);
};

} // namespace mahou