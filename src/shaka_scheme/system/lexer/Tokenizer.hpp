#ifndef SHAKA_LEXER_TOKENIZER_H
#define SHAKA_LEXER_TOKENIZER_H

#include "shaka_scheme/system/lexer/Token.hpp"
#include "shaka_scheme/system/exceptions/TokenizerException.hpp"

#include <iostream>
#include <string>
#include <deque>

#ifndef DEBUG_PRINT
#define DEBUG_PRINT 0
#else
#define DEBUG_PRINT 1
#endif

namespace shaka {

/**
 * @brief The monolithic lexer class that implements most of the context-free
 * grammar and token generation for Scheme.
 *
 * @note The Tokenizer class is designed to take input from an std::istream&,
 * such as std::cin. It is also able to handle backtracking, as it stores
 * Tokens on an internal deque that allows for queueing and reinserting back
 * onto the front, as well as popping Tokens from both ends.
 *
 * @implementation_specific Shaka Scheme relaxes the #!<directive> rule as
 * described in R7RS in order to allow for the #!quit directive to be used
 * to exit from the REPL.
 *
 * @throws TokenizerException upon any rule mismatches or errors. It is
 * expected that code above it will catch it, and then deal with it accordingly.
 *
 * @todo Finish the documentation for Tokenizer's methods and rules, or
 * re-factor the class into a more clean, modular design (idea: parser
 * combinators, although error handling will be more difficult).
 */
class Tokenizer {
private:
  /**
   * @brief The input stream reference for the lexer. The lexer does not own
   * error handling and state resetting for the stream upon error.
   */
  std::istream& in;

  /**
   * @brief The internal double-ended queue for Tokens.
   */
  std::deque<Token> tokens;
public:
  /**
   * @brief Constructs the Tokenizer.
   * @param in The stream to take input from.
   */
  Tokenizer(std::istream& in) :
      in(in) {}

  /**
   * @brief Returns the mutable reference to the internal std::istream.
   * @return The istream reference.
   */
  std::istream& get_istream() {
    return this->in;
  }

  /**
   * @brief Gets the internal deque of tokens.
   * @return The deque of Tokens.
   */
  std::deque<Token>& get_tokens() {
    return this->tokens;
  }

  /**
   * @brief Returns the cached, unread input Token, or consumes input from
   * the input stream until it can complete a Token.
   * @return The token that is immediately read. It is not put onto the queue.
   */
  Token get() {
    if (!tokens.empty()) {
      auto front = tokens.front();
      tokens.pop_front();
      return front;
    } else {
      try {
        this->read_next_token();
      } catch (shaka::TokenizerException e) {
        std::cerr << e.what() << std::endl;
        throw e;
      }
      auto front = tokens.front();
      tokens.pop_front();
      return front;
    }
  }

  /**
   * @brief Reads in input until a Token is generated, and then enqueued onto
   * the internal Token double-ended queue.
   * @return The token that is at the front of the Token deque.
   */
  Token peek() {
    if (!tokens.empty()) {
      auto front = tokens.front();
      return front;
    } else {
      try {
        this->read_next_token();
      } catch (shaka::TokenizerException e) {
        std::cerr << e.what() << std::endl;
        throw e;
      }
      auto front = tokens.front();
      return front;
    }
  }

  /**
   * @brief Places a Token back onto the front of the internal Token deque.
   * @param t The token to requeue onto the front of the Token double-ended
   * queue.
   */
  void unget(Token t) {
    tokens.push_front(t);
  }

  /**
   * @brief Parses the next token, and pushes it onto the back of the deque.
   */
  void read_next_token() {
    Token t = parse_token();
    if (DEBUG_PRINT) {
      std::cerr << "Parsed: " << static_cast<int>(t.type)
                << " :: " << t.str << std::endl;
    }
    tokens.push_back(t);
  }

