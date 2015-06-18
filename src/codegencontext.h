#ifndef CODEGENCONTEXT_H
#define CODEGENCONTEXT_H
#include "builder.h"

namespace noise {

class CodeGenContext
{
public:
    CodeGenContext(Builder &builder);
    ~CodeGenContext();

    llvm::StructType *GetContextType();

    llvm::Value *CreateAllocaContext();
    uint32_t CreateVariable(llvm::Type *type, llvm::Constant *defaultValue);

    llvm::Value *CreateLoadVariable(llvm::Value *codeGenContext, uint32_t id) const;
    llvm::Value *CreateStoreVariable(llvm::Value *codeGenContext, uint32_t id, llvm::Value *value) const;
    llvm::Type *GetVariableType(uint32_t id) const;


private:
    struct Var {
        llvm::Type *m_Type;
        llvm::Constant *m_DefaultValue;
    };

    Builder &m_Builder;
    std::vector<Var> m_Variables;

};

}
#endif // CODEGENCONTEXT_H
