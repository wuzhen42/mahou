#pragma once
#include "intersect.h"
#include "mesh.h"
#include <array>
#include <vector>

namespace mahou {
class Octree {
  struct Node {
    explicit Node(AABB bbox) : bbox(bbox) { children.fill(0); }

    std::array<int, 8> children;
    AABB bbox;
    std::vector<int> content;
  };
  std::vector<Node> nodes;

  std::vector<int> all_potential_triangles(Ray ray) const;

public:
  static Octree build(const Mesh &mesh);

  std::optional<std::pair<float, PointOnMesh>> closest(Ray ray, const Mesh &mesh) const;
};

} // namespace mahou
