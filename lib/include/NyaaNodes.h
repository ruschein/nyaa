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
#include "NyaaFunction.h"
#include "NyaaInstructions.h"
#include "NyaaToken.h"


namespace Nyaa {


// Forward declaration:
class CodeAndSourceLocation;


class TreeNode {
public:
    virtual ~TreeNode() { }

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

    virtual NodeType getType() const final { return operator_.isCompOp() ? NodeType::BOOLEAN_NODE : lhs_->getType(); }

    /** \return the left operand */
    virtual inline const TreeNode *getLeftChild() const final { return lhs_; }

    /** \return the right operand */
    virtual inline const TreeNode *getRightChild() const final { return rhs_; }

    virtual void genCode(std::stack<CodeAndSourceLocation> * const code_stack) const final;

    inline const Token &getOperator() const { return operator_; }
private:
    /**
     *  Picks one of four opcodes based on operand types.
     *  (N.B.: We assume that the LHS and RHS operands are of the same type!)
     */
    Instruction determineOpCode(const Instruction float_op_code, const Instruction string_op_code,
				const Instruction boolean_op_code, const Instruction int_op_code) const;
};


/**
 *  A node in the parse tree representing an integer constant.
 */
class BooleanConstantNode: public AbstractNode {
    bool value_;
public:
    BooleanConstantNode(const size_t source_location, const bool value)
        : AbstractNode(source_location), value_(value) { }

    virtual inline std::string toString() const final { return "BooleanConstantNode: " + std::to_string(value_); }

    virtual inline NodeType getType() const final { return NodeType::BOOLEAN_NODE; }

    /**
     *  \return null, This type of node never has any children!
     */
    virtual inline const TreeNode *getLeftChild() const final { return nullptr; }

    /**
     *  \return null, This type of node never has any children!
     */
    virtual inline const TreeNode *getRightChild() const final { return nullptr; }

    inline bool getValue() const { return value_; }

    virtual void genCode(std::stack<CodeAndSourceLocation> * const code_stack) const final;
};


/**
 *  A node in the parse tree representing a floating point constant.
 */
class FloatConstantNode: public AbstractNode {
    double value_;
public:
    FloatConstantNode(const size_t source_location, const double value)
        : AbstractNode(source_location), value_(value) { }

    virtual inline std::string toString() const final { return "FloatConstantNode: " + std::to_string(value_); }

    virtual inline NodeType getType() const final { return NodeType::FLOAT_NODE; }

    /**
     *  \return null, This type of node never has any children!
     */
    virtual inline const TreeNode *getLeftChild() const final { return nullptr; }

    /**
     *  \return null, This type of node never has any children!
     */
    virtual inline const TreeNode *getRightChild() const final { return nullptr; }

    inline double getValue() const { return value_; }

    virtual void genCode(std::stack<CodeAndSourceLocation> * const code_stack) const final;
};


/**
 *  A node in the parse tree representing a function call.
 */
class FuncCallNode: public AbstractNode {
    const Function &func_;
    NodeType return_type_;
    std::vector<TreeNode *> args_;
public:
    FuncCallNode(const size_t source_location, const Function &func, const NodeType return_type, const std::vector<TreeNode *> &args);
    virtual ~FuncCallNode();

    virtual inline std::string toString() const final {
        return "FuncCallNode: call to " + func_.getName() + " with " + std::to_string(args_.size()) + " args";
    }

    virtual inline NodeType getType() const final { return return_type_; }

    /**
     *  \return nullptr, This type of node never has any children!
     */
    virtual inline const TreeNode *getLeftChild() const final { return nullptr; }

    /**
     *  \return nullptr, This type of node never has any children!
     */
    virtual inline const TreeNode *getRightChild() const final { return nullptr; }

