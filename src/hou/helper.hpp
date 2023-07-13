#pragma once
#include <GU/GU_Detail.h>
#include <mahou/mesh.h>

inline mahou::Mesh build_mahou_mesh(GU_Detail *gdp) {
  std::vector<glm::vec3> positions;
  GA_Offset ptoff;
  GA_FOR_ALL_PTOFF(gdp, ptoff) {
    UT_Vector3F pnt = gdp->getPos3(ptoff);
    positions.emplace_back(pnt.x(), pnt.y(), pnt.z());
  }

  std::vector<mahou::Mesh::Face> faces;
  GA_Primitive *prim;
  GA_FOR_ALL_PRIMITIVES(gdp, prim) {
    if (prim->getVertexCount() > 4)
      throw std::runtime_error(fmt::format("face {} has more than 4 vertices", prim->getMapIndex()));

    mahou::Mesh::Face face;
    // Houdini face is CCW order
    for (int i = prim->getVertexCount() - 1; i >= 0; --i)
      face.push_back(prim->getPointIndex(i));
    faces.emplace_back(face);
  }

  return mahou::Mesh{std::move(positions), std::move(faces)};
}
