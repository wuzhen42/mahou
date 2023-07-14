#pragma once
#include "mesh.h"
#include <functional>

namespace mahou {
std::vector<glm::vec3> gradient_on(const Mesh &mesh, std::function<float(int)> phi);

std::vector<float> laplace_on(const Mesh &mesh, std::function<float(int)> phi);

} // namespace mahou
