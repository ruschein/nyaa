/** \file    NyaaToken.cc
 *  \brief   Implementation of the Token class for the Nyaa interpreter.
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
#include "NyaaToken.h"
#include <algorithm>


namespace Nyaa {


const std::string Token::NO_STRING_REP("?");


Token &Token::swap(Token &other) {
    std::swap(token_type_, other.token_type_);
    std::swap(string_rep_, other.string_rep_);
    std::swap(op_type_, other.op_type_);

    return *this;
}


} // namespace Nyaa
