
#include "jitnoise/module/invert.h"
#include "codegenerator.h"
using namespace noise::module;
using namespace noise;

class InvertCodeGenerator : public CodeGenerator
{
public:
    InvertCodeGenerator(Invert *a) : m_Module(a) {}
    llvm::Value *generate(Builder &builder, llvm::Value *x, llvm::Value *y, llvm::Value *z, llvm::Value *codeGenContext) {
        llvm::Value *sourceVal = m_Module->GetSourceModule(0).GetCodeGenerator()->generate(builder, x, y, z, codeGenContext);
        return builder.CreateFNeg(sourceVal);
    }
private:
    Invert *m_Module;
};

Invert::Invert ():
    Module (GetSourceModuleCount (), new InvertCodeGenerator(this))
{
}

