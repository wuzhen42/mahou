#pragma once
#include "mesh.h"

namespace mahou {
// shape diameter function
class SDF {
  std::vector<float> value;

  void fill_missing_value(const Mesh &mesh);

public:
  float at(int vtx) const { return value.at(vtx); }

  static SDF build(Mesh &mesh, double angle = 120, int samples = 30);
};
} // namespace mahou