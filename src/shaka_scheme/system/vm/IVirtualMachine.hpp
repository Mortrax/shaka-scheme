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

using Accumulator = std::shared_ptr<DataNode>;
using NextExpression = std::shared_ptr<DataNode>;
using CurrentEnvironment = std::shared_ptr<Environment>;
using CurrentValueRib = std::shared_ptr<DataNode>;
using CurrentStack = std::shared_ptr<CallFrame>;

/**
 * @brief The interface for the VirtualMachine. Lays out the specification
 * for the Heap Based VM described in Kent Dybvigs Dissertation
 *
 */
class IVirtualMachine {
public:

  virtual ~IVirtualMachine() {}

  /**
   * @brief The method that actually processes the 12 assembly instructions
   * Takes in an Evaluator that iteratively queries the contents of the
   * NextExpression and modifies each register accordingly based on the spec.
   * Changes the contents of each register in place
   * @param s An R-value reference to an Evaluator
   */

  virtual void evaluate_assembly_instruction(NextExpression x) = 0;

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
  virtual NextExpression query_next_expression() const = 0;

  /**
   * @brief Returns the contents of the CurrentEnvironment register
   * Should hold the current relevant Environment frame contingent upon
   * the contents of the NextExpression register
   * @return The pointer to the CurrentEnvironment frame
   */
  virtual CurrentEnvironment get_current_environment() const = 0;

  /**
   * @brief Returns the contents of the CurrentValueRib register
   * Should hold the list of all arguments that have been evaluated thus far
   * The rib can be used to extend the environment upon creation of a new frame
   * @return The list of arguments evaluated thus far
   */
  virtual CurrentValueRib get_current_value_rib() const = 0;

  /**
   * @brief Returns the contents of the CurrentStack register
   * Should hold a pointer to the top CallFrame on the stack
   * @return The pointer to the top CallFrame on the stack
   */
  virtual CurrentStack get_current_stack() const = 0;


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
  virtual void set_next_expression(NextExpression x) = 0;

  /**
   * @brief Creates a new frame from the contents of the CurrentEnvironment
   * register, the CurrentVaueRib register, and the ret parameter as the
   * NextExpression. Places that frame on the current stack
   * Is a result of the (frame x ret) Assembly instruction
   * @param ret The NextExpression to be placed in the new CallFrame object
   */
  virtual void add_frame_to_stack(NextExpression ret) = 0;

  /**
   * @brief Restores parameter s to be the CurrentStack
   * @param s The pointer to a CallFrame to represent the new CurrentStack
   */
  virtual void restore_stack(CurrentStack s) = 0;



private:

  Accumulator a;
  NextExpression x;
  CurrentEnvironment e;
  CurrentValueRib r;
  CurrentStack s;

};


} //namespace shaka

#endif //SHAKA_SCHEME_IVIRTUALMACHINE_HPP
