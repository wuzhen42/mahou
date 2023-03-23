#include "mesh.h"
#include <mahou/gradient.hpp>

#include <fmt/format.h>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>

int main() {
  mahou::Mesh mesh = mahou::Mesh::load_from_obj("D:/proj/mahou/data/spot_quadrangulated.obj");

  auto gradients = mahou::gradient_on(mesh, [&mesh](unsigned vtx) -> float { return mesh.position(vtx).x; });
  std::cout << fmt::format("gradients: {}", gradients.size()) << std::endl;
  for (unsigned i = 0; i != std::min<unsigned>(10, gradients.size()); ++i) {
    std::cout << fmt::format("gradient[{}]: {}", i, glm::to_string(gradients[i])) << std::endl;
  }
}
