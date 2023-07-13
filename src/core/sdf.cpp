#include "sdf.h"
#include "normal.h"
#include "octree.h"
#include <glm/gtx/quaternion.hpp>
#include <random>

#include "log.hpp"

namespace mahou {
SDF SDF::build(Mesh &mesh, double span, int samples) {
  mesh.require<Octree>();
  mesh.require<PolygonNormal>();
  mesh.require<TriangleNormal>();
  const auto &octree = mesh.get<Octree>();
  const auto &face_norm = mesh.get<PolygonNormal>();
  const auto &tri_norm = mesh.get<TriangleNormal>();

  span = glm::radians(std::min(std::max(span, 0.0), 180.0));
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<float> angle_sampler(0.f, 2 * M_PI);
  std::uniform_real_distribution<float> height_sampler(std::cos(span / 2), 1.f);

  SDF sdf{};
  for (int face = 0; face != mesh.num_faces(); ++face) {
    glm::vec3 axis = -face_norm.at(face);
    const bool is_norm_up = glm::all(glm::equal(glm::vec3{0, 1, 0}, axis));
    const bool is_norm_down = glm::all(glm::equal(glm::vec3{0, -1, 0}, axis));

    glm::vec3 face_center{0, 0, 0};
    for (auto vtx : mesh.vertices(face))
      face_center += mesh.position(vtx);
    face_center /= mesh.vertices(face).size();

    float value = 0;
    int counter = 0;
    for (int i = 0; i != samples; ++i) {
      float theta = angle_sampler(gen);
      float y = height_sampler(gen);
      float xz = std::sqrt(1 - y * y);
      glm::vec3 dir{xz * std::cos(theta), y, xz * std::sin(theta)};

      if (!is_norm_up && !is_norm_down) {
        glm::quat quat(glm::vec3{0, 1, 0}, axis);
        dir = quat * dir;
      } else if (is_norm_down) {
        dir = -dir;
      }

      Ray ray{face_center + 1E-3F * dir, dir};
      auto result = octree.closest(ray, mesh);
      if (!result.has_value())
        continue;

      auto [t, pointOnMesh] = result.value();
      if (glm::dot(tri_norm.at(pointOnMesh.triangle), axis) < 0)
        continue;

      float angle = std::acos(glm::dot(dir, axis));
      value += t * (1.0f / angle);
      counter++;
    }
    if (counter != 0)
      value /= counter;
    sdf.value.push_back(value);
  }

  return sdf;
}
} // namespace mahou