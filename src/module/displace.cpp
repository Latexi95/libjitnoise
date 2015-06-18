#include "jitnoise/module/displace.h"
#include "codegenerator.h"
#include "builder.h"
using namespace noise::module;
using namespace noise;


class DisplaceCodeGenerator : public CodeGenerator
{
public:
    DisplaceCodeGenerator(Displace *a) : m_Module(a) {}
    llvm::Value *generate(Builder &b, llvm::Value *x, llvm::Value *y, llvm::Value *z, llvm::Value *codeGenContext) {
        llvm::Value *xDisplace = b.CreateFAdd(x, m_Module->GetSourceModule(1).GetCodeGenerator()->generate(b, x, y, z, codeGenContext));
        llvm::Value *yDisplace = b.CreateFAdd(x, m_Module->GetSourceModule(2).GetCodeGenerator()->generate(b, x, y, z, codeGenContext));
        llvm::Value *zDisplace = b.CreateFAdd(x, m_Module->GetSourceModule(3).GetCodeGenerator()->generate(b, x, y, z, codeGenContext));

        return m_Module->GetSourceModule(0).GetCodeGenerator()->generate(b, xDisplace, yDisplace, zDisplace, codeGenContext);
    }
private:
    Displace *m_Module;
};

using namespace noise::module;

Displace::Displace ():
    Module (GetSourceModuleCount (), new DisplaceCodeGenerator(this))
{
}


