#include "jitnoise/module/spheres.h"
#include "codegenerator.h"
#include "builder.h"
using namespace noise::module;
using namespace noise;


class SpheresCodeGenerator : public CodeGenerator
{
public:
    SpheresCodeGenerator(Spheres *a) : m_Module(a) {}
    llvm::Value *generate(Builder &b, llvm::Value *x, llvm::Value *y, llvm::Value *z, llvm::Value *codeGenContext) {
        x = b.CreateFMul(x, b.getFPValue(m_Module->GetFrequency()));
        y = b.CreateFMul(y, b.getFPValue(m_Module->GetFrequency()));
        z = b.CreateFMul(z, b.getFPValue(m_Module->GetFrequency()));

        llvm::Value *distFromCenter = b.CreateFSqrt(b.CreateFAdd(b.CreateFMul(x, x), b.CreateFAdd(b.CreateFMul(y, y), b.CreateFMul(z, z))));
        llvm::Value *distFromSmallerSphere = b.CreateFSub(distFromCenter, b.CreateFloor(distFromCenter));
        llvm::Value *distFromLargerSphere = b.CreateFSub(b.getFPValue(1.0), distFromSmallerSphere);
        llvm::Value *nearestDist = b.CreateFMin(distFromSmallerSphere, distFromLargerSphere);
        return b.CreateFSub(b.getFPValue(1.0), b.CreateFMul(nearestDist, b.getFPValue(4.0)));
    }
private:
    Spheres *m_Module;
};


Spheres::Spheres ():
  Module (GetSourceModuleCount (), new SpheresCodeGenerator(this)),
  m_frequency (DEFAULT_SPHERES_FREQUENCY)
{
}

