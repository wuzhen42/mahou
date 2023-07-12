#include "triangulate.h"

namespace mahou {
void Triangulation::push(int v0, int v1, int v2, int face) {
  triangles.push_back({v0, v1, v2});
  face_of_triangle.push_back(face);
}

Triangulation Triangulation::build(const Mesh &mesh) {
  Triangulation triangulation;
  for (int face = 0; face != mesh.num_faces(); ++face) {
    if (mesh.is_triangle(face)) {
      triangulation.push(mesh.vtx(face, 0), mesh.vtx(face, 1), mesh.vtx(face, 2), face);
    } else {
      float dist_02 = glm::distance(mesh.position(mesh.vtx(face, 0)), mesh.position(mesh.vtx(face, 2)));
      float dist_13 = glm::distance(mesh.position(mesh.vtx(face, 1)), mesh.position(mesh.vtx(face, 3)));
      if (dist_02 < dist_13) {
        triangulation.push(mesh.vtx(face, 1), mesh.vtx(face, 2), mesh.vtx(face, 0), face);
        triangulation.push(mesh.vtx(face, 0), mesh.vtx(face, 2), mesh.vtx(face, 3), face);
      } else {
        triangulation.push(mesh.vtx(face, 0), mesh.vtx(face, 1), mesh.vtx(face, 3), face);
        triangulation.push(mesh.vtx(face, 3), mesh.vtx(face, 1), mesh.vtx(face, 2), face);
      }
    }
  }
  return triangulation;
}

TriangleSoup Triangulation::make_soup(const Mesh &mesh) const {
  TriangleSoup soup;
  for (auto tri : triangles) {
    soup.push_back(mesh.position(tri[0]));
    soup.push_back(mesh.position(tri[1]));
    soup.push_back(mesh.position(tri[2]));
  }
  return soup;
}
} // namespace mahou