  /**
   * @brief Parses a left parenthesis.
   * @return The token representing the left parenthesis.
   * @throws TokenizerException upon the next character not being '('.
   */
  Token parse_paren_start() {
    // Begin parenthesis
    if (in.peek() == '(') {
      in.get();
      return Token(Token::Type::PAREN_START, "(");
    } else {
      throw shaka::TokenizerException(20000, "Could not parse Token"
          ".PAREN_START");
    }
  }

  /**
   * @brief Parses a right parenthesis.
   * @return The token representing the right parenthesis.
   * @throws TokenizerException upon the next character not being ')'.
   */
  Token parse_paren_end() {
    if (in.peek() == ')') {
      in.get();
      return Token(Token::Type::PAREN_END, ")");
    } else {
      throw shaka::TokenizerException(20001, "Could not parse Token.PAREN_END");
    }
  }

  /**
   * @brief Parses a string element, complete with possible string escapes.
   * @param str The string to parse.
   * @throws TokenizerException upon an invalid character escape sequence.
   */
  void parse_string_element(std::string& str) {
    if (in.peek() == '\\') {
      in.get();
      // what type of escape?
      if (std::isalpha(in.peek())) {
        // Any of the mnemonic escapes?
        if (in.peek() == 'a') {
          in.get();
          str += '\a';
          return;
        } else if (in.peek() == 'b') {
          in.get();
          str += '\b';
          return;
        } else if (in.peek() == 't') {
          in.get();
          str += '\t';
          return;
        } else if (in.peek() == 'n') {
          in.get();
          str += '\n';
          return;
        } else if (in.peek() == 'r') {
          in.get();
          str += '\r';
          return;

          // If the first letter is 'x', then possibly hex digit
        } else if (in.peek() == 'x') {
          in.get();

          // If the next character is a hex digit,
          // then go into hex scalar value.
          if (is_hex_digit(in.peek())) {
            Token result(Token::Type::INVALID);
            if (parse_hex_scalar_value_character(result)) {
              str += result.str;
              return;
            } else {
              str += "x";
            }

            // Otherwise, it's just a literal 'x'
          } else {
            str += "x";
            return;
          }


          // Otherwise, it's going to either be a single character escape
          // or a named character escape.
        } else {
          if (DEBUG_PRINT) {
            std::cerr << "in single/named char escape" << std::endl;
          }
          std::string buffer;
          while (!is_delimiter(in.peek()) && in.peek() != EOF) {
            buffer += in.get();
          }
          if (DEBUG_PRINT) {
            std::cerr << "buffer: " << buffer << std::endl;
            std::cerr << "buffer.size(): " << buffer.size() << std::endl;
          }
          // If escape for whitespace, read in whitespace and then
          // continue reading string elements.
          if (std::isspace(in.peek()) && buffer.size() == 0) {
            while (std::isspace(in.peek())) {
              in.get();
            }
            return;

            // named character escape?
          } else if (buffer == "alarm") {
            str += "\a";
            return;
          } else if (buffer == "backspace") {
            str += "\b";
            return;
          } else if (buffer == "delete") {
            str += "\x7F";
            return;
          } else if (buffer == "escape") {
            str += "\x1B";
            return;
          } else if (buffer == "newline") {
            str += "\n";
            return;
          } else if (buffer == "null") {
            str += "\0";
            return;
          } else if (buffer == "return") {
            str += "\r";
            return;
          } else if (buffer == "space") {
            str += " ";
            return;
          } else if (buffer == "tab") {
            str += "\t";
            return;

            // Otherwise, error!
          } else {
            throw shaka::TokenizerException(20002, "Tokenizer.parse_token: Bad "
                "character escape");
            return;
          }
        } // character escape
        // Else, literal " or etc.
      } else if (in.peek() == '\"') {
        in.get();
        str += "\"";
        return;
      } else {
        str += in.peek();
        in.get();
      }
    } else {
      str += in.peek();
      in.get();
    }
  }

