#include "SOP_measure.h"
#include "mahou/gradient.hpp"

#include <GEO/GEO_PrimPoly.h>
#include <GU/GU_Detail.h>
#include <OP/OP_AutoLockInputs.h>
#include <OP/OP_Director.h>
#include <PRM/PRM_ChoiceList.h>
#include <glm/glm.hpp>

static PRM_Name sopMeasureTypeName("type", "Measure Type");
static PRM_Name sopMeasureType[] = {PRM_Name("gradient", "Gradient X"), PRM_Name("laplace", "Laplace X"), PRM_Name(0)};
static PRM_ChoiceList sopMeasureTypeMenu(PRM_CHOICELIST_SINGLE, sopMeasureType);
PRM_Template SOP_measure::myTemplateList[] = {PRM_Template(PRM_ORD, 1, &sopMeasureTypeName, 0, &sopMeasureTypeMenu),
                                              PRM_Template()};

struct HouMesh {
  GU_Detail *gdp;
  const GA_RWHandleV3 &Phandle;

public:
  HouMesh(GU_Detail *gdp, const GA_RWHandleV3 &Phandle) : gdp(gdp), Phandle(Phandle) {}

  std::size_t num_faces() const { return gdp->getNumPrimitives(); }

  bool is_triangle(unsigned face) const { return gdp->getPrimitiveByIndex(face)->getVertexCount() == 3; }

  unsigned face_vtx(unsigned face, unsigned localvtx) const {
    GA_Primitive *prim = gdp->getPrimitiveByIndex(face);
    localvtx = prim->getVertexCount() - 1 - localvtx;
    return prim->getPointIndex(localvtx);
  }

  glm::vec3 position(unsigned vtx) const {
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

  GA_RWHandleF Fhandle = gdp->findAttribute(GA_ATTRIB_POINT, "dist");

  GA_RWHandleV3 Phandle = gdp->findAttribute(GA_ATTRIB_POINT, "P");
  HouMesh mesh(gdp, Phandle);
  auto gradients = mahou::gradient_on(mesh, [&Fhandle](unsigned vtx) -> float { return Fhandle.get(vtx); });

  GA_RWHandleV3 Vhandle = gdp->addFloatTuple(GA_ATTRIB_PRIMITIVE, "gradient", 3);
  for (GA_Offset prim = 0; prim < gradients.size(); ++prim) {
    Vhandle.set(prim, UT_Vector3F{gradients[prim].x, gradients[prim].y, gradients[prim].z});
  }

  return error();
}
