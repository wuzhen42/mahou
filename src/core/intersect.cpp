#include "intersect.h"

namespace mahou {

void AABB::extend(glm::vec3 pnt) {
  min = glm::min(min, pnt);
  max = glm::max(max, pnt);
}

AABB::AABB(const std::vector<glm::vec3> &points) {
  for (auto pnt : points)
    this->extend(pnt);
}

AABB::AABB(glm::vec3 center, glm::vec3 extent) {
  min = center - extent * 0.5f;
  max = center + extent * 0.5f;
}

glm::vec3 AABB::center() const { return 0.5f * (min + max); }

glm::vec3 AABB::extent() const { return max - min; }

glm::vec3 swizzle_yzx(glm::vec3 v) { return {v.y, v.z, v.x}; }

bool test_axes(glm::vec3 e, Triangle p, glm::vec3 extent) {
  glm::vec3 e_yzx = swizzle_yzx(e);

  glm::vec3 p0 = (p[0] * e_yzx) - (swizzle_yzx(p[0]) * e);
  glm::vec3 p1 = (p[1] * e_yzx) - (swizzle_yzx(p[1]) * e);
  glm::vec3 p2 = (p[2] * e_yzx) - (swizzle_yzx(p[2]) * e);

  glm::vec3 min = glm::min(p0, glm::min(p1, p2));
  glm::vec3 fe_xyz = glm::abs(e);
  glm::vec3 fe_yzx = swizzle_yzx(fe_xyz);
  glm::vec3 rad = (extent * fe_yzx) + (swizzle_yzx(extent) * fe_xyz);
  if (glm::any(glm::greaterThan(min, rad)))
    return false;

  glm::vec3 max = glm::max(p0, glm::max(p1, p2));
  if (glm::any(glm::greaterThan(-rad, max)))
    return false;
  return true;
}

bool intersect(AABB bbox, Triangle triangle) {
  glm::vec3 extent = 0.5f * bbox.extent();
  glm::vec3 p0 = triangle[0] - bbox.center();
  glm::vec3 p1 = triangle[1] - bbox.center();
  glm::vec3 p2 = triangle[2] - bbox.center();

  // test box axes
  {
    if (glm::all(glm::greaterThanEqual(extent, glm::abs(p0))))
      return true;

    glm::vec3 min = glm::min(glm::min(p0, p1), p2);
    if (glm::any(glm::greaterThan(min, extent)))
      return false;

    glm::vec3 max = glm::max(glm::max(p0, p1), p2);
    if (glm::any(glm::greaterThan(-extent, max)))
      return false;
  }

  // test plane of triangle
  glm::vec3 v01 = p1 - p0;
  glm::vec3 v12 = p2 - p1;
  {
    glm::vec3 normal = glm::cross(v01, v12);
    float d = glm::dot(normal, p0);

    glm::vec3 max = extent;
    glm::vec3 min = extent;
    for (int i = 0; i != 3; ++i) {
      if (extent[i] > 0 && normal[i] < 0)
        max[i] = -extent[i];
      if (extent[i] > 0 && normal[i] > 0)
        min[i] = -extent[i];
    }

    if (d > glm::dot(normal, max))
      return false;
    if (glm::dot(normal, min) > d)
      return false;
  }

  // edge-edge test
  if (!test_axes(v01, {p0, p1, p2}, extent))
    return false;
  if (!test_axes(v12, {p0, p1, p2}, extent))
    return false;
  if (!test_axes(p0 - p2, {p0, p1, p2}, extent))
    return false;

  return true;
}

std::optional<float> intersect(Ray ray, AABB bbox) {
  for (int i = 0; i != 3; ++i)
    if (ray.d[i] == 0)
      ray.d[i] = 1E-10;

  float t1 = (bbox.min.x - ray.o.x) / ray.d.x;
  float t2 = (bbox.max.x - ray.o.x) / ray.d.x;
  float t3 = (bbox.min.y - ray.o.y) / ray.d.y;
  float t4 = (bbox.max.y - ray.o.y) / ray.d.y;
  float t5 = (bbox.min.z - ray.o.z) / ray.d.z;
  float t6 = (bbox.max.z - ray.o.z) / ray.d.z;

  float tmin = std::max(std::max(std::min(t1, t2), std::min(t3, t4)), std::min(t5, t6));
  float tmax = std::min(std::min(std::max(t1, t2), std::max(t3, t4)), std::max(t5, t6));

  // if tmax < 0, ray (line) is intersecting AABB, but whole AABB is behind us
  if (tmax < 0)
    return std::nullopt;

  // if tmin > tmax, ray doesn't intersect AABB
  if (tmin > tmax)
    return std::nullopt;

  return tmin < 0 ? tmax : tmin;
}

std::optional<std::tuple<float, float, float>> intersect(Ray ray, Triangle triangle, float enlarge) {
  // Find vectors for two edges sharing vert0
  const glm::vec3 edge1 = triangle[1] - triangle[0];
  const glm::vec3 edge2 = triangle[2] - triangle[0];

  // Begin calculating determinant - also used to calculate U parameter
  const glm::vec3 pvec = glm::cross(ray.d, edge2); // error ~ |v2-v0|

  // If determinant is near zero, ray lies in plane of triangle
  const float det = glm::dot(edge1, pvec); // error ~ |v2-v0|*|v1-v0|

  // the non-culling branch
  if (std::abs(det) < FLT_EPSILON * FLT_EPSILON)
    return std::nullopt;

  const float inv_det = 1.0 / det;

  // Calculate distance from vert0 to ray origin
  const glm::vec3 tvec = ray.o - triangle[0]; // error ~ |orig-v0|

  // Calculate U parameter and test bounds
  const float u = glm::dot(tvec, pvec) * inv_det;
  if (u < -enlarge || u > 1.0 + enlarge)
    return std::nullopt;

  // prepare to test V parameter
  const glm::vec3 qvec = glm::cross(tvec, edge1);

  // Calculate V parameter and test bounds
  const float v = glm::dot(ray.d, qvec) * inv_det;
  if (v < -enlarge || (u + v) > 1.0 + enlarge)
    return std::nullopt;

  // Calculate t, ray intersects triangle
  const float t = glm::dot(edge2, qvec) * inv_det;
  if (t < 0)
    return std::nullopt;

  return std::make_tuple(t, u, v);
}

} // namespace mahou