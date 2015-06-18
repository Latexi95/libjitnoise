#include "jitnoise/module/checkerboard.h"
#include "codegenerator.h"
#include "builder.h"
using namespace noise::module;
using namespace noise;


class CheckerboardCodeGenerator : public CodeGenerator
{
public:
    CheckerboardCodeGenerator(Checkerboard *a) : m_Module(a) {}
    llvm::Value *generate(Builder &b, llvm::Value *x, llvm::Value *y, llvm::Value *z, llvm::Value *) {
        llvm::Value *ix = b.CreateFPToSI(b.CreateFloor(b.CreateMakeInt32Range(x)), b.getIType());
        llvm::Value *iy = b.CreateFPToSI(b.CreateFloor(b.CreateMakeInt32Range(y)), b.getIType());
        llvm::Value *iz = b.CreateFPToSI(b.CreateFloor(b.CreateMakeInt32Range(z)), b.getIType());
        llvm::Value *ixt = b.CreateTrunc(ix, llvm::VectorType::get(b.getInt1Ty(), VECTOR_ELEMENT_COUNT));
        llvm::Value *iyt = b.CreateTrunc(iy, llvm::VectorType::get(b.getInt1Ty(), VECTOR_ELEMENT_COUNT));
        llvm::Value *izt = b.CreateTrunc(iz, llvm::VectorType::get(b.getInt1Ty(), VECTOR_ELEMENT_COUNT));

        llvm::Value *v = b.CreateXor(ixt, b.CreateXor(iyt, izt));
        return b.CreateSelect(v, b.getFPValue(-1.0), b.getFPValue(1.0));
    }
private:
    Checkerboard *m_Module;
};

Checkerboard::Checkerboard ():
  Module (GetSourceModuleCount (), new CheckerboardCodeGenerator(this))
{
}

