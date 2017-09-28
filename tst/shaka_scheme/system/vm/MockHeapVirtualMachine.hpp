//
// Created by Billy Wooton on 9/22/17.
//

#ifndef SHAKA_SCHEME_MOCKHEAPVIRTUALMACHINE_HPP
#define SHAKA_SCHEME_MOCKHEAPVIRTUALMACHINE_HPP

#include <shaka_scheme/system/base/Data.hpp>

class MockHeapVirtualMachine {
  using namespace shaka;

  GMOCK_METHOD1_(step, shaka::NodePtr(shaka::NodePtr));
  GMOCK_METH
};

#endif //SHAKA_SCHEME_MOCKHEAPVIRTUALMACHINE_HPP
