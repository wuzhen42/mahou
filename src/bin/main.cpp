#include "mahou/ddg.hpp"
#include "mesh.h"

#include <fmt/format.h>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>

int main() {
  mahou::Mesh mesh = mahou::Mesh::load_from_obj("D:/proj/mahou/data/spot_quadrangulated.obj");

  auto laplacians = mahou::laplace_on(mesh, [&mesh](unsigned vtx) -> float { return mesh.position(vtx).x; });
  for (unsigned i = 0; i != std::min<unsigned>(10, laplacians.size()); ++i) {
    std::cout << fmt::format("laplace[{}]: {}", i, laplacians[i]) << std::endl;
  }
}
