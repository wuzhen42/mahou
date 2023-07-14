#include "halfedge.h"

namespace mahou {
Halfedge Halfedge::build(const Mesh &mesh) {
  Halfedge halfedge;
  halfedge.hedge_of_vtx.resize(mesh.num_vertices(), -1);

  for (int face = 0; face != mesh.num_faces(); ++face) {
    auto vertices = mesh.vertices(face);
    for (int i = 0; i != vertices.size(); ++i) {
      const int n = vertices.size();
      Hedge edge;
      edge.vtx = vertices[(i + 1) % n];
      edge.face = face;

      int slot = halfedge.edges.size();
      edge.prev = (i == 0 ? slot + (n - 1) : slot - 1);
      edge.next = (i + 1 == n ? slot - (n - 1) : slot + 1);
      edge.twin = -1;

      for (int t = 0; t != halfedge.edges.size(); ++t) {
        const Hedge other = halfedge.edges[t];
        if (other.face == face) // (prev/next) part of current face hafledges may be still ill-formed
          continue;
        if (other.vtx == vertices[i] && halfedge.edges[other.prev].vtx == edge.vtx) {
          edge.twin = t;
          halfedge.edges[t].twin = slot;
          break;
        }
      }

      halfedge.edges.push_back(edge);
      if (i == 0)
        halfedge.hedge_of_face.push_back(slot);
      if (halfedge.hedge_of_vtx[edge.vtx] < 0)
        halfedge.hedge_of_vtx[edge.vtx] = slot;
    }
  }

  // update hedge handle of boundary vertices to make sure it points to the first hedge.
  for (int vtx = 0; vtx != mesh.num_vertices(); ++vtx) {
    if (!halfedge.vtx_on_boundary(vtx))
      continue;
    int start = halfedge.hedge_of_vtx[vtx];
    while (halfedge.at(start).twin >= 0)
      start = halfedge.at(halfedge.at(start).twin).prev;
    halfedge.hedge_of_vtx[vtx] = start;
  }
  return halfedge;
}

std::vector<Halfedge::Hedge> Halfedge::vtx_hedges(int vtx) const {
  int start = hedge_of_vtx[vtx];
  Hedge p = at(start);
  std::vector<Hedge> result;
  while (true) {
    int t = at(p.next).twin;
    if (t < 0)
      break;
    p = at(t);
    result.push_back(p);
    if (t == start)
      break;
  };
  return result;
}

std::vector<Halfedge::Hedge> Halfedge::face_hedges(int face) const {
  int start = hedge_of_face[face];
  int p = start;
  std::vector<Hedge> result;
  do {
    result.push_back(at(p));
    p = at(p).next;
  } while (p != start);
  return result;
}

bool Halfedge::vtx_on_boundary(int vtx) const {
  for (auto x : vtx_hedges(vtx)) {
    if (x.twin < 0)
      return true;
  }
  return false;
}

std::vector<int> Halfedge::vtx_neighbors(int vtx) const {
  std::vector<int> vertices;
  for (Hedge x : vtx_hedges(vtx)) {
    vertices.push_back(at(x.prev).vtx);
  }
  return vertices;
}

std::vector<int> Halfedge::face_neighbors(int face) const {
  std::vector<int> faces;
  for (Hedge x : face_hedges(face)) {
    faces.push_back(at(x.twin).face);
  }
  return faces;
}

std::vector<std::pair<int, int>> Halfedge::face_neighbors(int face, int level) const {
  std::vector<std::pair<int, int>> result;
  std::set<int> closed;
  std::vector<int> open{face};

  int dist = 1;
  while (dist <= level) {
    auto current = open;
    open.clear();

    for (int x : current) {
      for (int other : face_neighbors(x)) {
        if (closed.count(other))
          continue;
        result.emplace_back(other, dist);
        open.push_back(other);
      }
    }

    dist++;
  }

  return result;
}
} // namespace mahou