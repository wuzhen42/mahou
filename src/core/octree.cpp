#include "octree.h"
#include <numeric>
#include <stack>

#include <fmt/format.h>
#include <glm/gtx/string_cast.hpp>
#include <iostream>

namespace mahou {
Octree Octree::build(const TriangleSoup &triangle_soup) {
  AABB bbox(triangle_soup);
  glm::vec3 extent = bbox.extent();
  float size = std::max(std::max(extent.x, extent.y), extent.z);
  bbox = {bbox.center(), glm::vec3{size, size, size}};

  Octree tree;

  std::stack<unsigned> open;
  {
    const unsigned num_triangles = triangle_soup.size() / 3;
    std::vector<unsigned> triangles(num_triangles);
    std::iota(triangles.begin(), triangles.end(), 0);
    Octree::Node top{bbox};
    top.content = triangles;
    tree.nodes.push_back(top);
    open.push(0);
  }

  while (!open.empty()) {
    auto &node = tree.nodes[open.top()];
    open.pop();

    // linear search would be optimal for few triangles
    if (node.content.size() < 8)
      continue;

    std::vector<unsigned> content{std::move(node.content)};
    std::vector<Octree::Node> to_insert;

    AABB bbox = node.bbox;
    glm::vec3 child_extent = 0.5f * bbox.extent();
    static std::array<glm::vec3, 8> offset{
        glm::vec3{+1.f, -1.f, -1.f}, // Right, Top, Front
        glm::vec3{+1.f, -1.f, +1.f}, // Right, Top, Back
        glm::vec3{+1.f, +1.f, -1.f}, // Right, Bottom, Front
        glm::vec3{+1.f, +1.f, +1.f}, // Right, Bottom, Back
        glm::vec3{-1.f, -1.f, -1.f}, // Left, Top, Front
        glm::vec3{-1.f, -1.f, +1.f}, // Left, Top, Back
        glm::vec3{-1.f, +1.f, -1.f}, // Left, Bottom, Front
        glm::vec3{-1.f, +1.f, +1.f}, // Left, Bottom, Back
    };

    for (int i = 0; i != 8; ++i) {
      AABB child_bbox{bbox.center() + 0.5f * child_extent.x * offset[i], child_extent};
      std::vector<unsigned> child_content;
      for (unsigned tri : content) {
        Triangle triangle{triangle_soup[3 * tri], triangle_soup[3 * tri + 1], triangle_soup[3 * tri + 2]};
        if (mahou::intersect(child_bbox, triangle))
          child_content.push_back(tri);
      }
      if (child_content.empty())
        continue;

      // split will fail if mesh vertex happens to be on grid vertex
      if (child_content.size() == content.size()) {
        // stop splitting and cancel everything
        node.content = std::move(content);
        node.children.fill(0);
        to_insert.clear();
        break;
      }

      // insert child node
      Octree::Node child{child_bbox};
      child.content = std::move(child_content);

      const unsigned idx_child = tree.nodes.size() + to_insert.size();
      node.children[i] = idx_child;
      to_insert.push_back(child);
    }

    for (auto &x : to_insert) {
      tree.nodes.push_back(std::move(x));
      open.push(tree.nodes.size() - 1);
    }
  }

  return tree;
}

std::vector<unsigned> Octree::intersections(Ray ray) const {
  std::stack<int> open;
  open.push(0);

  std::vector<unsigned> result;
  while (!open.empty()) {
    const auto &node = nodes[open.top()];
    open.pop();

    if (!node.content.empty()) {
      AABB bbox = node.bbox;
      float t = intersect(ray, bbox).value();
      std::cout << fmt::format("t: {}, bbox: ({}, {})", t, glm::to_string(bbox.min), glm::to_string(bbox.max))
                << std::endl;
      result.insert(result.end(), node.content.begin(), node.content.end());
      continue;
    }

    for (int child : node.children) {
      if (child == 0)
        continue;
      if (intersect(ray, nodes[child].bbox).has_value()) {
        open.push(child);
      }
    }
  }

  std::cout << "candidate triangles: " << result.size() << std::endl;
  return result;
}

} // namespace mahou