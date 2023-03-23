#pragma once

#include <SOP/SOP_Node.h>

class SOP_measure : public SOP_Node {
public:
  ~SOP_measure() override;

  static PRM_Template myTemplateList[];

  static OP_Node *myConstructor(OP_Network *net, const char *name, OP_Operator *op) {
    return new SOP_measure(net, name, op);
  }

protected:
  OP_ERROR cookMySop(OP_Context &context) override;

private:
  SOP_measure(OP_Network *net, const char *name, OP_Operator *op) : SOP_Node(net, name, op) {}
};
