#include "jitnoise/module/voronoi.h"
#include "codegenerator.h"
#include "builder.h"
#include "jitnoise/mathconsts.h"
using namespace noise::module;
using namespace noise;


class VoronoiCodeGenerator : public CodeGenerator
{
public:
    VoronoiCodeGenerator(Voronoi *a) : m_Module(a), m_Function(0) {}
    llvm::Value *generate(Builder &b, llvm::Value *x, llvm::Value *y, llvm::Value *z, llvm::Value *codeGenContext) {
        if (!m_Function) {
            llvm::Value *x, *y, *z, *codeGenContext;
            m_Function = b.CreateModuleFunction(x, y, z, codeGenContext);

            Builder::InsertPoint ip = b.saveAndClearIP();
            llvm::BasicBlock *entryBB = llvm::BasicBlock::Create(b.getContext(), "entry", m_Function);
            b.SetInsertPoint(entryBB);

            x = b.CreateFMul(x, b.getFPValue(m_Module->GetFrequency()));
            y = b.CreateFMul(y, b.getFPValue(m_Module->GetFrequency()));
            z = b.CreateFMul(z, b.getFPValue(m_Module->GetFrequency()));

            llvm::Value *ix = b.CreateFPToSI(x, b.getIType());
            llvm::Value *iy = b.CreateFPToSI(y, b.getIType());
            llvm::Value *iz = b.CreateFPToSI(z, b.getIType());

            llvm::Value *xInt = b.CreateSelect(
                        b.CreateVecFCmpGT(x, b.getFPValue(0.0)),
                        ix,
                        b.CreateSub(ix, b.getIValue(1))
                        );

            llvm::Value *yInt = b.CreateSelect(
                        b.CreateVecFCmpGT(y, b.getFPValue(0.0)),
                        ix,
                        b.CreateSub(iy, b.getIValue(1))
                        );

            llvm::Value *zInt = b.CreateSelect(
                        b.CreateVecFCmpGT(z, b.getFPValue(0.0)),
                        iz,
                        b.CreateSub(iz, b.getIValue(1))
                        );

            llvm::Value *minDist = b.getFPValue(2147483647.0);
            llvm::Value *xCandidate = b.getFPValue(0.0);
            llvm::Value *yCandidate = b.getFPValue(0.0);
            llvm::Value *zCandidate = b.getFPValue(0.0);

            for (int zit = -2; zit <= 2; ++zit) {
                for (int yit = -2; yit <= 2; ++yit) {
                    for (int xit = -2; xit <= 2; ++xit) {
                        llvm::Value *xCur = b.CreateAdd(xInt, b.getIValue(xit));
                        llvm::Value *yCur = b.CreateAdd(yInt, b.getIValue(yit));
                        llvm::Value *zCur = b.CreateAdd(zInt, b.getIValue(zit));

                        llvm::Value *xCurF = b.CreateSIToFP(xCur, b.getIType());
                        llvm::Value *yCurF = b.CreateSIToFP(yCur, b.getIType());
                        llvm::Value *zCurF = b.CreateSIToFP(zCur, b.getIType());

                        llvm::Value *xPos = b.CreateFAdd(xCurF, b.CreateValueNoise3D(xCur, yCur, zCur, b.getIValue(m_Module->GetSeed())));
                        llvm::Value *yPos = b.CreateFAdd(yCurF, b.CreateValueNoise3D(xCur, yCur, zCur, b.getIValue(m_Module->GetSeed() + 1)));
                        llvm::Value *zPos = b.CreateFAdd(zCurF, b.CreateValueNoise3D(xCur, yCur, zCur, b.getIValue(m_Module->GetSeed() + 2)));

                        llvm::Value *xDist = b.CreateFSub(xPos, x);
                        llvm::Value *yDist = b.CreateFSub(yPos, y);
                        llvm::Value *zDist = b.CreateFSub(zPos, z);
                        llvm::Value *dist = b.CreateFAdd(b.CreateFMul(xDist, xDist), b.CreateFAdd(b.CreateFMul(yDist, yDist), b.CreateFMul(zDist, zDist)));

                        llvm::Value *cond = b.CreateVecFCmpLT(dist, minDist);
                        xCandidate = b.CreateSelect(cond, xPos, xCandidate);
                        yCandidate = b.CreateSelect(cond, yPos, yCandidate);
                        zCandidate = b.CreateSelect(cond, zPos, zCandidate);
                        minDist = b.CreateSelect(cond, dist, minDist);
                    }
                }
            }

            llvm::Value *value = b.getFPValue(0);
            if (m_Module->IsDistanceEnabled()) {
                llvm::Value *xDist = b.CreateFSub(xCandidate, x);
                llvm::Value *yDist = b.CreateFSub(yCandidate, y);
                llvm::Value *zDist = b.CreateFSub(zCandidate, z);
                llvm::Value *dist = b.CreateFSqrt(b.CreateFAdd(b.CreateFMul(xDist, xDist), b.CreateFAdd(b.CreateFMul(yDist, yDist), b.CreateFMul(zDist, zDist))));

                value = b.CreateFSub(b.CreateFMul(dist, b.getFPValue(SQRT_3)), b.getFPValue(1.0));
            }

            llvm::Value *xci = b.CreateFPToSI(b.CreateFloor(xCandidate), b.getIType());
            llvm::Value *yci = b.CreateFPToSI(b.CreateFloor(yCandidate), b.getIType());
            llvm::Value *zci = b.CreateFPToSI(b.CreateFloor(zCandidate), b.getIType());
            llvm::Value *noise = b.CreateValueNoise3D(xci, yci, zci, b.getIValue(0));

            b.CreateRet(b.CreateFAdd(value,
                                     b.CreateFMul(noise, b.getFPValue(m_Module->GetDisplacement()))));


            b.restoreIP(ip);
        }

        return b.CreateCallModuleFunction(m_Function, x, y, z, codeGenContext);
    }
private:
    Voronoi *m_Module;
    llvm::Function *m_Function;
};

Voronoi::Voronoi ():
  Module (GetSourceModuleCount (), new VoronoiCodeGenerator(this)),
  m_displacement   (DEFAULT_VORONOI_DISPLACEMENT),
  m_enableDistance (false                       ),
  m_frequency      (DEFAULT_VORONOI_FREQUENCY   ),
  m_seed           (DEFAULT_VORONOI_SEED        )
{
}
