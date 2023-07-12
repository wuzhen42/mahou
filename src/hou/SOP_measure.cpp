#include "SOP_measure.h"
#include <mahou/ddg.h>

#include <GEO/GEO_PrimPoly.h>
#include <GU/GU_Detail.h>
#include <OP/OP_AutoLockInputs.h>
#include <OP/OP_Director.h>
#include <PRM/PRM_ChoiceList.h>
#include <fmt/format.h>
#include <glm/glm.hpp>

static PRM_Name sopMeasureTypeName("type", "Measure Type");
static PRM_Name sopMeasureType[] = {PRM_Name("gradient", "Gradient X"), PRM_Name("laplace", "Laplace X"), PRM_Name(0)};
static PRM_ChoiceList sopMeasureTypeMenu(PRM_CHOICELIST_SINGLE, sopMeasureType);
PRM_Template SOP_measure::myTemplateList[] = {PRM_Template(PRM_ORD, 1, &sopMeasureTypeName, 0, &sopMeasureTypeMenu),
                                              PRM_Template()};

mahou::Mesh build_mahou_mesh(GU_Detail *gdp) {
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

struct HouMesh {
  GU_Detail *gdp;
  const GA_RWHandleV3 &Phandle;

public:
  HouMesh(GU_Detail *gdp, const GA_RWHandleV3 &Phandle) : gdp(gdp), Phandle(Phandle) {}

  std::size_t num_faces() const { return gdp->getNumPrimitives(); }

  bool is_triangle(int face) const { return gdp->getPrimitiveByIndex(face)->getVertexCount() == 3; }

  int face_vtx(int face, int localvtx) const {
    GA_Primitive *prim = gdp->getPrimitiveByIndex(face);
    localvtx = prim->getVertexCount() - 1 - localvtx;
    return prim->getPointIndex(localvtx);
  }

  glm::vec3 position(int vtx) const {
    UT_Vector3F pnt = Phandle.get(gdp->pointOffset(vtx));
    return {pnt.x(), pnt.y(), pnt.z()};
  }
};

SOP_measure::~SOP_measure() {}

OP_ERROR SOP_measure::cookMySop(OP_Context &context) {
  OP_AutoLockInputs inputs(this);
  if (inputs.lock(context) >= UT_ERROR_ABORT)
    return error();
  fpreal now = context.getTime();
  const int type = evalInt("type", 0, now);

  duplicateSource(0, context);

  try {
    mahou::Mesh mesh = build_mahou_mesh(gdp);
    GA_RWHandleF Fhandle = gdp->findAttribute(GA_ATTRIB_POINT, "dist");
    auto gradients = mahou::gradient_on(mesh, [&Fhandle](int vtx) -> float { return Fhandle.get(vtx); });

    GA_RWHandleV3 Vhandle = gdp->addFloatTuple(GA_ATTRIB_PRIMITIVE, "gradient", 3);
    for (GA_Offset prim = 0; prim < gradients.size(); ++prim) {
      Vhandle.set(prim, UT_Vector3F{gradients[prim].x, gradients[prim].y, gradients[prim].z});
    }
  } catch (const std::runtime_error &e) {
    addError(SOP_MESSAGE, e.what());
  }

  return error();
}
