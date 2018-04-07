/** \file    NyaaToken.h
 *  \brief   Token types for the Nyaa interpreter.
 *  \author  Dr. Johannes Ruscheinski
 */

/*
    Copyright (C) 2018 Dr. Johannes Ruscheinski

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef NYAA_TOKEN_H
#define NYAA_TOKEN_H


#include <string>


namespace Nyaa {


enum class TokenType {
  STRING_CONSTANT, FLOAT_CONSTANT, BOOLEAN_CONSTANT, IDENTIFIER, OPEN_BRACE, CLOSE_BRACE,
  OPEN_PAREN, CLOSE_PAREN, COLON, CARET, PLUS, MINUS, DIV, MUL, EQUAL, NOT_EQUAL, GREATER_THAN,
  LESS_THAN, GREATER_OR_EQUAL, LESS_OR_EQUAL, DOLLAR, COMMA, AMPERSAND, EOS, ERROR
};


enum class OpType { NONE, COMP_OP, ARITH_OP, STRING_OP };

 
class Token {
 public:
  static const std::string NO_STRING_REP;
 private:
  TokenType token_type_;
  std::string string_rep_; // The sting representation.
  OpType op_type_;
 public:
  inline Token(const TokenType token_type, const std::string &string_rep, const OpType op_type)
    : token_type_(token_type), string_rep_(string_rep), op_type_(op_type) { }

  inline TokenType getType() const { return token_type_; }
  inline bool IsCompOp() const { return op_type_ == OpType::COMP_OP; }
  inline bool IsArithOp() const { return op_type_ == OpType::ARITH_OP; }
  inline bool IsStringOp() const { return op_type_ == OpType::STRING_OP; }
};

 
const Token STRING_CONSTANT(TokenType::STRING_CONSTANT, Token::NO_STRING_REP, OpType::NONE);
const Token FLOAT_CONSTANT(TokenType::FLOAT_CONSTANT, Token::NO_STRING_REP, OpType::NONE);
const Token BOOLEAN_CONSTANT(TokenType::BOOLEAN_CONSTANT, Token::NO_STRING_REP, OpType::NONE);
const Token IDENTIFIER(TokenType::IDENTIFIER, Token::NO_STRING_REP, OpType::NONE);
const Token OPEN_BRACE(TokenType::OPEN_BRACE, "{", OpType::NONE);
const Token CLOSE_BRACE(TokenType::CLOSE_BRACE, "}", OpType::NONE);
const Token OPEN_PAREN(TokenType::OPEN_PAREN, "(", OpType::NONE);
const Token CLOSE_PAREN(TokenType::CLOSE_PAREN, ")", OpType::NONE);
const Token COLON(TokenType::COLON, ":", OpType::NONE);
const Token CARET(TokenType::CARET, "^", OpType::ARITH_OP);
const Token PLUS(TokenType::PLUS, "+", OpType::ARITH_OP);
const Token MINUS(TokenType::MINUS, "-", OpType::ARITH_OP);
const Token DIV(TokenType::DIV, "/", OpType::ARITH_OP);
const Token MUL(TokenType::MUL, "*", OpType::ARITH_OP);
const Token EQUAL(TokenType::EQUAL, "=", OpType::COMP_OP);
const Token NOT_EQUAL(TokenType::NOT_EQUAL, "<>", OpType::COMP_OP);
const Token GREATER_THAN(TokenType::GREATER_THAN, ">", OpType::COMP_OP);
const Token LESS_THAN(TokenType::LESS_THAN, "<", OpType::COMP_OP);
const Token GREATER_OR_EQUAL(TokenType::GREATER_OR_EQUAL, ">=", OpType::COMP_OP);
const Token LESS_OR_EQUAL(TokenType::LESS_OR_EQUAL, "<=", OpType::COMP_OP);
const Token DOLLAR(TokenType::DOLLAR, "$", OpType::NONE);
const Token COMMA(TokenType::COMMA, ",", OpType::NONE);
const Token AMPERSAND(TokenType::AMPERSAND, "&", OpType::NONE);
const Token EOS(TokenType::EOS, Token::NO_STRING_REP, OpType::NONE);
const Token ERROR(TokenType::ERROR, Token::NO_STRING_REP, OpType::NONE);


} // namespace Nyaa


#endif // ifndef NYAA_TOKEN_H
