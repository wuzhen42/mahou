#include "normal.h"
#include "triangulate.h"

namespace mahou {
PolygonNormal PolygonNormal::build(const Mesh &mesh) {
  std::vector<glm::vec3> normals;
  for (int face = 0; face != mesh.num_faces(); ++face) {
    glm::vec3 normal{0, 0, 0};
    auto vertices = mesh.vertices(face);
    for (int t = 0; t != vertices.size(); ++t) {
      glm::vec3 curr = mesh.position(vertices[t]);
      glm::vec3 next = mesh.position(vertices[(t + 1) % vertices.size()]);
      normal.x += (curr.y - next.y) * (curr.z + next.z);
      normal.y += (curr.z - next.z) * (curr.x + next.x);
      normal.z += (curr.x - next.x) * (curr.y + next.y);
    }
    normals.push_back(glm::normalize(normal));
  }

  return PolygonNormal{normals};
}

TriangleNormal TriangleNormal::build(Mesh &mesh) {
  const auto &triangulation = mesh.get<Triangulation>();
  std::vector<glm::vec3> normals;

  for (int i = 0; i != triangulation.size(); ++i) {
    glm::vec3 p0 = mesh.position(triangulation.vtx(i, 0));
    glm::vec3 p1 = mesh.position(triangulation.vtx(i, 1));
    glm::vec3 p2 = mesh.position(triangulation.vtx(i, 2));
    normals.push_back(glm::normalize(glm::cross(p1 - p0, p2 - p0)));
  }
  return TriangleNormal{normals};
}

VertexNormal VertexNormal::build(Mesh &mesh, bool angleWeighted) {
  std::vector<glm::vec3> normals(mesh.num_vertices(), {0, 0, 0});
  const PolygonNormal &faceNormal = mesh.get<PolygonNormal>();

  for (int face = 0; face != mesh.num_faces(); ++face) {
    auto vertices = mesh.vertices(face);
    for (int i = 0; i != vertices.size(); ++i) {
      int curr = vertices[i];
      int next = vertices[(i + 1) % vertices.size()];
      int prev = vertices[(i - 1 + vertices.size()) % vertices.size()];
      glm::vec3 vec_to_prev = mesh.position(prev) - mesh.position(curr);
      glm::vec3 vec_to_next = mesh.position(next) - mesh.position(curr);
      float angle = std::acos(glm::dot(glm::normalize(vec_to_prev), glm::normalize(vec_to_next)));
      normals[curr] += angle * faceNormal.at(face);
    }
  }

  for (auto &x : normals)
    x = glm::normalize(x);
  return VertexNormal{normals};
}

} // namespace mahou