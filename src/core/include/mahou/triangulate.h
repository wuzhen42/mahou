#pragma once
#include "mesh.h"
#include "types.hpp"
#include <array>
#include <vector>

namespace mahou {
class Triangulation {
  std::vector<std::array<int, 3>> triangles;
  std::vector<int> face_of_triangle;

  void push(int v0, int v1, int v2, int face);

public:
  static Triangulation build(const Mesh &mesh);

  TriangleSoup make_soup(const Mesh &mesh) const;

  int vtx(int tri, int id) const { return triangles[tri][id]; }
};
} // namespace mahou