  /**
   * @brief Parses in a string token, as well as it delimiting double quotes.
   * @return A Token with the correct Token::Type::STRING and std::string
   * contents.
   * @throws TokenizerException if the next character from input at the start
   * of this procedure is not an ASCII double quote.
   */
  Token parse_string() {
    // String
    if (in.peek() == '\"') {
      in.get();
      // Read in the rest of the string.
      std::string buffer;
      while (in.peek() != '\"') {
        parse_string_element(buffer);
      }
      in.get();
      return Token(Token::Type::STRING, buffer);
    } else {
      throw shaka::TokenizerException(20003, "Could not parse Token.STRING");
    }
  }

  /**
   * @brief Parses the rest of a line comment.
   *
   * Assumes that the first ';' has been parsed already.
   */
  void parse_line_comment() {
    if (DEBUG_PRINT) {
      std::cout << "What LINE COMMENT is this? \'" <<
                in.peek() << "\'" << std::endl;
    }
    // If it's a comment, read until the
    // end of the lie.
    while (in.peek() != '\n') {
      in.get();
    }
    in.get();
  }

  /**
   * @brief Parses a single hexidecimal scalar value character.
   * @param result The Token to write the valid result into.
   * @return Whether the parse happened successfully (always true in
   * practice, as an exception is thrown upon error).
   * @throws TokenizerExcpetion if the hex scalar value character was invalid.
   */
  bool parse_hex_scalar_value_character(Token& result) {
    if (is_hex_digit(in.peek())) {
      std::string buffer;
      while (is_hex_digit(in.peek())) {
        buffer += in.get();
      }
      int i = std::stoi(buffer, 0, 16);
      buffer.clear();
      buffer += static_cast<char>(i);
      result = Token(Token::Type::CHARACTER, buffer);
      return true;
    } else {
      throw shaka::TokenizerException(20004,
                                      "Could not parse CHARACTER.hex_scalar_value");
    }
  }

  bool parse_bytevector_byte(std::string& str) {
    if (std::isdigit(in.peek())) {
      std::string buffer;
      while (std::isdigit(in.peek())) {
        buffer += in.get();
      }
      int num = std::stoi(buffer);
      if (num >= 0 && num <= 255) {
        str += buffer;
        return true;
      } else {
        if (DEBUG_PRINT) {
          std::cerr << "Tokenizer.parse_bytevector_byte: Not in range [0, 255]"
                    << std::endl;
        }
        throw shaka::TokenizerException(20005,
                                        "Tokenizer.parse_bytevector_byte: Not in range [0, 255]");
        return false;
      }

    } else {
      if (DEBUG_PRINT) {
        std::cerr << "Tokenizer.parse_bytevector_byte: Not a digit"
                  << std::endl;
      }
      throw shaka::TokenizerException(20006,
                                      "Tokenizer.parse_bytevector_byte: Not a digit");
      return false;
    }
  }

