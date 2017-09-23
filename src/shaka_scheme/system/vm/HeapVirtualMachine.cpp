//
// Created by Billy Wooton on 9/21/17.
//
#include "HeapVirtualMachine.hpp"

namespace shaka {

void HeapVirtualMachine::evaluate_assembly_instruction() {
}

Accumulator HeapVirtualMachine::query_accumulator() const {
  return accumulator;
}

Expression HeapVirtualMachine::query_next_expression() const {
  return next_expression;
}

EnvPtr HeapVirtualMachine::get_current_environment() const {
  return env;
}

FramePtr HeapVirtualMachine::get_current_stack() const {
  return control_stack;
}

void HeapVirtualMachine::set_accumulator(Accumulator a) {
  this->accumulator = a;
}

void HeapVirtualMachine::set_next_expression(Expression x) {
  this->next_expression = x;
}

void HeapVirtualMachine::set_current_environment(EnvPtr e) {
  this->env = e;
}





} //namespace shaka