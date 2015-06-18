
#include "jitnoise/module/abs.h"
#include "codegenerator.h"
using namespace noise::module;
using namespace noise;

class AbsCodeGenerator : public CodeGenerator
{
public:
    AbsCodeGenerator(Abs *a) : m_Module(a) {}
    llvm::Value *generate(Builder &b, llvm::Value *x, llvm::Value *y, llvm::Value *z, llvm::Value *codeGenContext) {
        llvm::Value *sourceVal = m_Module->GetSourceModule(0).GetCodeGenerator()->generate(b, x, y, z, codeGenContext);
        return b.CreateFAbs(sourceVal);
    }
private:
    Abs *m_Module;
};

Abs::Abs ():
    Module (GetSourceModuleCount (), new AbsCodeGenerator(this))
{
}

