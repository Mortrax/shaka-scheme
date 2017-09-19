//
// Created by Billy Wooton on 9/18/17.
//

#include "CallFrame.hpp"

namespace shaka {

using FramePtr = std::shared_ptr<CallFrame>;

FramePtr create_frame(CallFrame frame) {
  return std::make_shared<CallFrame>(frame);
}

} // namespace shaka

shaka::CallFrame::CallFrame(Expression ret,
                            EnvPtr env,
                            ValueRib rib = nullptr,
                            FramePtr next_frame = nullptr) :


