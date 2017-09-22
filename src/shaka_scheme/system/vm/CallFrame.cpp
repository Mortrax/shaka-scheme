//
// Created by Billy Wooton on 9/18/17.
//

#include "CallFrame.hpp"

shaka::CallFrame::CallFrame(Expression ret,
                            EnvPtr env,
                            ValueRib rib = nullptr,
                            FramePtr next_frame = nullptr) :
  ret(ret),
  env(env),
  rib(rib),
  next_frame(next_frame) {}


Expression shaka::CallFrame::get_next_expression() {
  return ret;
}

EnvPtr shaka::CallFrame::get_environment_pointer() {
  return env;
}

ValueRib shaka::CallFrame::get_value_rib() {
  return rib;
}

FramePtr shaka::CallFrame::get_next_frame() {
  return next_frame;
}

void shaka::CallFrame::set_next_expression(Expression exp) {
  this->ret = exp;
}

void shaka::CallFrame::set_environment_pointer(EnvPtr env) {
  this->env = env;
}

void shaka::CallFrame::set_next_frame(FramePtr next_frame) {
  this->next_frame = next_frame;
}

void shaka::CallFrame::set_value_rib(ValueRib rib) {
  this->rib = rib;
}




