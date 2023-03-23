#pragma once

#include <boost/container/static_vector.hpp>
#include <glm/glm.hpp>
#include <string>
#include <vector>

namespace mahou {

class Mesh {
public:
  using Face = boost::container::static_vector<unsigned, 4>;

private:
  std::vector<glm::vec3> positions;
  std::vector<Face> faces;

public:
  std::size_t num_vertices() const { return positions.size(); }

  std::size_t num_faces() const { return faces.size(); }

  glm::vec3 position(unsigned vtx) const { return positions[vtx]; }

  bool is_triangle(unsigned face) const { return faces[face].size() == 3; }

  bool is_quad(unsigned face) const { return faces[face].size() == 4; }

  Face vertices(unsigned face) const { return faces[face]; }

  unsigned face_vtx(unsigned face, unsigned localvtx) const { return vertices(face)[localvtx]; }

  static Mesh load_from_obj(std::string path);
};
} // namespace mahou