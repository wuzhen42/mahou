#include "SOP_measure.h"
#include "helper.hpp"
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
