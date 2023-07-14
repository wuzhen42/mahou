#pragma once
#include "mesh.h"
#include <set>

namespace mahou {
class Halfedge {
  struct Hedge;
  std::vector<Hedge> edges;
  std::vector<int> hedge_of_vtx, hedge_of_face;

public:
  struct Hedge {
    int vtx;
    int face;
    int prev;
    int next;
    int twin;
  };

  static Halfedge build(const Mesh &mesh);

  Hedge at(int idx) const { return edges.at(idx); }

  // TODO change this to iterator
  std::vector<Hedge> vtx_hedges(int vtx) const;

  std::vector<Hedge> face_hedges(int face) const;

  bool vtx_on_boundary(int vtx) const;

  std::vector<int> vtx_neighbors(int vtx) const;

  std::vector<int> face_neighbors(int face) const;

  std::vector<std::pair<int, int>> face_neighbors(int face, int level) const;
};

} // namespace mahou