#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>

#include <mahou/halfedge.h>

TEST_CASE("neighbors", "[halfedge]") {
  using namespace mahou;

  Mesh mesh = Mesh::load_from_obj("C:/proj/mahou/data/spot_quadrangulated.obj");
  Halfedge halfedge = Halfedge::build(mesh);
  REQUIRE_THAT(halfedge.vtx_neighbors(931), Catch::Matchers::UnorderedEquals(std::vector<int>{929, 925, 930, 922}));
  REQUIRE_THAT(halfedge.face_neighbors(2203),
               Catch::Matchers::UnorderedEquals(std::vector<int>{2200, 2125, 2197, 2202}));
}
