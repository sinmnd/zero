#ifndef AST_H
#define AST_H

#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"

class AST;
class Expr;
class Factor;
class BinaryOp;
class WithDecl;

class ASTVisitor {
    public:
        virtual void visit(AST &) {};
        virtual void visit(Expr &) {};
        virtual void visit(Factor &) = 0;
        virtual void visit(BinaryOp &) = 0;
        virtual void visit(WithDecl &) = 0;
};

class AST {
    public:
        virtual ~AST() {}
        virtual void accept(ASTVisitor &) = 0;
};

class Expr : public AST {
    public:
        Expr() {}
};

class Factor : public Expr {
    public:
        enum ValueKind {Ident, Number};
    
    private:
        ValueKind kind;
        llvm::StringRef val;

    public:
        Factor(ValueKind kind, llvm::StringRef val) : kind(kind), val(val) {}
        ValueKind getKind() {
            return kind;
        }
        llvm::StringRef getVal() {
            return val;
        }
        virtual void accept(ASTVisitor &v) override {
            v.visit(*this);
        }
};

class BinaryOp : public Expr {
    public:
        enum Operator {Add, Sub, Mul, Div};

    private:
        Expr *left;
        Expr *right;
        Operator op;
    
    public:
        BinaryOp(Operator op, Expr *l, Expr *r) : op(op), left(l), right(r) {}
        Expr *getLeft() {
            return left;
        }
        Expr *getRight() {
            return right;
        }
        Operator getOperator() {
            return op;
        }
        virtual void accept(ASTVisitor &v) override {
            v.visit(*this);
        }
};

class WithDecl : public AST {
    using VarList = llvm::SmallVector<llvm::StringRef, 8>;
    VarList vars;
    Expr *expr;

    public:
        WithDecl(VarList vars, Expr *expr) : vars(vars), expr(expr) {};
        VarList::const_iterator begin() {
            return vars.begin();
        }
        VarList::const_iterator end() {
            return vars.end();
        }
        Expr *getExpr() {
            return expr;
        }
        virtual void accept(ASTVisitor &v) override {
            v.visit(*this);
        }
};

#endif