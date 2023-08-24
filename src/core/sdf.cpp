#include "sdf.h"
#include "halfedge.h"
#include "normal.h"
#include "octree.h"
#include <glm/gtx/quaternion.hpp>
#include <random>
#include <set>

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

    float total_value = 0;
    float total_weight = 0;
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
      float weight = angle != 0 ? 1.0f / angle : 1.0f;
      weight = std::max(1.0f, weight);

      total_value += t * weight;
      total_weight += weight;
      counter++;
    }
    if (total_weight != 0)
      sdf.value.push_back(total_value / total_weight);
    else
      sdf.value.push_back(0.f);
  }

  // postprocess
  sdf.fill_missing_value(mesh);
  sdf.smooth(mesh);
  sdf.to_logspace();
  return sdf;
}

void SDF::fill_missing_value(Mesh &mesh) {
  const Halfedge &halfedge = mesh.get<Halfedge>();
  for (int face = 0; face != mesh.num_faces(); ++face) {
    if (value[face] == 0) {
      float sum = 0;
      int counter = 0;
      for (int other : halfedge.face_neighbors(face)) {
        if (value[other] > 0) {
          sum += value[other];
          ++counter;
        }
      }
      value[face] = sum / counter;
    }
  }

  float min = FLT_MAX;
  std::vector<int> missing;
  for (int face = 0; face != mesh.num_faces(); ++face) {
    if (value[face] > 0)
      min = std::min(min, value[face]);
    else
      missing.push_back(face);
  }
  for (int face : missing)
    value[face] = min;
}

#define SQUARE(x) ((x) * (x))
float guassian(float value, float deviation) { return std::exp(-0.5f * SQUARE(value / deviation)); }

void SDF::smooth(Mesh &mesh) {
  int window_size = std::floor(std::sqrt(mesh.num_faces() / 2000)) + 1;
  float spatial_factor = window_size / 2.0;

  const Halfedge &halfedge = mesh.get<Halfedge>();

  std::vector<float> smoothed(value.size(), 0);
  for (int face = 0; face != mesh.num_faces(); ++face) {
    auto neighbors = halfedge.face_neighbors(face, window_size);
    float curr = value.at(face);

    float range_factor;
    {
      float deviation = 0;
      for (auto [other, dist] : neighbors)
        deviation += SQUARE(value[other] - curr);
      deviation = std::sqrt(deviation / neighbors.size());
      range_factor = 1.5 * deviation;
    }
    if (range_factor == 0) {
      smoothed[face] = curr;
      break;
    }

    float total_value = 0;
    float total_weight = 0;
    for (auto [other, dist] : neighbors) {
      float spatial_weight = guassian(dist, spatial_factor);
      float range_weight = guassian(std::abs(value[other] - curr), range_factor);
      float weight = spatial_weight * range_weight;

      total_value += value[other] * weight;
      total_weight += weight;
    }
    smoothed[face] = total_value / total_weight;
  }

  this->value = smoothed;
}
#undef SQUARE

void SDF::normalize() {
  float max = *std::max_element(value.begin(), value.end());
  for (float &x : value)
    x /= max;
}

void SDF::to_logspace() {
  auto [_min, _max] = std::minmax_element(value.begin(), value.end());
  float min = *_min;
  float max = *_max;

  const float alpha = 4.0;
  for (float &x : value)
    x = std::log((x - min) / (max - min) * alpha + 1) / std::log(alpha + 1);
}
} // namespace mahou