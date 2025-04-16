#include "codegen.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace {
    class ToIRVisitor : public ASTVisitor {
        Module *md;
        IRBuilder<> builder;
        Type *voidTy;
        Type *int32Ty;
        PointerType *ptrTy;
        Constant *int32Zero;
        Value *v;
        StringMap<Value *> nameMap;

        public:
            ToIRVisitor(Module *m) : md(m), builder(m->getContext()) {
                voidTy = Type::getVoidTy(m->getContext());
                int32Ty = Type::getInt32Ty(m->getContext());
                ptrTy = PointerType::getUnqual(m->getContext());
                int32Zero = ConstantInt::get(int32Ty, 0, true);
            }

            void run(AST *tree) {
                FunctionType *mainFTy = FunctionType::get(int32Ty, {int32Ty, ptrTy}, false);
                Function *mainF = Function::Create(mainFTy, GlobalValue::ExternalLinkage, "main", md);
                BasicBlock *bB = BasicBlock::Create(md->getContext(), "entry", mainF);
                builder.SetInsertPoint(bB);
                tree->accept(*this);
                FunctionType *calcWriteFTy = FunctionType::get(voidTy, {int32Ty}, false);
                Function *calcWriteF = Function::Create(calcWriteFTy, GlobalValue::ExternalLinkage, "calc_write", md);
                builder.CreateCall(calcWriteFTy, calcWriteF, {v});
                builder.CreateRet(int32Zero);
            }

            virtual void visit(WithDecl &node) override {
                FunctionType *calcReadFTy = FunctionType::get(int32Ty, {ptrTy}, false);
                Function *calcReadF = Function::Create(calcReadFTy, GlobalValue::ExternalLinkage, "calc_read", md);
                for(auto i = node.begin(), e = node.end(); i != e; ++i) {
                    StringRef var = *i;
                    Constant *varText = ConstantDataArray::getString(md->getContext(), var);
                    GlobalVariable *varGV = new GlobalVariable(*md, varText->getType(), true, 
                        GlobalValue::PrivateLinkage, varText, Twine(var).concat(".var"));
                    CallInst *call = builder.CreateCall(calcReadFTy, calcReadF, {varGV});
                    nameMap[var] = call;
                }
                node.getExpr()->accept(*this);
            }

            virtual void visit(Factor &node) override {
                if(node.getKind() == Factor::Ident) {
                    v = nameMap[node.getVal()];
                } else {
                    int intval;
                    node.getVal().getAsInteger(10, intval);
                    v = ConstantInt::get(int32Ty, intval, true);
                }
            }

            virtual void visit(BinaryOp &node) override {
                node.getLeft()->accept(*this);
                Value *left = v;
                node.getRight()->accept(*this);
                Value *right = v;
                switch(node.getOperator()) {
                    case BinaryOp::Add:
                        v = builder.CreateNSWAdd(left, right);
                        break;
                    case BinaryOp::Sub:
                        v = builder.CreateNSWSub(left, right);
                        break;
                    case BinaryOp::Mul:
                        v = builder.CreateNSWMul(left, right);
                        break;
                    case BinaryOp::Div:
                        v = builder.CreateSDiv(left, right);
                        break;
                }
            }
    };
}

void CodeGen::compile(AST *tree) {
    LLVMContext context;
    Module *m = new Module("calc.expr", context);
    ToIRVisitor visitor(m);
    visitor.run(tree);
    m->print(outs(), nullptr);
}