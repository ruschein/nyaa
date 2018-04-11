/** \file    NyaaCodeAndSourceLocation.h
 *  \brief   Declaration of the CodeAndSourceLocation class.
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
#ifndef NYAA_CODE_AND_SOURCE_LOCATION_H
#define NYAA_CODE_AND_SOURCE_LOCATION_H


#include <cinttypes>
#include <NyaaInstructions.h>


namespace Nyaa {


/** \class CodeAndSourceLocation
 *  \brief Encapsulates a single opcode code and the location in the original source equation.
 */
class CodeAndSourceLocation {
    const Instruction instruction_;
    const size_t source_location_;
public:
    inline CodeAndSourceLocation(const Instruction instruction, const size_t source_location)
        : instruction_(instruction), source_location_(source_location) { }
    CodeAndSourceLocation() = default;
    CodeAndSourceLocation(const CodeAndSourceLocation &rhs) = default;

    inline Instruction getCode() const { return instruction_; }
    inline int getSourceLocation() const { return source_location_; }
};


} // namespace Nyaa


#endif // ifndef NYAA_CODE_AND_SOURCE_LOCATION_H
