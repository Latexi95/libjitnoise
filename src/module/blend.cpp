#include "jitnoise/module/blend.h"
#include "codegenerator.h"
#include "builder.h"
using namespace noise::module;
using namespace noise;


class BlendCodeGenerator : public CodeGenerator
{
public:
    BlendCodeGenerator(Blend *a) : m_Module(a), m_Function(0) {}
    llvm::Value *generate(Builder &b, llvm::Value *x, llvm::Value *y, llvm::Value *z, llvm::Value *codeGenContext) {

        if (!m_Function) {
            llvm::Value *x, *y, *z, *codeGenContext;
            m_Function = b.CreateModuleFunction(x, y, z, codeGenContext);

            Builder::InsertPoint ip = b.saveAndClearIP();

            llvm::BasicBlock *entryBB = llvm::BasicBlock::Create(b.getContext(), "entry", m_Function);

            b.SetInsertPoint(entryBB);

            llvm::Value *v0 = m_Module->GetSourceModule(0).GetCodeGenerator()->generate(b, x, y, z, codeGenContext);
            llvm::Value *v1 = m_Module->GetSourceModule(1).GetCodeGenerator()->generate(b, x, y, z, codeGenContext);
            llvm::Value *alpha = b.CreateFDiv(b.CreateFAdd(m_Module->GetSourceModule(2).GetCodeGenerator()->generate(b, x, y, z, codeGenContext), b.getFPValue(1.0)), b.getFPValue(2.0));

            b.CreateRet(b.CreateLinearInterp(v0, v1, alpha));

            b.restoreIP(ip);
        }




        return b.CreateCallModuleFunction(m_Function, x, y, z, codeGenContext);
    }
private:
    Blend *m_Module;
    llvm::Function *m_Function;
};


Blend::Blend ():
    Module (GetSourceModuleCount (), new BlendCodeGenerator(this))
{
}
