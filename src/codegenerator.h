#ifndef CODEGENERATOR_H
#define CODEGENERATOR_H
#include "llvm.h"
#include "builder.h"
namespace noise {

class CodeGenerator
{
public:
    CodeGenerator();
    virtual ~CodeGenerator();

    virtual llvm::Value *generate(Builder &, llvm::Value *x, llvm::Value *y, llvm::Value *z, llvm::Value *codeGenContext) = 0;
    virtual void initializeContext(Builder &) { }
};

}

#endif // CODEGENERATOR_H
