//
// Created by Billy Wooton on 9/14/17.
//

#ifndef SHAKA_SCHEME_CONTROLFRAME_HPP
#define SHAKA_SCHEME_CONTROLFRAME_HPP

#include "../base/Data.hpp"
namespace shaka {

class CallFrame {

public:

  /**
   * @brief destructor for CallFrame
   */
  ~CallFrame();

  /**
   * @brief copy constructor for CallFrame
   * @param other the CallFrame object to construct this object from
   */
  CallFrame(const CallFrame& other);

  /**
   * @brief standard constructor for CallFrame to be used primarily by VM
   * @param ret The return address (next expression to evaluate)
   * @param env The immediate Environment frame referenced by this call frame
   * @param rib The list of arguments evaluated this far in this frame
   * @param next_frame The pointer to the next CallFrame in the ControlStack
   */
  CallFrame(Expression ret, EnvPtr env, ValueRib rib = nullptr, FramePtr
  next_frame = nullptr);

  /**
   * @brief getter method for next_expression (return address)
   * @return the contents of the next expression field
   */
  Expression get_next_expression();


  /**
   * @brief getter method for the active environment
   * @return the contents of the environment field (EnvPtr)
   */
  EnvPtr get_environment_pointer();

  /**
   * @brief getter method for the value rib
   * @return the list of the arugments evaluated thus far in the CallFrame
   */
  ValueRib get_value_rib();


  /**
   * @brief getter method for the pointer to the ControlStack
   * @note The ControlStack is simply a singly linked list of all of the
   * - CallFrames
   * @return the contents of the next_frame field (rest of ControlStack)
   */
  FramePtr get_next_frame();

  /**
   * @brief setter method for the expression field (return address)
   * @note of potential utility in debugging APIs
   * @param exp The Expression pointer to replace the current exp field
   */
  void set_next_expression(Expression exp);

  /**
   * @brief setter method for the environment field
   * @note of potential utility in debugging APIs
   * @param env The Environment pointer to replace the current env field
   */
  void set_environment_pointer(EnvPtr env);


  /**
   * @brief setter method for the current value rib
   * @note of potential utility in debugging APIs
   * @param rib The new ValueRib pointer to replace the current rib field
   */
  void set_value_rib(ValueRib rib);


  /**
   * @brief setter method for the next_frame pointer
   * @param next_frame The FramePtr to replace the current next_frame
   */
  void set_next_frame(FramePtr next_frame);

private:

  using EnvPtr = std::shared_ptr<Environment>;
  using Expression = std::shared_ptr<DataNode>;
  using ValueRib = std::shared_ptr<DataNode>;
  using FramePtr = std::shared_ptr<CallFrame>;

  Expression ret;
  EnvPtr env;
  ValueRib rib;
  FramePtr next_frame;


};

} // namespace shaka
#endif //SHAKA_SCHEME_CONTROLFRAME_HPP
