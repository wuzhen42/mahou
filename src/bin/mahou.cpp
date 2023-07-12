#include "mahou/octree.h"
#include "mesh.h"

#include <fmt/format.h>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>

int main() {
  using namespace mahou;

  Mesh mesh = Mesh::load_from_obj("C:/proj/mahou/data/spot_triangulated.obj");

  TriangleSoup triangles;
  for (int i = 0; i != mesh.num_faces(); ++i) {
    triangles.push_back(mesh.position(mesh.face_vtx(i, 0)));
    triangles.push_back(mesh.position(mesh.face_vtx(i, 1)));
    triangles.push_back(mesh.position(mesh.face_vtx(i, 2)));
  }

  Octree octree = Octree::build(triangles);
  Ray ray{glm::vec3{-0.05, 1, -2}, glm::vec3{0.05, -1, 2}};
  for (int tri : octree.intersections(ray)) {
    Triangle triangle{
        mesh.position(mesh.face_vtx(tri, 0)),
        mesh.position(mesh.face_vtx(tri, 1)),
        mesh.position(mesh.face_vtx(tri, 2)),
    };
    auto result = intersect(ray, triangle);
    if (!result.has_value())
      continue;
    auto [t, u, v] = result.value();
    std::cout << fmt::format("triangle: {} t: {}", tri, t) << std::endl;
  }
}
