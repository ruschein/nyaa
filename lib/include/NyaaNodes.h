/** \file    NyaaNodes.h
 *  \brief   Interface of the AST node type hierarchy for the parser for the Nyaa interpreter.
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
#ifndef NYAA_NODES_H
#define NYAA_NODES_H


#include <stack>
#include <string>
#include "NyaaInstructions.h"
#include "NyaaToken.h"


namespace Nyaa {


// Forward declaration:
class CodeAndSourceLocation;

 
enum class NodeType { BOOLEAN_NODE, INT_NODE, FLOAT_NODE, STRING_NODE };


class TreeNode {
public:
    virtual ~TreeNode();
  
    /** Returns the start of the location in the equation where the code was found that was
     *  turned into a node in the parse tree.
     *  \return the location in the source code that resulted in this node
     */
    virtual size_t getSourceLocation() const = 0;

    /** Returns the type of this node.
     *  \return the type of the result of the code generated from this node will result in
     */
    virtual NodeType getType() const = 0; // The type of this node.

    /** Returns the left child if it exists or null if it doesn't.
     *  \return the left child, if any, of this node in the parse tree
     */
    virtual const TreeNode *getLeftChild() const = 0;

    /** Returns the right child if it exists or null if it doesn't.
     *  \return the right child, if any, of this node in the parse tree
     */
    virtual const TreeNode *getRightChild() const = 0;

    /** Generates code for this node and pushes it onto the execution stack.
     * \param code_stack the execution stack to push the generated code for this node.
     */
    virtual void genCode(std::stack<CodeAndSourceLocation> * const code_stack) const = 0;
};


class AbstractNode: public TreeNode {
    const size_t source_location_; // Which location in the "source code" is this associated with.
public:
    /** Base class constructor for any <code>Node</code> type.
     *  @param sourceLocation  start of the location in the equation where the code was found
     *                         that was turned into a node in the parse tree
     */
    inline explicit AbstractNode(const size_t source_location): source_location_(source_location) { }

    virtual inline size_t getSourceLocation() const final override { return source_location_; }

    /** Returns a <code>std::string</code> representation of this node.
     *  \return a textual representation of this node
     *  \note   This is typically only used for debugging.
     */
    virtual std::string toString() const = 0;
};


class BinOpNode: public AbstractNode {
    const Token operator_;
    const TreeNode *lhs_, *rhs_;
public:
    BinOpNode(const size_t source_location, const Token &operator_type, const TreeNode * const lhs,
	      const TreeNode * const rhs);
    virtual ~BinOpNode() final { delete lhs_; delete rhs_;}

    inline virtual std::string toString() const final { return "BinOpNode: " + operator_.getStringRep(); }

    NodeType getType() const final { return operator_.isCompOp() ? NodeType::BOOLEAN_NODE : lhs_->getType(); }

    /** \return the left operand */
    inline const TreeNode *getLeftChild() const final { return lhs_; }

    /** \return the right operand */
    inline const TreeNode *getRightChild() const final { return rhs_; }

    void genCode(std::stack<CodeAndSourceLocation> * const code_stack) const final;

    inline const Token &getOperator() const { return operator_; }
private:
    /**
     *  Picks one of four opcodes based on operand types.
     *  (N.B.: We assume that the LHS and RHS operands are of the same type!)
     */
    Instruction determineOpCode(const Instruction float_op_code, const Instruction string_op_code,
				const Instruction boolean_op_code, const Instruction int_op_code) const;
};
 

} // namespace Nyaa


#endif // ifndef NYAA_NODES_H
