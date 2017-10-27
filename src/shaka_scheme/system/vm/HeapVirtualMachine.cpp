//
// Created by Billy Wooton on 10/2/17.
//

#include "shaka_scheme/system/vm/HeapVirtualMachine.hpp"
#include "shaka_scheme/system/core/lists.hpp"

namespace shaka {

HeapVirtualMachine::~HeapVirtualMachine() {}

void HeapVirtualMachine::evaluate_assembly_instruction() {
  shaka::DataPair& exp_pair = exp->get<DataPair>();
  shaka::Symbol& instruction = exp_pair.car()->get<Symbol>();

  // (halt)
  if (instruction == shaka::Symbol("halt")) {
    return;
  }

  // (refer var x)
  if (instruction == shaka::Symbol("refer")) {
    shaka::DataPair& exp_cdr = exp_pair.cdr()->get<DataPair>();
    shaka::Symbol& var = exp_cdr.car()->get<Symbol>();
    this->set_accumulator(env->get_value(var));

    DataPair& next_pair= exp_cdr.cdr()->get<DataPair>();
    NodePtr next_expression = next_pair.car();

    this->set_expression(next_expression);
  }

  // (constant obj x)
  if (instruction == shaka::Symbol("constant")) {
    shaka::DataPair& exp_cdr = exp_pair.cdr()->get<DataPair>();
    NodePtr obj = exp_cdr.car();

    this->set_accumulator(obj);

    DataPair& next_pair = exp_cdr.cdr()->get<DataPair>();

    NodePtr next_expression = next_pair.car();

    this->set_expression(next_expression);
  }

  // (close vars body x)
  if (instruction == shaka::Symbol("close")) {

    // Get the rest of the instruction
    shaka::DataPair& exp_cdr = exp_pair.cdr()->get<DataPair>();

    // Get the variable list for the closure object
    NodePtr vars_list = exp_cdr.car();

    // Put the variables into a std::vector

    std::vector<shaka::Symbol> vars;

    while (!core::is_null_list(vars_list)) {
      vars.push_back((vars_list->get<DataPair>().car()->get<Symbol>()));
      vars_list = vars_list->get<DataPair>().cdr();
    }

    // Get the body expression for the closure object
    NodePtr body = exp_cdr.cdr()->get<DataPair>().car();

    // Get the next assembly instruction
    NodePtr next_expression = exp_cdr.cdr()->
        get<DataPair>().cdr()->get<DataPair>().car();

    NodePtr closure = create_node(
        Closure(
            this->get_environment(),
            body,
            vars,
            nullptr,
            nullptr
        )
    );

    this->set_accumulator(closure);
    this->set_expression(next_expression);

  }


  // (test then else)
  if (instruction == shaka::Symbol("test")) {
    shaka::DataPair& exp_cdr = exp_pair.cdr()->get<DataPair>();
    NodePtr then_exp = exp_cdr.car();
    NodePtr else_exp = exp_cdr.cdr()->get<DataPair>().car();

    if (this->acc->get_type() == shaka::Data::Type::SYMBOL &&
        this->acc->get<Symbol>() == Symbol("#f")) {

      this->set_expression(else_exp);
    }

    else {
      this->set_expression(then_exp);
    }

  }

  // (assign var x)

  if (instruction == shaka::Symbol("assign")) {
    shaka::DataPair& exp_cdr = exp_pair.cdr()->get<DataPair>();
    shaka::Symbol& var = exp_cdr.car()->get<Symbol>();

    DataPair& next_pair = exp_cdr.cdr()->get<DataPair>();

    NodePtr expression = next_pair.car();

    this->env->set_value(var, this->acc);

    this->set_expression(expression);

  }

  // (conti x)

  if (instruction == shaka::Symbol("conti")) {

    // Create the function body for the continuation

    NodePtr call_frame = std::make_shared<Data>(*this->frame);
    NodePtr nuate = std::make_shared<Data>(Symbol("nuate"));
    NodePtr var = std::make_shared<Data>(Symbol("kont_v000"));

    // Create the variable list

    std::vector<Symbol> vars;
    vars.push_back(Symbol("kont_v000"));


    // Create the continuation

    NodePtr func_body = core::list(nuate, call_frame, var);
    EnvPtr empty_env = std::make_shared<Environment>(nullptr);
    NodePtr continuation =
        std::make_shared<Data>(
            Closure(
                empty_env,
                func_body,
                vars,
                nullptr,
                this->frame
            )
        );

    // Place the continuation in the accumulator

    this->set_accumulator(continuation);

    // Set the next expression register to x

    this->set_expression(exp_pair.cdr()->get<DataPair>().car());

  }

  // (frame x ret)

  if (instruction == shaka::Symbol("frame")) {
    shaka::DataPair& expr_cdr = exp_pair.cdr()->get<DataPair>();

    NodePtr x = expr_cdr.car();

    NodePtr ret = expr_cdr.cdr()->get<DataPair>().car();

    FramePtr new_frame =
        std::make_shared<CallFrame>(ret, this->env,
                                    this->rib, this->frame);
    this->frame = new_frame;
    this->set_expression(x);

    shaka::ValueRib vr;

    this->set_value_rib(vr);

  }

  // (argument x)

  if (instruction == shaka::Symbol("argument")) {
    shaka::DataPair& exp_cdr = exp_pair.cdr()->get<DataPair>();

    NodePtr x = exp_cdr.car();

    this->rib.push_back(this->acc);

    this->set_expression(x);

  }

  // (return)

  if (instruction == shaka::Symbol("return")) {
    this->set_expression(this->frame->get_next_expression());
    this->set_value_rib(this->frame->get_value_rib());
    this->set_environment(this->frame->get_environment_pointer());
    this->frame = this->frame->get_next_frame();

  }

}

Accumulator HeapVirtualMachine::get_accumulator() const {
  return this->acc;
}

Expression HeapVirtualMachine::get_expression() const {
  return this->exp;
}

EnvPtr HeapVirtualMachine::get_environment() const {
  return this->env;
}

ValueRib HeapVirtualMachine::get_value_rib() const {
  return this->rib;
}

FramePtr HeapVirtualMachine::get_call_frame() const {
  return this->frame;
}

void HeapVirtualMachine::set_accumulator(Accumulator a) {
  this->acc = a;
}

void HeapVirtualMachine::set_expression(Expression x) {
  this->exp = x;
}

void HeapVirtualMachine::push_call_frame(FramePtr s) {
  this->frame = s;
}

void HeapVirtualMachine::set_environment(EnvPtr e) {
  this->env = e;
}

void HeapVirtualMachine::set_value_rib(ValueRib r) {
  this->rib = r;
}


} //namespace shaka

