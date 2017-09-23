//
// Created by Billy Wooton on 9/3/17.
//

#ifndef SHAKA_SCHEME_IVIRTUALMACHINE_HPP
#define SHAKA_SCHEME_IVIRTUALMACHINE_HPP

#include "../base/Data.hpp"

namespace shaka {

/**
 * @note Forward declaration of CallFrame class that will need to
 * be implemented to augment the design of the VM
 */

class CallFrame;

/**
 * @note These are type aliases to assist in maintaining uniformity
 * of VM design in preparation for later design changes to other parts
 * of the system. These aliases can be changed later without the
 * entire code base for the VM needing to be modified
 */

using Accumulator = std::shared_ptr<Data>;
using Expression = std::shared_ptr<Data>;
using EnvPtr = std::shared_ptr<Environment>;
using ValueRib = std::shared_ptr<Data>;
using FramePtr = std::shared_ptr<CallFrame>;


/**
 * @brief The interface for the VirtualMachine. Lays out the specification
 * for the Heap Based VM described in Kent Dybvig's Dissertation
 *
 */
class IVirtualMachine {
public:

  /**
   * @brief The method that actually processes the 12 assembly instructions
   * Iteratively evaluates whatever happens to be the current contents of
   * the NextExpression register, until a (halt) instruction is reached
   * Changes the contents of each register in place
   */

  virtual void evaluate_assembly_instruction() = 0;

  /**
   * @brief Returns the current contents of the Accumulator register
   * Useful for debugging and potentially for the overall functionality
   * of the VM itself
   * @return The last value to be computed by evaluation
   */
  virtual Accumulator query_accumulator() const = 0;

  /**
   * @brief Returns the current contents of the NextExpression register
   * Should be one of the 12 assembly instructions assuming we are performing
   * compilation prior to evaluation
   * @return The next expression to be evaluated
   */
  virtual Expression query_next_expression() const = 0;

  /**
   * @brief Returns the contents of the CurrentEnvironment register
   * Should hold the current relevant Environment frame contingent upon
   * the contents of the NextExpression register
   * @return The pointer to the CurrentEnvironment frame
   */
  virtual EnvPtr get_current_environment() const = 0;

  /**
   * @brief Returns the contents of the CurrentValueRib register
   * Should hold the list of all arguments that have been evaluated thus far
   * The rib can be used to extend the environment upon creation of a new frame
   * @return The list of arguments evaluated thus far
   */
  virtual ValueRib get_current_value_rib() const = 0;

  /**
   * @brief Returns the contents of the CurrentStack register
   * Should hold a pointer to the top CallFrame on the stack
   * @return The pointer to the top CallFrame on the stack
   */
  virtual FramePtr get_current_stack() const = 0;


  /**
   * @brief Sets the value of the Accumulator register to be the value
   * passed in; is a result of many of the Assembly instructions
   * @param a The new value to be placed in the Accumulator
   */
  virtual void set_accumulator(Accumulator a) = 0;

  /**
   * @brief Sets the value of the NextExpression register to be the
   * value passed in; is a result of most of the Assembly instructions
   * @param x The new contents of the NextExpression register
   */
  virtual void set_next_expression(Expression x) = 0;

  /**
   * @brief Sets the contents of the CurrentEnvironment register to be
   * the value passed in
   * @param e The new EnvPtr to be placed in the register
   */
  virtual void set_current_environment(EnvPtr e) = 0;

  /**
   * @brief Sets the contents of the CurrentValueRib register to be
   * the value passed in
   * @param r The new ValueRib to be placed in the register
   */
  virtual void set_current_value_rib(ValueRib r) = 0;

  /**
   * @brief Restores parameter s to be the CurrentStack
   * @param s The pointer to a CallFrame to represent the new CurrentStack
   */
  virtual void restore_stack(FramePtr s) = 0;

};


} //namespace shaka

#endif //SHAKA_SCHEME_IVIRTUALMACHINE_HPP
