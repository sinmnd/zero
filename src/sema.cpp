#include "sema.h"
#include "llvm/ADT/StringSet.h"
#include "llvm/Support/raw_ostream.h"

namespace {
    class DeclCheck : public ASTVisitor {
        llvm::StringSet<> scope;
        bool state;
        enum ErrorType {
            Twice, Not
        };
        void error(ErrorType et, llvm::StringRef v) {
            llvm::errs() << "Variable " << v << " " << (et == Twice ? "already" : "not") << " declared\n";
            state = false;
        }
        public:
            DeclCheck() : state(true) {}
            bool hasError() { return !state; }
            virtual void visit(Factor &node) override {
                if(node.getKind() == Factor::Ident) {
                    if(scope.find(node.getVal()) == scope.end()) {
                        error(Not, node.getVal());
                    }
                }
            }

            virtual void visit(BinaryOp &node) override {
                if(node.getLeft()) {
                    node.getLeft()->accept(*this);
                }
                else {
                    state = false;
                }
                if(node.getRight()) {
                    node.getRight()->accept(*this);
                }
                else {
                    state = false;
                }
            }

            virtual void visit(WithDecl &node) override {
                for(auto i = node.begin(), e = node.end(); i != e; ++i) {
                    if(!scope.insert(*i).second) {
                        error(Twice, *i);
                    }
                }
                if (node.getExpr()) {
                    node.getExpr()->accept(*this);
                }
                else {
                    state = false;
                }
            }
    };
        
}

bool Sema::semantic(AST *tree) {
    if (!tree) {
        return false;
    }
    DeclCheck dc;
    tree->accept(dc);
    return dc.hasError();
}