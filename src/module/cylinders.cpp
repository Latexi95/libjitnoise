#include "jitnoise/module/cylinders.h"
#include "codegenerator.h"
#include "builder.h"
using namespace noise::module;
using namespace noise;


class CylindersCodeGenerator : public CodeGenerator
{
public:
    CylindersCodeGenerator(Cylinders *a) : m_Module(a) {}
    llvm::Value *generate(Builder &b, llvm::Value *x, llvm::Value *, llvm::Value *z, llvm::Value *) {
        x = b.CreateFMul(x, b.getFPValue(m_Module->GetFrequency()));
        z = b.CreateFMul(z, b.getFPValue(m_Module->GetFrequency()));

        llvm::Value *distFromCenter = b.CreateFSqrt(b.CreateFAdd(b.CreateFMul(x, x), b.CreateFMul(z, z)));
        llvm::Value *distFromSmallerSphere = b.CreateFSub(distFromCenter, b.CreateFloor(distFromCenter));
        llvm::Value *distFromLargerSphere = b.CreateFSub(b.getFPValue(1.0), distFromSmallerSphere);
        llvm::Value *nearestDist = b.CreateFMin(distFromSmallerSphere, distFromLargerSphere);
        return b.CreateFSub(b.getFPValue(1.0), b.CreateFMul(nearestDist, b.getFPValue(4.0)));
    }
private:
    Cylinders *m_Module;
};

Cylinders::Cylinders ():
  Module (GetSourceModuleCount (), new CylindersCodeGenerator(this)),
  m_frequency (DEFAULT_CYLINDERS_FREQUENCY)
{
}

