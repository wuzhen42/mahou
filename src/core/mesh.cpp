#include "mesh.h"
#include <fmt/format.h>
#include <fstream>
#include <sstream>

namespace mahou {
Mesh Mesh::load_from_obj(std::string path) {
  std::ifstream fin(path);
  if (!fin.is_open())
    throw std::runtime_error(fmt::format("invalid path: {}", path));

  Mesh mesh;

  std::string line;
  while (std::getline(fin, line)) {
    std::istringstream iss(line);
    std::string key;
    iss >> key;
    if ("v" == key) {
      glm::vec3 pnt;
      iss >> pnt.x >> pnt.y >> pnt.z;
      mesh.positions.push_back(pnt);
    } else if ("f" == key) {
      Face face;
      std::string vertex;
      while (iss >> vertex) {
        if (vertex.empty())
          break;
        std::size_t split = vertex.find('/');
        if (split != std::string::npos)
          vertex = vertex.substr(0, split);
        face.push_back(std::stoi(vertex) - 1);
      }
      mesh.faces.push_back(face);
    } else if ("#" == key || "vt" == key) {
      continue;
    } else {
      throw std::runtime_error(fmt::format("error on loading obj: {} [unknown symbol: {}]", path, key));
    }
  }

  return mesh;
}
} // namespace mahou