    virtual void genCode(std::stack<CodeAndSourceLocation> * const code_stack) const final {
        for (int i(args_.size() - 1); i >= 0; --i)
            args_[i]->genCode(code_stack);
        code_stack->push(CodeAndSourceLocation(args_.size(), -1));
        code_stack->push(CodeAndSourceLocation(func_, -1));
        code_stack->push(CodeAndSourceLocation(Instruction::CALL, getSourceLocation()));
    }
};


/**
 *  A node in the parse tree representing an attribute reference.
 */
class IdentNode: public AbstractNode {
    const std::string attrib_name_;
    const std::shared_ptr<AbstractNode> default_value_;
    const NodeType type_;
public:
    IdentNode(const size_t source_location, const std::string &attrib_name,
	      const std::shared_ptr<AbstractNode> default_value, const NodeType type)
        : AbstractNode(source_location), attrib_name_(attrib_name), default_value_(default_value), type_(type)
    {
        if (type == NodeType::NULL_NODE)
	  throw std::invalid_argument("in IdentNode::IdentNode: \"type\" must not be NULL_NODE.");
        if (default_value != nullptr and default_value->getType() != type)
            throw std::invalid_argument("in IdentNode::IdentNode: default value must match \"type\".");
    }

    virtual inline std::string toString() const final {
        return "IdentNode: " + attrib_name_ + (default_value_ == nullptr ? "" : " default=" + default_value_->toString());
    }

    inline NodeType getType() const final { return type_; }

    /**
     *  \return nullptr, This type of node never has any children!
     */
    inline const TreeNode *getLeftChild() const final { return nullptr; }

    /**
     *  \return nullptr, This type of node never has any children!
     */
    inline const TreeNode *getRightChild() const final { return nullptr; }

    inline const std::string &getAttribName() const { return attrib_name_; }
    inline const AbstractNode *getDefaultValue() const { return default_value_.get(); }

    virtual void genCode(std::stack<CodeAndSourceLocation> * const code_stack) const final {
        if (default_value_ != nullptr)
            code_stack->push(CodeAndSourceLocation(default_value_, -1));
        code_stack->push(CodeAndSourceLocation(attribName, -1));
        code_stack->push(CodeAndSourceLocation(default_value_ == nullptr ? Instruction::AREF : Instruction::AREF2, getSourceLocation()));
    }
};


/**
 *  A node in the parse tree representing a conversion to a string
 */
class SConvNode: public AbstractNode {
    const std::shared_ptr<AbstractNode> convertee_;
public:
    SConvNode(const std::shared_ptr<AbstractNode> convertee)
        : AbstractNode(-1 /* Type conversions are generated by the compiler and do not correspond to actual source locations! */),
          convertee_(convertee)
    {
        if (convertee == nullptr)
            throw std::invalid_argument("in SConvNode::SConvNode: convertee must not be nullptr.");

        const NodeType type(convertee_->getType());
        if (type != NodeType::FLOAT_NODE and type != NodeType::INT_NODE and type != NodeType::BOOLEAN_NODE)
            throw std::invalid_argument("in SConvNode::SConvNode: convertee must be of type FLOAT, INT, or BOOLEAN.");
    }

    virtual inline std::string toString() const final {
        return "SConvNode: convertee = " + convertee_->toString();
    }

    inline NodeType getType() const final { return NodeType::STRING_NODE; }

    /**
     *  \return the only child of this node
     */
    inline const TreeNode *getLeftChild() const final { return convertee_.get(); }

    /**
     *  \return nullptr, This type of node never has any right children!
     */
    inline const TreeNode *getRightChild() const final { return nullptr; }

    void genCode(std::stack<CodeAndSourceLocation> * const code_stack) const final {
        convertee_->genCode(code_stack);

        const NodeType type(convertee_->getType());
        if (type == NodeType::FLOAT_NODE)
            code_stack->push(CodeAndSourceLocation(Instruction::SCONVF, getSourceLocation()));
        else if (type == NodeType::INT_NODE)
            code_stack->push(CodeAndSourceLocation(Instruction::SCONVI, getSourceLocation()));
        else if (type == NodeType::BOOLEAN_NODE)
            code_stack->push(CodeAndSourceLocation(Instruction::SCONVB, getSourceLocation()));
        else
            throw std::range_error("unknown node type: " + type + ".");
    }
};


/**
 *  A node in the parse tree representing a unary operator application.
 */
public class UnaryOpNode extends AbstractNode {
    private final Token operator;
    private final TreeNode operand;

