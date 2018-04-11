/** \file    NyaaNodes.cc
 *  \brief   Implementation of the AST node type hierarchy for the parser for the Nyaa interpreter.
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
#include "NyaaNodes.h"
#include <stdexcept>
#include "NyaaCodeAndSourceLocation.h"


namespace Nyaa {


BinOpNode::BinOpNode(const size_t source_location, const Token &operator_type, const TreeNode * const lhs, const TreeNode * const rhs)
  : AbstractNode(source_location), operator_(operator_type), lhs_(lhs), rhs_(rhs)
{
    if (lhs == nullptr)
        throw std::invalid_argument("in BinOpNode::BinOpNode: operand must not be NULL!");
    if (rhs == nullptr)
        throw std::invalid_argument("in BinOpNode::BinOpNode: right operand must not be NULL!");
    if (lhs->getType() != rhs->getType())
        throw std::invalid_argument("in BinOpNode::BinOpNode:  left and right operands must be of the same type!");
}

  
void BinOpNode::genCode(std::stack<CodeAndSourceLocation> * const code_stack) const {
    rhs_->genCode(code_stack);
    lhs_->genCode(code_stack);

    switch (operator_.getType()) {
    case TokenType::CARET:
       code_stack->emplace(CodeAndSourceLocation(Instruction::FPOW, getSourceLocation()));
       break;
    case TokenType::PLUS:
        code_stack->emplace(CodeAndSourceLocation(Instruction::FADD, getSourceLocation()));
	break;
    case TokenType::MINUS:
        code_stack->emplace(CodeAndSourceLocation(Instruction::FSUB, getSourceLocation()));
	break;
    case TokenType::DIV:
        code_stack->emplace(CodeAndSourceLocation(Instruction::FDIV, getSourceLocation()));
	break;
    case TokenType::MUL:
        code_stack->emplace(CodeAndSourceLocation(Instruction::FMUL, getSourceLocation()));
	break;
    case TokenType::EQUAL:
      code_stack->emplace(CodeAndSourceLocation(determineOpCode(Instruction::BEQLF, Instruction::BEQLS,
								Instruction::BEQLB, Instruction::BEQLI),
						getSourceLocation()));
	break;
    case TokenType::NOT_EQUAL:
        code_stack->emplace(CodeAndSourceLocation(determineOpCode(Instruction::BNEQLF, Instruction::BNEQLS,
								  Instruction::BNEQLB, Instruction::BNEQLI),
						  getSourceLocation()));
	break;
    case TokenType::GREATER_THAN:
        code_stack->emplace(CodeAndSourceLocation(determineOpCode(Instruction::BGTF, Instruction::BGTS,
								  Instruction::BGTB, Instruction::BGTI),
						  getSourceLocation()));
	break;
    case TokenType::LESS_THAN:
        code_stack->emplace(CodeAndSourceLocation(determineOpCode(Instruction::BLTF, Instruction::BLTS,
								  Instruction::BLTB, Instruction::BLTI),
						  getSourceLocation()));
	break;
    case TokenType::GREATER_OR_EQUAL:
        code_stack->emplace(CodeAndSourceLocation(determineOpCode(Instruction::BGTEF, Instruction::BGTES,
								  Instruction::BGTEB, Instruction::BGTEI),
						  getSourceLocation()));
	break;
    case TokenType::LESS_OR_EQUAL:
        code_stack->emplace(CodeAndSourceLocation(determineOpCode(Instruction::BLTEF, Instruction::BLTES,
								  Instruction::BLTEB, Instruction::BLTEI),
						  getSourceLocation()));
	break;
    case TokenType::AMPERSAND:
        code_stack->emplace(CodeAndSourceLocation(Instruction::SCONCAT, getSourceLocation()));
	break;
    default:
      throw std::runtime_error(std::to_string(getSourceLocation()) + ": unknown operator: " + operator_.getStringRep() + ".");
    }
}


static std::string NodeTypeToString(const NodeType node_type) {
    switch (node_type) {
    case NodeType::FLOAT_NODE:
        return "FLOAT";
    case NodeType::STRING_NODE:
        return "STRING";
    case NodeType::BOOLEAN_NODE:
        return "BOOLEAN";
    case NodeType::INT_NODE:
        return "INT";
    }
}

  
Instruction BinOpNode::determineOpCode(const Instruction float_op_code, const Instruction string_op_code,
				       const Instruction boolean_op_code, const Instruction int_op_code) const
{
    const NodeType operand_type(lhs_->getType());
    if (operand_type == NodeType::FLOAT_NODE)
        return float_op_code;
    else if (operand_type == NodeType::STRING_NODE)
        return string_op_code;
    else if (operand_type == NodeType::BOOLEAN_NODE)
        return boolean_op_code;
    else
        return int_op_code;

    throw std::runtime_error(std::to_string(lhs_->getSourceLocation()) + ": invalid LHS operand type for comparison: "
			     + NodeTypeToString(operand_type) + ".");
}


} // namespace Nyaa
