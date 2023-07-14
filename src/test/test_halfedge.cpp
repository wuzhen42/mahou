#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>

#include <mahou/halfedge.h>

TEST_CASE("ring", "[halfedge]") {
  using namespace mahou;

  Mesh mesh = Mesh::load_from_obj("C:/proj/mahou/data/spot_quadrangulated.obj");
  Halfedge halfedge = Halfedge::build(mesh);
  REQUIRE_THAT(halfedge.ring(931), Catch::Matchers::UnorderedEquals(std::vector<int>{929, 925, 930, 922}));
}
