#include "shaka_scheme/system/parser/parser_definitions.hpp"

namespace shaka {
namespace parser {

ParserInput::ParserInput(std::string str, std::string origin) :
    lex(str, origin) {}

void ParserInput::append_input(std::string str) {
  lex.append_input(str);
}

lexer::LexResult ParserInput::get() {
  if (tokens.empty()) {
    return lexer::rules::scheme_lexer(lex);
  } else {
    auto token = tokens.front();
    tokens.pop_front();
    return token;
  }
}

lexer::LexResult ParserInput::peek() {
  if (tokens.empty()) {
    auto token = lexer::rules::scheme_lexer(lex);
    if (!token.is_token()) {
      return token;
    }
    tokens.push_back(token);
    return tokens.front();
  } else {
    return tokens.front();
  }
}

void ParserInput::unget(lexer::LexResult token) {
  tokens.push_front(token);
}

ParserResult::ParserResult(std::string type, NodePtr it) :
    type(type), it(it), lex_result(lexer::Error("", lexer::LexInfo(),
                                                "ParserResult-good")) {}

ParserResult::ParserResult(std::string type,
                           NodePtr it,
                           lexer::LexResult result) :
    type(type), it(it), lex_result(result) {}

bool ParserResult::is_valid() const { return type == "valid"; }
bool ParserResult::is_lexer_error() const { return type == "lexer-error"; }
bool ParserResult::is_parser_error() const { return type == "parser-error"; }
bool ParserResult::is_incomplete() const { return type == "incomplete"; }
bool ParserResult::is_complete() const { return type == "complete"; }

std::ostream& operator<<(std::ostream& left, ParserResult right) {
  left << "ParserResult("
       << "type:" << right.type << " | "
       << "it:";
  if (right.it) {
    left << *right.it << " | ";
  } else {
    left << "<nullptr>" << " | ";
  }
  left << "lex_result:" << right.lex_result << ")";
  return left;
}

ParserResult Valid(lexer::LexResult result) {
  return ParserResult("valid", nullptr, result);
}

ParserResult Complete(NodePtr node) {
  return ParserResult("complete", node);
}

ParserResult LexerError(lexer::LexResult result) {
  return ParserResult("lexer-error", nullptr, result);
}

ParserResult ParserError(lexer::LexResult result, std::string str) {
  return ParserResult("parser-error", create_node(String(str)), result);
}

ParserResult Incomplete(lexer::LexResult result) {
  return ParserResult("incomplete", nullptr, result);
}

using ParserRule = std::function<ParserResult(ParserInput&)>;
using DataConstructor = std::function<NodePtr(ParserResult)>;

ParserResult parse_simple(ParserInput& in) {
  auto next = in.peek();
  if (next.token_type == "string") {
    in.get();
    return Complete(create_node(String(next.str)));
  } else if (next.token_type == "identifier") {
    in.get();
    return Complete(create_node(Symbol(next.str)));
  } else if (next.token_type == "boolean-true") {
    in.get();
    return Complete(create_node(Boolean(true)));
  } else if (next.token_type == "boolean-false") {
    in.get();
    return Complete(create_node(Boolean(false)));
  } else {
    return ParserError(in.peek(), "could not match to simple datum");
  }
}


ParserResult parse_list(ParserInput& in) {
  //std::cout << "parse-list" << std::endl;
  auto data_list = core::list();
  //std::cout << "TOKEN: " << in.peek() << std::endl;
  if (in.peek().token_type == "paren-left") {
    in.get();
  }
  while (in.peek().token_type != "paren-right") {
    //std::cout << "TOKEN: " << in.peek() << std::endl;
    ParserResult datum_result = parse_datum(in);
    //std::cout << "DATUM:" << datum_result << std::endl;
    if (datum_result.is_complete()) {
      data_list = core::append(data_list, core::list(datum_result.it));
    } else if (in.peek().token_type == "dot") {
      in.get();
      //std::cout << "in.peek(): " << in.peek() << std::endl;
      datum_result = parse_datum(in);
      //std::cout << "IMPROPER LIST LAST DATUM: " << datum_result <<
      // std::endl;
      if (datum_result.is_complete()) {
        data_list = core::append(data_list, datum_result.it);
      } else {
        return ParserError(datum_result.lex_result, "could not match to "
            "last datum for improper list");
      }
    } else {
      return datum_result;
    }
  }
  if (in.peek().token_type == "paren-right") {
    in.get();
    return Complete(data_list);
  } else {
    return ParserError(in.peek(),
                       "could not match to closing parens for list");
  }
}


ParserResult parse_datum(ParserInput& in) {
  while (in.peek().token_type == "comment") {
    in.get();
  }

  if (in.peek().token_type == "datum-comment") {
    in.get();
    auto parsed_datum = parse_datum(in);
    if (!parsed_datum.is_complete()) {
      return parsed_datum;
    }
  }

  if (in.peek().token_type == "quote") {
    auto saved_token = in.get();
    auto parsed_datum = parse_datum(in);
    if (!parsed_datum.is_complete()) {
      in.unget(saved_token);
      return parsed_datum;
    } else {
      return Complete(core::list(
          create_node(Symbol("quote")),
          parsed_datum.it));
    }
  }

  auto simple_datum = parse_simple(in);
  if (simple_datum.is_complete()) {
    return simple_datum;
  } else {
    auto next = in.peek();
    if (next.token_type == "paren-left") {
      return parse_list(in);
    } else {
      return ParserError(next, "could not parse non-simple datum");
    }
  }
}

} // namespace parser
} // namespace shaka