  bool rule_hash(Token& result) {
    if (in.peek() == '#') {
      in.get();

      // vector start token
      if (in.peek() == '(') {
        in.get();
        result = Token(Token::Type::VECTOR_START, "#(");
        return true;

        // <bytevector> ==> #u8( <byte>* )
        // Just read in the start token.
      } else if (in.peek() == 'u') {
        in.get();
        if (in.peek() == '8') {
          in.get();
          if (in.peek() == '(') {
            in.get();
            result = Token(Token::Type::BYTEVECTOR_START, "#u8(");
            return true;
          }
        } else {
          if (DEBUG_PRINT) {
            std::cerr << "bytevector: failed to read in 8 after u" << std::endl;
          }
          throw shaka::TokenizerException(20007,
                                          "Tokenizer.rule_hash: bytevector invalid prefix");
          return false;
        }

        // <character>
      } else if (in.peek() == '\\') {
        in.get();
        if (DEBUG_PRINT) {
          std::cerr << "in character" << std::endl;
        }

        // what type of escape?
        if (std::isalpha(in.peek())) {
          // If the first letter is 'x', then possibly hex digit
          if (in.peek() == 'x') {
            in.get();

            // If the next character is a hex digit,
            // then go into hex scalar value.
            if (is_hex_digit(in.peek())) {
              return parse_hex_scalar_value_character(result);

              // Otherwise, it's just a literal 'x'
            } else {
              result = Token(Token::Type::CHARACTER, "x");
              return true;
            }

            // Otherwise, it's going to either be a single character escape
            // or a named character escape.
          } else {
            if (DEBUG_PRINT) {
              std::cerr << "in single/named char escape" << std::endl;
            }
            std::string buffer;
            while (!is_delimiter(in.peek()) && in.peek() != EOF) {
              buffer += in.get();
            }
            if (DEBUG_PRINT) {
              std::cerr << "buffer: " << buffer << std::endl;
              std::cerr << "buffer.size(): " << buffer.size() << std::endl;
            }
            // single character escape?
            if (buffer.size() == 1) {
              result = Token(Token::Type::CHARACTER, buffer);

              // named character escape?
            } else if (buffer == "alarm") {
              result = Token(Token::Type::CHARACTER, "\a");
              return true;
            } else if (buffer == "backspace") {
              result = Token(Token::Type::CHARACTER, "\b");
              return true;
            } else if (buffer == "delete") {
              result = Token(Token::Type::CHARACTER, "\x7F");
              return true;
            } else if (buffer == "escape") {
              result = Token(Token::Type::CHARACTER, "\x1B");
              return true;
            } else if (buffer == "newline") {
              result = Token(Token::Type::CHARACTER, "\n");
              return true;
            } else if (buffer == "null") {
              result = Token(Token::Type::CHARACTER, "\0");
              return true;
            } else if (buffer == "return") {
              result = Token(Token::Type::CHARACTER, "\r");
              return true;
            } else if (buffer == "space") {
              result = Token(Token::Type::CHARACTER, " ");
              return true;
            } else if (buffer == "tab") {
              result = Token(Token::Type::CHARACTER, "\t");
              return true;

              // Otherwise, error!
            } else {
              throw "Tokenizer.parse_token: Bad character escape";
              result = Token(Token::Type::END_OF_FILE);
              return false;
            }
          } // character escape
        }

        // <boolean> #t or #true?
      } else if (in.peek() == 't') {

        std::string buffer;
        while (std::isalpha(in.peek())) {
          buffer += in.get();
        }
        if (buffer == "true" || buffer == "t") {
          result = Token(Token::Type::BOOLEAN_TRUE, "#t");
          return true;
        } else {
          throw shaka::TokenizerException(20008, "Tokenizer.parse_token: "
              "invalid hash identifier/boolean does not match to true");
          result = Token(Token::Type::END_OF_FILE);
          return false;
        }
        // <boolean> #f or #false?
      } else if (in.peek() == 'f') {
        std::string buffer;
        while (std::isalpha(in.peek())) {
          buffer += in.get();
        }
        if (buffer == "false" || buffer == "f") {
          result = Token(Token::Type::BOOLEAN_FALSE, "#f");
          return true;
        } else {
          throw shaka::TokenizerException(20009, "Tokenizer.parse_token: "
              "invalid hash identifier/boolean does not match to true");
          result = Token(Token::Type::END_OF_FILE);
          return false;
        }
        // Nested comment must keep track
        // of depth.
      } else if (in.peek() == '|') {
        int depth_count = 1;
        while (depth_count > 0) {
          if (in.peek() == '|') {
            in.get();
            if (in.peek() == '#') {
              in.get();
              depth_count--;
            }
          } else if (in.peek() == '#') {
            in.get();
            if (in.peek() == '|') {
              in.get();
              depth_count++;
            }
          } else {
            if (DEBUG_PRINT) {
              std::cerr << "What is this? \'" << in.peek()
                        << "\'" << std::endl;
            }
            in.get();
          }
        }
        return false;

        // Single datum comment.
      } else if (in.peek() == ';') {
        in.get();
        result = Token(Token::Type::DATUM_COMMENT, "#;");
        return true;

        // Directive
      } else if (in.peek() == '!') {
        in.get();

        // Read in the directive string.
        std::string buffer;
        while (!is_delimiter(in.peek())) {
          buffer += in.get();
        }
        result = Token(Token::Type::DIRECTIVE, buffer);
        return true;
      } else {
        if (DEBUG_PRINT) {
          std::cerr << "BROKE ON: " << in.peek() << std::endl;
        }
        throw shaka::TokenizerException(20011, "Tokenizer.parse_token: invalid "
            "hash directive");
        result = Token(Token::Type::END_OF_FILE);
        return false;
      }
    } else {
      if (DEBUG_PRINT) {
        std::cerr << "BROKE ON: " << in.peek() << std::endl;
      }
      throw shaka::TokenizerException(20012, "Tokenizer.parse_token: is not a "
          "hash? wrong");
      result = Token(Token::Type::END_OF_FILE);
      return false;
    }

    /// @todo Need to make sure why the compiler gives a
    ///       warning why it doesn't think it may
    ///       not return a value.
    return false;
  }

