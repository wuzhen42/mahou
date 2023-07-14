#pragma once

#include <any>
#include <boost/container/static_vector.hpp>
#include <fmt/format.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>

namespace mahou {

class Mesh {
public:
  using Face = boost::container::static_vector<int, 4>;

private:
  std::vector<glm::vec3> positions;
  std::vector<Face> faces;
  std::vector<std::any> components;

public:
  Mesh() = default;

  Mesh(std::vector<glm::vec3> positions, std::vector<Face> faces) : positions(positions), faces(faces) {}

  std::size_t num_vertices() const { return positions.size(); }

  std::size_t num_faces() const { return faces.size(); }

  glm::vec3 position(int vtx) const { return positions[vtx]; }

  bool is_triangle(int face) const { return faces[face].size() == 3; }

  bool is_quad(int face) const { return faces[face].size() == 4; }

  Face vertices(int face) const { return faces[face]; }

  int vtx(int face, int localvtx) const { return vertices(face)[localvtx]; }

  template <typename T> const T &get() const {
    for (const auto &x : components) {
      if (x.type() == typeid(T))
        return std::any_cast<const T &>(x);
    }
    throw std::runtime_error(fmt::format("no required component: [{}]", typeid(T).name()));
  }

  template <typename T> T &get() {
    require<T>();
    for (auto &x : components) {
      if (x.type() == typeid(T))
        return std::any_cast<T &>(x);
    }
    throw std::runtime_error(fmt::format("no required component: [{}]", typeid(T).name()));
  }

  template <typename T> void require() {
    for (auto &x : components) {
      if (x.type() == typeid(T))
        return;
    }

    components.emplace_back(T::build(*this));
  }

  static Mesh load_from_obj(std::string path);
};
} // namespace mahou
