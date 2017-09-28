//
// Created by Billy Wooton on 9/14/17.
//

#ifndef SHAKA_SCHEME_CONTROLFRAME_HPP
#define SHAKA_SCHEME_CONTROLFRAME_HPP

#include "../base/Data.hpp"
#include <vector>
namespace shaka {

class CallFrame {

public:


  /**
   * @brief Standard constructor for CallFrame to be used primarily by VM
   * @param ret The return address (next expression to evaluate)
   * @param env The immediate Environment frame referenced by this call frame
   * @param rib The list of arguments evaluated this far in this frame
   * @param next_frame The pointer to the next CallFrame in the ControlStack
   */
  CallFrame(Expression ret, EnvPtr env, ValueRib rib = nullptr, FramePtr
  next_frame = nullptr);

  /**
   * @brief Getter method for next_expression (return address)
   * @return The contents of the next expression field
   */
  Expression get_next_expression();


  /**
   * @brief Getter method for the active environment
   * @return The contents of the environment field (EnvPtr)
   */
  EnvPtr get_environment_pointer();

  /**
   * @brief Getter method for the value rib
   * @return The list of the arguments evaluated thus far in the CallFrame
   */
  ValueRib get_value_rib();


  /**
   * @brief Getter method for the pointer to the ControlStack
   * @return The contents of the next_frame field (rest of ControlStack)
   */
  FramePtr get_next_frame();

  /**
   * @brief Setter method for the expression field (return address)
   * @param exp The Expression pointer to replace the current exp field
   */
  void set_next_expression(Expression exp);

  /**
   * @brief Setter method for the environment field
   * @param env The Environment pointer to replace the current env field
   */
  void set_environment_pointer(EnvPtr env);


  /**
   * @brief Setter method for the current value rib
   * @param rib The new ValueRib pointer to replace the current rib field
   */
  void set_value_rib(ValueRib rib);


  /**
   * @brief Setter method for the next_frame pointer
   * @param next_frame The FramePtr to replace the current next_frame
   */
  void set_next_frame(FramePtr next_frame);

private:

  using EnvPtr = std::shared_ptr<Environment>;
  using Expression = std::shared_ptr<Data>;
  using ValueRib = std::vector<NodePtr>;
  using FramePtr = std::shared_ptr<CallFrame>;

  Expression ret;
  EnvPtr env;
  ValueRib rib;
  FramePtr next_frame;


};

} // namespace shaka
#endif //SHAKA_SCHEME_CONTROLFRAME_HPP