  Token parse_number(std::string& buffer) {

    bool number = false;
    // Parse sign if it's there
    if (is_explicit_sign(in.peek())) {
      buffer += in.get();
    }
    // Parse the integer part.
    while (std::isdigit(in.peek())) {
      buffer += in.get();

      number = true;
    }

    // Parse in a dot if it's a float
    if (in.peek() == '.') {
      buffer += in.get();

      // Parse in the fractional integer part.
      while (std::isdigit(in.peek())) {
        buffer += in.get();
      }

      number = true;
    }

    // If there is a /, it is a fraction
    if (in.peek() == '/') {
      buffer += in.get();

      while (std::isdigit(in.peek())) {
        buffer += in.get();

        number = true;
      }
    }
    //
    //  // Make sure that it's the end of the number
    //  if (is_delimiter(in.peek())) {
    //      return Token(Token::Type::NUMBER, buffer);
    //  } else {
    //      if (DEBUG_PRINT) {
    //          std::cerr << "Tokenizer.parse_number: Did not find following delimiter" << std::endl;
    //      }
    //      throw shaka::TokenizerException("Tokenizer.parse_number: Did not find following delimiter");
    //      return Token(Token::Type::INVALID);
    //  }
    //

    if (number == true) {
      return Token(Token::Type::NUMBER, buffer);
    } else {
      return Token(Token::Type::INVALID);
    }
  }

