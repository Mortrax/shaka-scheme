//
// Created by Billy Wooton on 9/8/17.
//

#ifndef SHAKA_SCHEME_HEAPVIRTUALMACHINE_HPP
#define SHAKA_SCHEME_HEAPVIRTUALMACHINE_HPP

#include "IVirtualMachine.hpp"
#include "CallFrame.hpp"

namespace shaka {

class HeapVirtualMachine : public IVirtualMachine {

  HeapVirtualMachine(
      Accumulator a,
      Expression x,
      EnvPtr e,
      ValueRib r,
      FramePtr s);

  virtual void evaluate_assembly_instruction() override;

  virtual Accumulator query_accumulator() const override;

  virtual Expression query_next_expression() const override;

  virtual EnvPtr get_current_environment() const override;

  virtual FramePtr get_current_stack() const override;

  virtual void set_accumulator(Accumulator a) override;

  virtual void set_next_expression(Expression x) override;

  virtual void set_current_environment(EnvPtr e) override;

  virtual void set_current_value_rib(ValueRib r) override;

  virtual void restore_stack(FramePtr s) override;


private:
  Accumulator accumulator;
  Expression next_expression;
  EnvPtr env;
  ValueRib current_value_rib;
  FramePtr control_stack;
};

}// namespace shaka
#endif //SHAKA_SCHEME_HEAPVIRTUALMACHINE_HPP
