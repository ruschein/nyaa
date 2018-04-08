/** \file    NyaaTokenizer.cc
 *  \brief   Implementation of the tokenizer for the Nyaa interpreter.
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
#include "NyaaTokenizer.h"
#include <stdexcept>
#include <cctype>
#include <cstring>


namespace Nyaa {


Token Tokenizer::getToken() {
    // Do we have a chached token?
    if (previous_token_ != NULL_TOKEN) {
        Token retval(NULL_TOKEN);
        retval.swap(previous_token_);
	std::swap(current_pos_, previous_current_pos_);
	std::swap(identifier_in_braces_, previous_identifier_in_braces_);
	std::swap(ch_, previous_ch_);
	std::swap(token_start_pos_, previous_token_start_pos_);
	std::swap(string_constant_, previous_string_constant_);
	std::swap(float_constant_, previous_float_constant_);
	std::swap(boolean_constant_, previous_boolean_constant_);
	std::swap(int_constant_, previous_int_constant_);
	std::swap(identifier_, previous_identifier_);
	return retval;
    }

    // Skip over whitespace:
    while (ch_ != source_.cend() and std::isspace(*ch_))
	++ch_;

    token_start_pos_ = ch_;

    if (ch_ == source_.cend()) {
        token_start_pos_ = source_.cend();
        return EOS;
    }

    switch (*ch_) {
    case ':': 
        // colon separates identifier from default value in braces
        identifier_in_braces_ = false; 
        return COLON;
    case '^': return CARET;
    case '{':
        identifier_in_braces_ = true;
        return OPEN_BRACE;
    case '}':
       identifier_in_braces_ = false;
       return CLOSE_BRACE;
    case '(': return OPEN_PAREN;
    case ')': return CLOSE_PAREN;
    case '+': return PLUS;
    case '-': return MINUS;
    case '/': return DIV;
    case '*': return MUL;
    case '=': return EQUAL;
    case '$': return DOLLAR;
    case ',': return COMMA;
    case '&': return AMPERSAND;
    }

    if (*ch_ == '"')
        return parseStringConstant();
    if (not identifier_in_braces_ and (std::isdigit(*ch_) or *ch_ == '.')) {
        --ch_;
        return parseNumericConstant();
    }

    if (*ch_ == '<') {
        ++ch_;
        if (ch_ == source_.cend())
            return LESS_THAN;
        if (*ch_ == '>')
            return NOT_EQUAL;
        if (*ch_ == '=')
            return LESS_OR_EQUAL;
        --ch_;
        return LESS_THAN;
    }

    if (*ch_ == '>') {
        ++ch_;
        if (ch_ == source_.cend())
            return GREATER_THAN;
        if (*ch_ == '=')
            return GREATER_OR_EQUAL;
        --ch_;
        return GREATER_THAN;
    }

    if (identifier_in_braces_ or std::isalpha(*ch_)) {
        --ch_;
        return identifier_in_braces_ ? parseIdentifier() : parseSimpleIdentifier();
    }

    error_msg_ = "unexpected input character '" + std::string(1, *ch_) + "'";

    return ERROR;
}


void Tokenizer::ungetToken(const Token &token) {
    if (previous_token_ != NULL_TOKEN)
        throw std::runtime_error("in Nyaa::Tokenizer::ungetToken: can't unget two tokens in a row!");
    previous_token_ = token;
}


Token Tokenizer::parseStringConstant() {
    string_constant_.clear();

    bool escaped(false);
    while (ch_ != source_.cend()) {
      if (*ch_ == '\\')
          escaped = true;
      else {
          if (escaped) {
              switch (*ch_) {
              case '\\':
                  string_constant_ += '\\';
                  break;
              case '"':
                  string_constant_ += '"';
                  break;
              case 'n':
                  string_constant_ += '\n';
                  break;
              default:
                  error_msg_ = "unknown escape character '" + std::string(1, *ch_) + "'.";
                  return ERROR;
              }

              escaped = false;
          } else if (*ch_ == '"') {
              return STRING_CONSTANT;
          } else
              string_constant_ += *ch_;
      }

      ++ch_;
    }

    error_msg_ = "unterminated String constant.";
    return ERROR;
}

  
Token Tokenizer::parseNumericConstant() {
    std::string number;
    while (ch_ != source_.cend() and std::isdigit(*ch_))
        number += *ch_++;

    if (ch_ == source_.cend() or (*ch_ != 'e' and *ch_ != 'E' and *ch_ != '.')) {
convert:
        if (std::sscanf(number.c_str(), "%lf", &float_constant_) != 1) {
            error_msg_ = "invalid numeric constant.";
	    return ERROR;
        }
	if (ch_ != source_.cend())
	    --ch_;
        return FLOAT_CONSTANT;
    }

    // Optional decimal point.
    if (*ch_ == '.') {
      number += *ch_++;
      while (ch_ != source_.cend() and std::isdigit(*ch_))
          number += *ch_++;
    }

    // Optional exponent.
    if (*ch_ == 'e' or *ch_ == 'E') {
        number += *ch_++;
        if (ch_ == source_.cend()) {
            error_msg_ = "invalid numeric constant.";
	    return ERROR;
	}

      // Optional sign.
      if (*ch_ == '+' or *ch_ == '-')
          number += *ch_++;

      // Now we require at least a single digit.
      if (not std::isdigit(*ch_)) {
          error_msg_ = "missing digits in exponent.";
	  return ERROR;
      }
      --ch_;

      while (ch_ != source_.cend() and std::isdigit(*ch_))
          number += *ch_++;
    }
    --ch_;

    goto convert;
}

  
Token Tokenizer::parseIdentifier() {
    std::string identifier;
    bool escaped(false);
    while (ch_ != source_.cend() and
           ((*ch_ != '}' and *ch_ != ':' and *ch_ != ',' and *ch_ != '(' and *ch_ != ')') or escaped))
    {
        if (escaped) {
            escaped = false;
            identifier += *ch_;
        } else if (*ch_ == '\\')
            escaped = true;
        else
            identifier += *ch_;

	++ch_;
    }
    if (escaped) {
        error_msg_ = "invalid column name at end of formula.";
        return ERROR;
    }
    --ch_;

    if (::strcasecmp(identifier.c_str(), "TRUE") == 0) {
        boolean_constant_ = true;
        return BOOLEAN_CONSTANT;
    }
    if (::strcasecmp(identifier.c_str(), "FALSE") == 0) {
        boolean_constant_ = false;
        return BOOLEAN_CONSTANT;
    }

    std::swap(identifier, identifier_);
    return IDENTIFIER;
}

  
Token Tokenizer::parseSimpleIdentifier() {
    std::string identifier;
    while (ch_ != source_.cend() and (std::isalpha(*ch_) or std::isdigit(*ch_) or *ch_ == '_'))
        identifier += *ch_++;
    --ch_;

    if (::strcasecmp(identifier.c_str(), "TRUE") == 0) {
        boolean_constant_ = true;
        return BOOLEAN_CONSTANT;
    }
    if (::strcasecmp(identifier.c_str(), "FALSE") == 0) {
        boolean_constant_ = false;
        return BOOLEAN_CONSTANT;
    }

    std::swap(identifier, identifier_);
    return IDENTIFIER;
}
  
  
} // namespace Nyaa