  Token parse_token() {
    bool done = false;
    while (!done) {
      // Quote
      if (in.peek() == '\'') {
        in.get();
        return Token(Token::Type::QUOTE, "\'");

        // Backtick
      } else if (in.peek() == '`') {
        in.get();
        return Token(Token::Type::BACKTICK, "`");

        // Comma
      } else if (in.peek() == ',') {
        in.get();
        // Comma at-sign.
        if (in.peek() == '@') {
          in.get();
          return Token(Token::Type::COMMA_ATSIGN, ",@");
        } else {
          return Token(Token::Type::COMMA, ",");
        }

      } else if (in.peek() == '.') {
        in.get();
        return Token(Token::Type::PERIOD, ".");

        // Begin parenthesis
      } else if (in.peek() == '(') {
        return parse_paren_start();

        // End parenthesis
      } else if (in.peek() == ')') {
        return parse_paren_end();

        // String
      } else if (in.peek() == '\"') {
        return parse_string();

        // Line comment
      } else if (in.peek() == ';') {
        parse_line_comment();
        // Then, continue and get next token to parse.
        done = false;

        // End of file
      } else if (in.peek() == EOF) {
        if (DEBUG_PRINT) {
          std::cerr << "What EOF is this? \'" << in.peek() << "\'" << std::endl;
        }
        in.get();
        return Token(Token::Type::END_OF_FILE);

        // Skip whitespace
      } else if (std::isspace(in.peek())) {
        if (DEBUG_PRINT) {
          std::cerr << "What space is this? \'" << in.peek() << "\'"
                    << std::endl;
        }
        in.get();
        done = false;

        // Comment or boolean begins with #
      } else if (in.peek() == '#') {
        Token result(Token::Type::END_OF_FILE);
        if (rule_hash(result)) {
          return result;
        } else {
          done = false;
        }

        // Identifier ==> <initial> <subsequent>*
      } else if (is_initial(in.peek())) {
        if (DEBUG_PRINT) {
          std::cout << "What INITIAL is this? \'" << in.peek() << "\'"
                    << std::endl;
        }
        std::string buffer;
        buffer += in.get();
        // Keep getting subsequent if needed.
        while (is_subsequent(in.peek())) {
          buffer += in.get();
        }
        return Token(Token::Type::IDENTIFIER, buffer);

        // Identifier ==> <vertical line> <symbol element>* <vertical line>
      } else if (in.peek() == '|') {
        in.get();
        std::string buffer;
        while (handle_symbol_element(in, buffer));
        if (in.peek() == '|') {
          in.get();
          return Token(Token::Type::IDENTIFIER, buffer);
        } else {
          throw shaka::TokenizerException(20013, "Tokenizer.parse_token: "
              "invalid pipe-delimted identifier syntax");
          return Token(Token::Type::END_OF_FILE);
        }

        // Identifier ==> <explicit sign> ...
      } else if (is_explicit_sign(in.peek())) {
        std::string buffer;
        buffer += in.get();
        // <sign subsequent> <subsequent>*
        if (is_sign_subsequent(in.peek())) {
          buffer = +in.get();
          while (is_subsequent(in.peek())) {
            buffer += in.get();
          }
          return Token(Token::Type::IDENTIFIER, buffer);
          // . <dot subsequent> <subsequent>*
        } else if (in.peek() == '.') {
          buffer += in.get();
          if (is_dot_subsequent(in.peek())) {
            buffer += in.get();
            while (is_subsequent(in.peek())) {
              buffer += in.get();
            }
            return Token(Token::Type::IDENTIFIER, buffer);

            // if it's a digit, it's probably a number
          } else if (std::isdigit(in.peek())) {
            return parse_number(buffer);

            // no <dot subsequent> ==> error!
          } else {
            throw shaka::TokenizerException(20014, "Tokenizer.parse_token: bad "
                "dot subsequent identifier");
            return Token(Token::Type::END_OF_FILE);
          }

          // Parse in a number
        } else if (std::isdigit(in.peek())) {
          std::string buffer;
          return parse_number(buffer);

          // No other identifier? Just <explicit sign> is fine.
        } else {
          return Token(Token::Type::IDENTIFIER, buffer);
        }

        // Identifier ==> . <dot subsequent> <subsequent>
      } else if (in.peek() == '.') {
        std::string buffer;
        buffer += in.get();
        if (is_dot_subsequent(in.peek())) {
          buffer += in.get();
          while (is_subsequent(in.peek())) {
            buffer += in.get();
          }
          return Token(Token::Type::IDENTIFIER, buffer);
        } else {
          throw shaka::TokenizerException(20015, "Tokenizer.parse_token: bad "
              "dot subsequent identifier");
          return Token(Token::Type::END_OF_FILE);
        }

        // <number> ==> <num 10> for now
        /// @todo Add real, rational, complex for different radixes.
      } else if (std::isdigit(in.peek())) {
        std::string buffer;
        return parse_number(buffer);

        // NOT A VALID TOKEN!!!
      } else {
        if (DEBUG_PRINT) {
          std::cerr << "BROKE ON: " << in.peek() << std::endl;
        }
        throw shaka::TokenizerException(20016, "Tokenizer.parse_token: invalid "
            "token");
        return Token(Token::Type::END_OF_FILE);
      }
    }
    throw 1;
    return Token(Token::Type::END_OF_FILE);
  }

