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
    char key;
    iss >> key;
    switch (key) {
    case 'v': {
      glm::vec3 pnt;
      iss >> pnt.x >> pnt.y >> pnt.z;
      mesh.positions.push_back(pnt);
    } break;
    case 'f': {
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
    } break;
    case '#':
      break;
    default:
      throw std::runtime_error(fmt::format("error on loading *.obj: {}", path));
    }
  }

  return mesh;
}
} // namespace mahou
