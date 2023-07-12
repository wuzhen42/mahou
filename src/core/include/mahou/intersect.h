#pragma once
#include "types.hpp"
#include <array>
#include <glm/glm.hpp>
#include <boost/optional.hpp>
#include <vector>
#include <optional>

namespace mahou {
struct AABB {
  glm::vec3 min = {FLT_MAX, FLT_MAX, FLT_MAX};
  glm::vec3 max = {FLT_MIN, FLT_MIN, FLT_MIN};

  void extend(glm::vec3 pnt);

  AABB() = default;

  explicit AABB(const std::vector<glm::vec3> &points);

  AABB(glm::vec3 center, glm::vec3 extent);

  glm::vec3 center() const;

  glm::vec3 extent() const;
};

struct Ray {
  glm::vec3 o, d;
};

bool intersect(AABB bbox, Triangle triangle);

std::optional<float> intersect(Ray ray, AABB bbox);

std::optional<std::tuple<float, float, float>> intersect(Ray ray, Triangle triangle, float enlarge = 0.0);

} // namespace mahou