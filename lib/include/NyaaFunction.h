/** \file    NyaaFunction.h
 *  \brief   Abstract base class for functions of the Nyaa interpreter.
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
#ifndef NYAA_FUNCTION_H
#define NYAA_FUNCTION_H


#include <stdexcept>
#include <string>
#include <vector>
#include <cinttypes>


namespace Nyaa {


enum class NodeType { BOOLEAN_NODE, INT_NODE, FLOAT_NODE, STRING_NODE, NULL_NODE };


class FuncArg {
    NodeType type_;
    bool bool_value_;
    double double_value_;
    int64_t int_value_;
    std::string string_value_;
public:
    FuncArg(const bool bool_value): type_(BOOLEAN_NODE), bool_value_(bool_value) { }
    FuncArg(const double double_value): type_(FLOAT_NODE), double_value_(double_value) { }
    FuncArg(const int64_t int_value): type_(INT_NODE), int_value_(int_value) { }
    FuncArg(const std::string &string_value): type_(STRING_NODE), string_value_(string_value) { }

    inline NodeType getType() const { return type_ }

    inline bool getBoolValue() const {
        if (type_ != BOOLEAN_NODE)
            throw std::logic_error("in FuncArg::getBoolValue: not a boolean argument!");
	return bool_value_;
    }

    inline double getDoubleValue() const {
        if (type_ != FLOAT_NODE)
            throw std::logic_error("in FuncArg::getDoubleValue: not a floating point argument!");
	return double_value_;
    }

    inline int64_t getIntValue() const {
        if (type_ != INT_NODE)
            throw std::logic_error("in FuncArg::getIntValue: not an integer argument!");
	return int_value_;
    }

    inline const std::string &getStringValue() const {
        if (type_ != STRING_NODE)
            throw std::logic_error("in FuncArg::getStringValue: not a string argument!");
	return string_value_;
    }
};

 
/** 
 * The function interface.
 */
class Function {
public:
    /**
     *  Used to parse the function string.  This name is treated in a case-insensitive manner!
     *  \return the name by which you must call the function when used in an attribute equation.
     */
    virtual const std::String &getName() = 0;

    /**
     *  Used to provide help for users.  Unlike getUsageDescription(), this is an informal English description,
     *  like "Calculates the natural logarithm of its argument."
     *
     *  \return a description of what this function does
     */
    virtual const std::string getFunctionSummary() = 0;

    /**
     *  Used to provide help for users.  Unlike getFunctionSummary(), this describes how to call this function,
     *  like "Call with LN(number)."
     *
     *  \return a description of how to use this function
     */
    virtual const std::string getUsageDescription() = 0;

    /**
     *  \return the static return type of this function, NULL_NODE, FLOAT_NODE, INT_NODE, STRING_NODE, or BOOLEAN_NODE.
     *           If the static return type is NULL_NODE, the dynamic return type will be one of FLOAT_NODE, INT_NODE, STRING_NODE, or BOOLEAN_NODE
     *           and will depend on the arguments passed to the function!
     *
     *  Note, this is used by external tools used to filter a list of functions based on what a valid return type might be.
     */
    virtual NodeType getReturnType() = 0;

    /**
     *  \return the return type for this function (FLOAT_NODE, INT_NODE, STRING_NODE, or BOOLEAN_NODE)
     *          or NULL_NODE if the arguments passed in had the wrong arity or a type mismatch.
     *
     *  Note that this is different from getReturnType() in that it will never return the wildcard Object.class.
     *  It is used by the parser which knows the actual type of the arguments in any given call to this function.
     */
    virtual NodeType validateArgTypes(const std::vector<NodeType> &arg_types) = 0;

    /**
     *  Used to invoke this function.
     *  \param args the function arguments which must correspond in type and number to what getParameterTypes() returns.
     *  \return the result of the function evaluation.  The actual type of the returned object will be what getReturnType() returns.
     *  \throws FunctionError 
     *  \throws ArithmeticException thrown if a numeric error, e.g. a division by zero occurred.
     *  \throws IllegalArgumentException thrown for any error that is not a numeric error, for example if a function only accepts positive numbers and a negative number was passed in.
     */
    Object evaluateFunction(const Object[] args) throws FunctionError = 0;
};


} // namespace Nyaa


#endif // ifndef NYAA_FUNCTION_H
