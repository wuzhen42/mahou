#include "SOP_sdf.h"
#include "helper.hpp"
#include <OP/OP_AutoLockInputs.h>
#include <mahou/sdf.h>

PRM_Template SOP_SDF::myTemplateList[] = {PRM_Template()};

OP_ERROR SOP_SDF::cookMySop(OP_Context &context) {
  OP_AutoLockInputs inputs(this);
  if (inputs.lock(context) >= UT_ERROR_ABORT)
    return error();

  duplicateSource(0, context);

  try {
    mahou::Mesh mesh = build_mahou_mesh(gdp);
    auto sdf = mahou::SDF::build(mesh);

    GA_RWHandleF Fhandle = gdp->addFloatTuple(GA_ATTRIB_PRIMITIVE, "sdf", 1);
    GA_Primitive *prim;
    GA_FOR_ALL_PRIMITIVES(gdp, prim) {
      GA_Index id = prim->getMapIndex();
      Fhandle.set(id, sdf.at(id));
    }

  } catch (const std::runtime_error &e) {
    addError(SOP_MESSAGE, e.what());
  }

  return error();
}