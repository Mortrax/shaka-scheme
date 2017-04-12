#ifndef SHAKA_PARSER_RULES_RULE_EXPRESSION_IMPL_H_
#define SHAKA_PARSER_RULES_RULE_EXPRESSION_IMPL_H_

#include <cctype>
#include <exception>
#include <stack>

#include "parser/primitives.h"
#include "parser/Tokenizer.h"

#include "parser/rule_quote_impl.h"
#include "parser/rule_define_impl.h"
#include "parser/rule_lambda_impl.h"

namespace shaka {
namespace parser {
namespace rule {

// <expression> ::=
//      <identifier>         |
//      <proc call>          |
//      <lambda expr>        |
//      <conditional>        |
//      <assignment>         |
//      <derived expression> |
//      <macro use>          |
//      <macro block>        |
//      <includer>           |
template <typename T>
bool expression (InputStream& in, NodePtr root, T& interm) {
    std::stack<shaka::Token> tokens;

    try {

        // First, I want to check for any rules that may be recursive
        // or that may include any of the smaller rules such as literal.
        //
        // Things that can start with '(':
        //      quotation
        // Things that must start with '(':
        //      procedure call: (<operator> ......
        //      lambda        : (lambda .......
        //      conditional   : (if ........
        //
        //  Required look ahead.
        if(in.peek().type == shaka::Token::Type::PAREN_START) {
          tokens.push(in.get());
          interm += tokens.top().get_string();

          if (in.peek().type == shaka::Token::Type::IDENTIFIER) {
            if (in.peek().get_string() == "define") define(in, root, interm);
            else if (in.peek().get_string() == "lambda") lambda(in, root, interm);
          }
        }

        // Pretty much can only be quotation
        else if(in.peek().type == shaka::Token::Type::QUOTE) {
          if(quote_literal(in, root, interm)) return true;
          else throw std::runtime_error("");
        }

        // Covers NUMBER
        // This turns in to a shaka::NUMBER data
        else if(in.peek().type == shaka::Token::Type::NUMBER) {
            tokens.push(in.get());
            interm += tokens.top().get_string();

            // Add to tree
            if(root != nullptr)
                root->push_child(shaka::Number(std::stod(tokens.top().get_string())));
        }

        // Covers.......
        //
        //      identifier
        //      boolean
        //      character
        //      string
        //
        // Place these directly as children of the root as shaka::SYMBOL
        else if(in.peek().type == shaka::Token::Type::IDENTIFIER    ||
                in.peek().type == shaka::Token::Type::BOOLEAN_TRUE  ||
                in.peek().type == shaka::Token::Type::BOOLEAN_FALSE ||
                in.peek().type == shaka::Token::Type::CHARACTER     ||
                in.peek().type == shaka::Token::Type::STRING) {

            tokens.push(in.get());
            interm += tokens.top().get_string();

            // Add to tree
            if(root != nullptr)
                root->push_child(shaka::Symbol(tokens.top().get_string()));

        }
        else throw std::runtime_error("EXPRESSION: Got to else, no conditions met");

        return true;

    } catch(std::runtime_error& e) {

        while(!tokens.empty()) {
            in.unget(tokens.top());
            tokens.pop();

        }

        if(root != nullptr) {
            std::size_t size = root->get_num_children();
            for(std::size_t i = 0; i < size; ++i) {
                root->remove_child(i);
            }
        }

        return false;
    }
}


} // rule
} // parser
} // shaka


#endif // SHAKA_PARSER_RULES_RULE_EXPRESSION_IMPL_H_