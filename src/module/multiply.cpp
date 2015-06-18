#include "jitnoise/module/multiply.h"
#include "codegenerator.h"
#include "builder.h"
using namespace noise::module;
using namespace noise;


class MultiplyCodeGenerator : public CodeGenerator
{
public:
    MultiplyCodeGenerator(Multiply *a) : m_Module(a) {}
    llvm::Value *generate(Builder &builder, llvm::Value *x, llvm::Value *y, llvm::Value *z, llvm::Value *codeGenContext) {
        llvm::Value *source0 = m_Module->GetSourceModule(0).GetCodeGenerator()->generate(builder, x, y, z, codeGenContext);
        llvm::Value *source1 = m_Module->GetSourceModule(1).GetCodeGenerator()->generate(builder, x, y, z, codeGenContext);

        return builder.CreateFMul(source0, source1);
    }
private:
    Multiply *m_Module;
};


Multiply::Multiply ():
  Module (GetSourceModuleCount (), new MultiplyCodeGenerator(this))
{
}

