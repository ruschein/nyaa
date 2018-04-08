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
public:
    explicit Tokenizer(const std::string &source);

    /** Calls this until it returns EOS. */
    Token getToken();

    /** You may only call this optionally once after a prior call to getToken(). */ 
    void ungetToken(const Token &token);

    /** \return The position where the current token started. */
    size_t getStartPos() const;

    const std::string &getStringConstant() const;
    const std::string &getFloatConstant() const;
    const std::string &getBooleanConstant() const;
    const std::string &getIntConstant() const;
    const std::string &getIdent() const;
    const std::string &getErrMsg() const;
private:
    Token parseStringConstant();
    Token parseNumericConstant();
    Token parseIdentifier();
    Token parseSimpleIdentifier();
};

  
} // namespace Nyaa


#endif // ifndef NYAA_TOKENIZER_H


