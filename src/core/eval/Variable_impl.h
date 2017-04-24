#ifndef SHAKA_CORE_EVAL_VARIABLE_IMPL_H
#define SHAKA_CORE_EVAL_VARIABLE_IMPL_H

#include "core/eval/Variable.h"

namespace shaka {
namespace eval {

NodePtr Variable::evaluate(NodePtr list, EnvPtr env) {
    // Check if the first item is a symbol
    if (list->is_symbol()) {
        // Get the key and the value, and store in the environment
        auto key = get<Symbol>(list->get_data());
        // If it doesn't contain a key, raise an exception.
        if (!env->is_defined(key)) {
            throw std::runtime_error(
                std::string("eval.Variable: variable `") +
                key.get_value() +
                std::string("` is undefined"));
            return nullptr;
        }
        return env->get_value(key);
    } else {
        // If the first item is not a symbol, raise an exception
        throw std::runtime_error("eval.Variable: argument is not a Symbol");
        return nullptr;
    }
}

} // namespace eval
} // namespace shaka


#endif // SHAKA_CORE_EVAL_VARIABLE_H
