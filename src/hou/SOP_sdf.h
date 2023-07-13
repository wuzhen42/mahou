#pragma once

#include <SOP/SOP_Node.h>

class SOP_SDF : public SOP_Node {
public:
  ~SOP_SDF() override = default;

  static PRM_Template myTemplateList[];

  static OP_Node *myConstructor(OP_Network *net, const char *name, OP_Operator *op) {
    return new SOP_SDF(net, name, op);
  }

protected:
  OP_ERROR cookMySop(OP_Context &context) override;

private:
  SOP_SDF(OP_Network *net, const char *name, OP_Operator *op) : SOP_Node(net, name, op) {}
};
