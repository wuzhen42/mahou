#pragma once
#include "intersect.h"
#include "types.hpp"
#include <array>
#include <vector>

namespace mahou {
class Octree {
  struct Node {
    explicit Node(AABB bbox) : bbox(bbox) { children.fill(0); }

    std::array<unsigned, 8> children;
    AABB bbox;
    std::vector<unsigned> content;
  };
  std::vector<Node> nodes;

public:
  static Octree build(const TriangleSoup &triangles);

  std::vector<unsigned> intersections(Ray ray) const;
};

} // namespace mahou
