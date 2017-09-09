//
// Created by Billy Wooton on 9/8/17.
//

#ifndef SHAKA_SCHEME_HEAPVIRTUALMACHINE_HPP
#define SHAKA_SCHEME_HEAPVIRTUALMACHINE_HPP

#include "IVirtualMachine.hpp"

namespace shaka {

class HeapVirtualMachine : public IVirtualMachine {

  HeapVirtualMachine(
      Accumulator a,
      NextExpression x,
      CurrentEnvironment e,
      CurrentValueRib r,
      CurrentStack s) : a(a), x(x), e(e), r(r), s(s) {}

  virtual ~HeapVirtualMachine() override {}

  virtual void evaluate_assembly_instruction() override {}

  virtual Accumulator query_accumulator() const override {
    return a;
  }






private:
  Accumulator a;
  NextExpression x;
  CurrentEnvironment e;
  CurrentValueRib r;
  CurrentStack s;
};

}// namespace shaka
#endif //SHAKA_SCHEME_HEAPVIRTUALMACHINE_HPP
