#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <mahou/mesh.h>
#include <mahou/octree.h>
#include <mahou/triangulate.h>

TEST_CASE("raycast octree", "[octree]") {
  using namespace mahou;

  Mesh mesh = Mesh::load_from_obj("C:/proj/mahou/data/spot_triangulated.obj");
  Octree octree = Octree::build(mesh);

  {
    Ray ray{glm::vec3{-0.05, 1, -2}, glm::vec3{0.05, -1, 2}};
    auto result = octree.closest(ray, mesh);
    REQUIRE(result.has_value());
    REQUIRE_THAT(result.value().first, Catch::Matchers::WithinAbs(0.6671182513237, 1e-5));
    REQUIRE(result.value().second.triangle == 5305);
  }

  {
    Ray ray{glm::vec3{-0.05, 1, -2}, glm::vec3{0.05, -1, 1}};
    auto result = octree.closest(ray, mesh);
    REQUIRE_FALSE(result.has_value());
  }
}
