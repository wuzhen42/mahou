#include <catch2/catch_test_macros.hpp>

#include <mahou/intersect.h>

TEST_CASE("triangle and bbox", "[intersection]") {
  mahou::AABB bbox(glm::vec3{0, 0, 0}, glm::vec3{2, 2, 2});
  std::array<glm::vec3, 3> triangle{
      glm::vec3{0, 0, 0},
      glm::vec3{1, 0, 0},
      glm::vec3{0, 0, 1},
  };
  REQUIRE(mahou::intersect(bbox, triangle));

  triangle[0] = {1.944064335128112, 0.4217371405854736, 0.4003542444723278};
  triangle[1] = {2.1232333755585553, -0.2874546321173621, -0.2815148664880115};
  triangle[2] = {0.9602729902994372, 0.2874546321173621, 0.2815148664880115};
  REQUIRE(mahou::intersect(bbox, triangle));

  triangle[2] = {1.0602729902994372, 0.2874546321173621, 0.2815148664880115};
  REQUIRE(!mahou::intersect(bbox, triangle));

  triangle[0] = {0.2561263648755575, 1.403542153457057, 0.4968969626018003};
  triangle[1] = {2.1367682922621087, 0.7311670945959243, 0.391551240442393};
  triangle[2] = {0.15212792468173764, 1.4255682284115523, -1.5002758332422026};
  REQUIRE(!mahou::intersect(bbox, triangle));
}
