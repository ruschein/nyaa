/** \file    NyaaTokenizer.h
 *  \brief   Ibterface of the tokenizer the Nyaa interpreter.
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
#ifndef NYAA_TOKENIZER_H
#define NYAA_TOKENIZER_H


#include <string>
#include "NyaaToken.h"


namespace Nyaa {


/** \class NyaaTokenizer
  * \brief Generates a token stream for the parser.
  */
class Tokenizer {
    const std::string &source_;
    Token previous_token_;
    size_t current_pos_, previous_current_pos_;
    bool identifier_in_braces_, previous_identifier_in_braces_;
    std::string::const_iterator ch_, previous_ch_;
    std::string::const_iterator token_start_pos_, previous_token_start_pos_;
    std::string string_constant_, previous_string_constant_;
    double float_constant_, previous_float_constant_;
    bool boolean_constant_, previous_boolean_constant_;
    long int_constant_, previous_int_constant_;
    std::string identifier_, previous_identifier_;
    std::string error_msg_;
public:
    explicit Tokenizer(const std::string &source)
        : source_(source), previous_token_(NULL_TOKEN), current_pos_(-1),
          identifier_in_braces_(false), ch_(source_.cbegin()) { }

    /** Calls this until it returns EOS. */
    Token getToken();

    /** You may only call this optionally once after a prior call to getToken(). */
    void ungetToken(const Token &token);

    /** \return The position where the current token started. */
    size_t getStartPos() const;

    const std::string &getStringConstant() const { return string_constant_; }
    double getFloatConstant() const { return float_constant_; }
    bool getBooleanConstant() const { return boolean_constant_; }
    long getIntConstant() const { return int_constant_; }
    const std::string &getIdent() const { return identifier_; }
    inline const std::string &getErrMsg() const { return error_msg_; }
private:
    Token parseStringConstant();
    Token parseNumericConstant();

    /**
     *  Looks for an attribute name.  Attribute names in formulas must not contain '}', ':', ',' nor
     *  '(' or ')'.  In order to allow a '}' or a ':' in an attribute name, it has to be escaped with a
     *  backslash.  Any backslash in an identifier a.k.a. attribute name implies that the next
     *  input character will be included in the identifier this also allows for embedding
     *  backslashes by doubling them.
     */
    Token parseIdentifier();

    Token parseSimpleIdentifier();
};


} // namespace Nyaa


#endif // ifndef NYAA_TOKENIZER_H
