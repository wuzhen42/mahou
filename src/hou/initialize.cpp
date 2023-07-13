#include <OP/OP_OperatorTable.h>
#include <UT/UT_DSOVersion.h>

#include "SOP_measure.h"
#include "SOP_sdf.h"

void newSopOperator(OP_OperatorTable *table) {
  table->addOperator(new OP_Operator("mahou_measure", "Mahou Measure", SOP_measure::myConstructor,
                                     SOP_measure::myTemplateList, 1, 1, 0));
  table->addOperator(
      new OP_Operator("mahou_sdf", "Mahou SDF", SOP_SDF::myConstructor, SOP_SDF::myTemplateList, 1, 1, 0));
}