  bool is_delimiter(char c) {
    return (
        std::isspace(c)
            || c == '|'
            || c == '('
            || c == ')'
            || c == '\"'
            || c == ';'
    );
  }

  bool is_special_initial(char c) {
    return (
        c == '!' ||
            c == '$' ||
            c == '%' ||
            c == '&' ||
            c == '*' ||
            c == '/' ||
            c == '!' ||
            c == ':' ||
            c == '>' ||
            c == '=' ||
            c == '>' ||
            c == '?' ||
            c == '^' ||
            c == '_' ||
            c == '!'
    );
  }

  bool is_special_subsequent(char c) {
    return (
        c == '.'
            || c == '@'
            || is_explicit_sign(c)
    );
  }

  bool is_explicit_sign(char c) {
    return (c == '+' || c == '-');
  }

  bool is_letter(char c) {
    return (std::isalpha(c));
  }

  bool is_subsequent(char c) {
    return (is_initial(c)
        || is_digit(c)
        || is_special_subsequent(c));
  }

  bool is_initial(char c) {
    return (is_letter(c) || is_special_initial(c));
  }

  bool is_digit(char c) {
    return (std::isdigit(c));
  }

  bool handle_symbol_element(std::istream& in, std::string& interm) {

    // Terminating symbol
    if (in.peek() == '|') {
      return false;

      // Escape sequence
    } else if (in.peek() == '\\') {
      in.get();
      // inline_hex_escape?
      if (in.peek() == 'x') {
        in.get();
        return (handle_inline_hex_escape(in, interm));

        // mnemonic_escape?
      } else if (in.peek() == 'a') {
        in.get();
        interm += '\a';
        return true;
      } else if (in.peek() == 'b') {
        in.get();
        interm += '\b';
        return true;
      } else if (in.peek() == 't') {
        in.get();
        interm += '\t';
        return true;
      } else if (in.peek() == 'n') {
        in.get();
        interm += '\n';
        return true;
      } else if (in.peek() == 'r') {
        in.get();
        interm += '\r';
        return true;

        // escape for pipe character?
      } else if (in.peek() == '|') {
        in.get();
        interm += '|';
        return true;

        // otherwise, invalid!
      } else {
        throw shaka::TokenizerException(20017,
                                        "Tokenizer.mnemonic_escape: invalid escape character");
        return false;
      }

      // Otherwise, just consume the next character.
    } else {
      interm += in.get();
      return true;
    }
  }

  bool is_dot_subsequent(char c) {
    return (is_sign_subsequent(c) || c == '.');
  }

  bool handle_inline_hex_escape(std::istream& in, std::string& interm) {
    char c = in.peek();
    std::string buffer;
    while (
        std::isdigit(c)
            || c == 'a'
            || c == 'b'
            || c == 'c'
            || c == 'd'
            || c == 'e'
            || c == 'f'
        ) {
      in.get();
      buffer += c;
      c = in.peek();
    }
    // Correct form ==> push character onto interm string.
    if (in.peek() == ';') {
      int i = std::stoi(buffer, 0, 16);
      interm += static_cast<char>(i);
      return true;

      // incorect form ==> error!
    } else {
      throw shaka::TokenizerException(20018, "Tokenizer"
          ".handle_inline_hex_escape: no terminating \';\' character");
      return false;
    }
  }

  bool is_sign_subsequent(char c) {
    return (
        is_initial(c)
            || is_explicit_sign(c)
            || c == '@'
    );
  }

  bool is_hex_digit(char c) {
    return (
        std::isdigit(c)
            || c == 'a'
            || c == 'b'
            || c == 'c'
            || c == 'd'
            || c == 'e'
            || c == 'f'
    );
  }
};

} // namespace shaka

#endif // SHAKA_LEXER_TOKENIZER_H
