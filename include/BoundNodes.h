#pragma once

#include "ConstantValue.h"
#include "Symbol.h"

namespace slang {

class TypeSymbol;

enum class BoundNodeKind {
    Unknown,
    Literal,
    Parameter,
    Variable,
    UnaryExpression,
    BinaryExpression,
    TernaryExpression,
    AssignmentExpression,
    CallExpression,
    StatementList,
    ReturnStatement,
    VariableDeclaration,
    ConditionalStatement
};

class BoundNode {
public:
    BoundNodeKind kind;

    BoundNode(BoundNodeKind kind) : kind(kind) {}

    bool bad() const { return kind == BoundNodeKind::Unknown; }
};

class BoundExpression : public BoundNode {
public:
    const ExpressionSyntax* syntax;
    const TypeSymbol* type;

    BoundExpression(BoundNodeKind kind, const ExpressionSyntax* syntax, const TypeSymbol* type) :
        BoundNode(kind), syntax(syntax), type(type) {}
};

class BadBoundExpression : public BoundExpression {
public:
    BoundExpression* child;

    BadBoundExpression(BoundExpression* child) :
        BoundExpression(BoundNodeKind::Unknown, nullptr, nullptr), child(child) {}
};

class BoundLiteral : public BoundExpression {
public:
    ConstantValue value;

    BoundLiteral(const ExpressionSyntax* syntax, const TypeSymbol* type, const ConstantValue& value) :
        BoundExpression(BoundNodeKind::Literal, syntax, type), value(value) {}

    BoundLiteral(const ExpressionSyntax* syntax, const TypeSymbol* type, ConstantValue&& value) :
        BoundExpression(BoundNodeKind::Literal, syntax, type), value(std::move(value)) {}
};

class BoundParameter : public BoundExpression {
public:
    const ParameterSymbol& symbol;

    BoundParameter(const ExpressionSyntax* syntax, const ParameterSymbol& symbol) :
        BoundExpression(BoundNodeKind::Parameter, syntax, symbol.type), symbol(symbol) {}
};

class BoundVariable : public BoundExpression {
public:
    const VariableSymbol* symbol;

    BoundVariable(const ExpressionSyntax* syntax, const VariableSymbol* symbol) :
        BoundExpression(BoundNodeKind::Variable, syntax, symbol->type), symbol(symbol) {}
};

class BoundUnaryExpression : public BoundExpression {
public:
    BoundExpression* operand;

    BoundUnaryExpression(const ExpressionSyntax* syntax, const TypeSymbol* type, BoundExpression* operand) :
        BoundExpression(BoundNodeKind::UnaryExpression, syntax, type), operand(operand) {}
};

class BoundBinaryExpression : public BoundExpression {
public:
    BoundExpression* left;
    BoundExpression* right;

    BoundBinaryExpression(const ExpressionSyntax* syntax, const TypeSymbol* type, BoundExpression* left, BoundExpression* right) :
        BoundExpression(BoundNodeKind::BinaryExpression, syntax, type), left(left), right(right) {}
};

class BoundAssignmentExpression : public BoundExpression {
public:
    BoundExpression* left;
    BoundExpression* right;

    BoundAssignmentExpression(const ExpressionSyntax* syntax, const TypeSymbol* type, BoundExpression* left, BoundExpression* right) :
        BoundExpression(BoundNodeKind::AssignmentExpression, syntax, type), left(left), right(right) {}
};

class BoundCallExpression : public BoundExpression {
public:
    const SubroutineSymbol* subroutine;
    ArrayRef<const BoundExpression*> arguments;

    BoundCallExpression(const ExpressionSyntax* syntax, const SubroutineSymbol* subroutine, ArrayRef<const BoundExpression*> arguments) :
        BoundExpression(BoundNodeKind::CallExpression, syntax, subroutine->returnType),
        subroutine(subroutine), arguments(arguments) {}
};

class BoundTernaryExpression : public BoundExpression {
public:
    BoundExpression* pred;
    BoundExpression* left;
    BoundExpression* right;

    BoundTernaryExpression(const ExpressionSyntax* syntax, const TypeSymbol* type, BoundExpression* pred, BoundExpression* left, BoundExpression* right) :
        BoundExpression(BoundNodeKind::TernaryExpression, syntax, type), pred(pred), left(left), right(right) {}

};

class BoundStatement : public BoundNode {
public:
    const StatementSyntax* syntax;

    BoundStatement(BoundNodeKind kind, const StatementSyntax* syntax) :
        BoundNode(kind), syntax(syntax) {}
};

class BadBoundStatement : public BoundStatement {
public:
    BoundStatement* child;

    BadBoundStatement(BoundStatement* child) :
        BoundStatement(BoundNodeKind::Unknown, nullptr), child(child) {}
};

class BoundStatementList : public BoundStatement {
public:
    ArrayRef<const BoundStatement*> list;

    BoundStatementList(ArrayRef<const BoundStatement*> list) :
        BoundStatement(BoundNodeKind::StatementList, nullptr), list(list) {}
};

class BoundReturnStatement : public BoundStatement {
public:
    const BoundExpression* expr;

    BoundReturnStatement(const StatementSyntax* syntax, const BoundExpression* expr) :
        BoundStatement(BoundNodeKind::ReturnStatement, syntax), expr(expr) {}
};

class BoundVariableDecl : public BoundStatement {
public:
    const VariableSymbol* symbol;

    BoundVariableDecl(const VariableSymbol* symbol) :
        BoundStatement(BoundNodeKind::VariableDeclaration, nullptr), symbol(symbol) {}
};

class BoundConditionalStatement : public BoundStatement {
public:
    const BoundExpression* cond;
    const BoundStatement* ifTrue;
    const BoundStatement* ifFalse;

    BoundConditionalStatement(const StatementSyntax *syntax, const BoundExpression* cond,
                              const BoundStatement* ifTrue, const BoundStatement* ifFalse) :
        BoundStatement(BoundNodeKind::ConditionalStatement, syntax),
        cond(cond), ifTrue(ifTrue), ifFalse(ifFalse) {}
};

}
