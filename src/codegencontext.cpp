#include "codegencontext.h"
using namespace noise;
CodeGenContext::CodeGenContext(Builder &builder) :
    m_Builder(builder)
{

}



noise::CodeGenContext::~CodeGenContext()
{

}

llvm::StructType *CodeGenContext::GetContextType()
{
    if (m_Variables.empty()) {
        CreateVariable(m_Builder.getInt1Ty(), m_Builder.getInt1(false));
    }
    std::vector<llvm::Type*> elems;
    for (auto &var : m_Variables) {
        elems.push_back(var.m_Type);
    }

    return llvm::StructType::get(m_Builder.getContext(), elems );
}

llvm::Value *CodeGenContext::CreateAllocaContext()
{
    llvm::StructType *type = GetContextType();
    llvm::Value *alloc = m_Builder.CreateAlloca(type);

    std::vector<llvm::Constant*> elems;
    for (auto &var : m_Variables) {
        elems.push_back(var.m_DefaultValue);
    }
    m_Builder.CreateStore(llvm::ConstantStruct::get(type, elems), alloc);
    return alloc;
}

uint32_t CodeGenContext::CreateVariable(llvm::Type *type, llvm::Constant *defaultValue)
{
    Var var;
    var.m_Type = type;
    var.m_DefaultValue = defaultValue;
    m_Variables.push_back(var);
    return m_Variables.size() - 1;
}

llvm::Value *CodeGenContext::CreateLoadVariable(llvm::Value *codeGenContext, uint32_t id) const
{
    return m_Builder.CreateLoad(m_Builder.CreateGEP(codeGenContext, std::vector<llvm::Value*>({m_Builder.getInt32(0), m_Builder.getInt32(id)})));
}

llvm::Value *CodeGenContext::CreateStoreVariable(llvm::Value *codeGenContext, uint32_t id, llvm::Value *value) const
{
    return m_Builder.CreateStore(value, m_Builder.CreateGEP(codeGenContext, std::vector<llvm::Value*>({m_Builder.getInt32(0), m_Builder.getInt32(id)})));
}

llvm::Type *CodeGenContext::GetVariableType(uint32_t id) const
{
    return m_Variables[id].m_Type;
}
