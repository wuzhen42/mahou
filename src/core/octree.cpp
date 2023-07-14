#include "octree.h"
#include "triangulate.h"
#include <numeric>
#include <stack>

namespace mahou {
Octree Octree::build(Mesh &mesh) {
  const TriangleSoup &triangle_soup = mesh.get<Triangulation>().make_soup(mesh);

  AABB bbox(triangle_soup);
  glm::vec3 extent = bbox.extent();
  float size = std::max(std::max(extent.x, extent.y), extent.z);
  bbox = {bbox.center(), glm::vec3{size, size, size}};

  Octree tree;

  std::stack<int> open;
  {
    const int num_triangles = triangle_soup.size() / 3;
    std::vector<int> triangles(num_triangles);
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

    std::vector<int> content{std::move(node.content)};
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
      std::vector<int> child_content;
      for (int tri : content) {
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

      const int idx_child = tree.nodes.size() + to_insert.size();
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

std::optional<std::pair<float, PointOnMesh>> Octree::closest(Ray ray, const Mesh &mesh) const {
  std::optional<std::pair<float, PointOnMesh>> result;
  const auto &triangulation = mesh.get<Triangulation>();

  for (int tri : this->all_potential_triangles(ray)) {
    Triangle triangle{
        mesh.position(triangulation.vtx(tri, 0)),
        mesh.position(triangulation.vtx(tri, 1)),
        mesh.position(triangulation.vtx(tri, 2)),
    };
    auto _result = intersect(ray, triangle);
    if (!_result.has_value())
      continue;
    auto [t, u, v] = _result.value();
    if (!result.has_value() || t < result.value().first)
      result = std::make_pair(t, PointOnMesh{tri, u, v});
  }
  return result;
}

std::vector<int> Octree::all_potential_triangles(Ray ray) const {
  std::stack<int> open;
  open.push(0);

  std::vector<int> result;
  while (!open.empty()) {
    const auto &node = nodes[open.top()];
    open.pop();

    if (!node.content.empty()) {
      AABB bbox = node.bbox;
      float t = intersect(ray, bbox).value();
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

  return result;
}

} // namespace mahou