    public UnaryOpNode(final int sourceLocation, final Token operator, final TreeNode operand) {
        super(sourceLocation);

        if (operand == nullptr)
            throw new IllegalArgumentException("operand must not be nullptr.");

        this.operator = operator;
        this.operand = operand;
    }

    public String toString() { return "UnaryOpNode: " + operator; }

    inline NodeType getType() const final { return operand.getType(); }

    /**
     *  \return the operand
     */
    inline const TreeNode *getLeftChild() const final { return operand; }

    /**
     *  \return nullptr, This type of node never has any left children!
     */
    inline const TreeNode *getRightChild() const final { return nullptr; }

    public Token getOperator() { return operator; }

    public void genCode(final Stack<CodeAndSourceLocation> code_stack) {
        operand.genCode(code_stack);

        switch (operator) {
        case PLUS:
            code_stack.push(new CodeAndSourceLocation(Instruction.FUPLUS, getSourceLocation()));
            break;
        case MINUS:
            code_stack.push(new CodeAndSourceLocation(Instruction.FUMINUS, getSourceLocation()));
            break;
        default:
            throw new IllegalStateException("invalid unary operation: " + operator + ".");
        }
    }
};


/**
 *  A node in the parse tree representing a conversion to a floating point number
 */
public class FConvNode extends AbstractNode {
    private final TreeNode convertee;

    public FConvNode(final TreeNode convertee) {
        super(-1); // Type conversions are generated by the compiler and do not correspond to actual source locations!

        if (convertee == nullptr)
            throw new IllegalArgumentException("convertee must not be nullptr.");

        final Class type = convertee.getType();
        if (type != Long.class && type != Boolean.class && type != String.class)
            throw new IllegalArgumentException("convertee must be of type Long, Boolean or String.");

        this.convertee = convertee;
    }

    public String toString() {
        return "FConvNode: convertee = " + convertee;
    }

    inline NodeType getType() const final { return Double.class; }

    /**
     *  \return the only child of this node
     */
    inline const TreeNode *getLeftChild() const final { return convertee; }

    /**
     *  \return nullptr, This type of node never has any right children!
     */
    inline const TreeNode *getRightChild() const final { return nullptr; }

    public void genCode(final Stack<CodeAndSourceLocation> code_stack) {
        convertee.genCode(code_stack);

        final Class type = convertee.getType();
        if (type == Long.class)
            code_stack.push(new CodeAndSourceLocation(Instruction.FCONVI, getSourceLocation()));
        else if (type == Boolean.class)
            code_stack.push(new CodeAndSourceLocation(Instruction.FCONVB, getSourceLocation()));
        else if (type == String.class)
            code_stack.push(new CodeAndSourceLocation(Instruction.FCONVS, getSourceLocation()));
        else
            throw new IllegalStateException("unknown type: " + type + ".");
    }
};


/**
 *  A node in the parse tree representing an integer constant.
 */
class StringConstantNode: public AbstractNode {
    const std::string value_;
public:
   inline StringConstantNode(const size_t source_location, const std::string &value)
        : AbstractNode(source_location), value_(value) { }

    virtual inline std::string toString() const final { return "StringConstantNode: " + value_; }

    virtual inline NodeType getType() const final { return NodeType::STRING_NODE; }

    /**
     *  \return nullptr, This type of node never has any children!
     */
    virtual inline const TreeNode *getLeftChild() const final { return nullptr; }

    /**
     *  \return nullptr, This type of node never has any children!
     */
    virtual inline const TreeNode *getRightChild() const final { return nullptr; }

    inline const std::string &getValue() const { return value_; }

    virtual inline void genCode(std::stack<CodeAndSourceLocation> * const code_stack) const final {
        code_stack->push(CodeAndSourceLocation(value_, getSourceLocation()));
    }
};


} // namespace Nyaa


#endif // ifndef NYAA_NODES_H
