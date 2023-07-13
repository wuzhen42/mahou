#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <mahou/sdf.h>

TEST_CASE("sdf plain", "[sdf]") {
  using namespace mahou;

  Mesh mesh = Mesh::load_from_obj("C:/proj/mahou/data/spot_triangulated.obj");
  SDF::build(mesh);
}
