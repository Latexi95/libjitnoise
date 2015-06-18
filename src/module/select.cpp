#include "jitnoise/module/select.h"
#include "codegenerator.h"
#include "builder.h"
using namespace noise::module;
using namespace noise;


class SelectCodeGenerator : public CodeGenerator
{
public:
    SelectCodeGenerator(Select *a) : m_Module(a), m_Function(0) {}
    llvm::Value *generate(Builder &b, llvm::Value *x, llvm::Value *y, llvm::Value *z, llvm::Value *codeGenContext) {
        if (!m_Function) {
            llvm::Value *x, *y, *z, *codeGenContext;
            m_Function = b.CreateModuleFunction(x, y, z, codeGenContext);

            Builder::InsertPoint ip = b.saveAndClearIP();

            llvm::BasicBlock *entryBB = llvm::BasicBlock::Create(b.getContext(), "entry", m_Function);
            b.SetInsertPoint(entryBB);

            llvm::Value *controlValue = m_Module->GetSourceModule(2).GetCodeGenerator()->generate(b, x, y, z, codeGenContext);

            if (b.getExecutionContext().vectorize) {
                llvm::Value *source1 = m_Module->GetSourceModule(0).GetCodeGenerator()->generate(b, x, y, z, codeGenContext);
                llvm::Value *source2 = m_Module->GetSourceModule(1).GetCodeGenerator()->generate(b, x, y, z, codeGenContext);

                if (m_Module->GetEdgeFalloff() > 0.0) {
                    llvm::Value *cond;
                    llvm::Value *selectTrue;
                    double lowerCurve, upperCurve;

                    cond = b.CreateVecFCmpLT(controlValue, b.getFPValue(m_Module->GetUpperBound() + m_Module->GetEdgeFalloff()));
                    lowerCurve = m_Module->GetUpperBound() - m_Module->GetEdgeFalloff();
                    upperCurve = m_Module->GetUpperBound() + m_Module->GetEdgeFalloff();
                    llvm::Value *alpha = b.CreateFDiv(b.CreateFSub(controlValue, b.getFPValue(lowerCurve)), b.getFPValue(upperCurve - lowerCurve));
                    alpha = b.CreateSCurve3(alpha);
                    selectTrue = b.CreateLinearInterp(
                                source1,
                                source2,
                                alpha
                                );
                    llvm::Value *select4 = b.CreateSelect(cond, selectTrue, source1);

                    cond = b.CreateVecFCmpLT(controlValue, b.getFPValue(m_Module->GetUpperBound() - m_Module->GetEdgeFalloff()));
                    selectTrue = source2;
                    llvm::Value *select3 = b.CreateSelect(cond, selectTrue, select4);

                    cond = b.CreateVecFCmpLT(controlValue, b.getFPValue(m_Module->GetLowerBound() + m_Module->GetEdgeFalloff()));
                    lowerCurve = m_Module->GetLowerBound() - m_Module->GetEdgeFalloff();
                    upperCurve = m_Module->GetLowerBound() + m_Module->GetEdgeFalloff();
                    alpha = b.CreateFDiv(b.CreateFSub(controlValue, b.getFPValue(lowerCurve)), b.getFPValue(upperCurve - lowerCurve));
                    alpha = b.CreateSCurve3(alpha);
                    selectTrue = b.CreateLinearInterp(
                                source1,
                                source2,
                                alpha
                                );
                    llvm::Value *select2 = b.CreateSelect(cond, selectTrue, select3);

                    cond = b.CreateVecFCmpLT(controlValue, b.getFPValue(m_Module->GetLowerBound() - m_Module->GetEdgeFalloff()));
                    selectTrue = source1;
                    llvm::Value *select1 = b.CreateSelect(cond, selectTrue, select2);

                    b.CreateRet(select1);
                }
                else {
                    llvm::Value *outBoundsCond = b.CreateOr(
                                b.CreateVecFCmpLT(controlValue, b.getFPValue(m_Module->GetLowerBound())),
                                b.CreateVecFCmpLT(b.getFPValue(m_Module->GetUpperBound()), controlValue)
                                );

                    llvm::Value *result = b.CreateSelect(outBoundsCond, source1, source2);
                    b.CreateRet(result);
                }

            }
            else {
                if (m_Module->GetEdgeFalloff() > 0.0) {
                    llvm::BasicBlock *outLowerBoundBB = llvm::BasicBlock::Create(b.getContext(), "outBound", m_Function);
                    llvm::BasicBlock *lowerBoundEdgeCondBB = llvm::BasicBlock::Create(b.getContext(), "lowerBoundEdgeCond", m_Function);
                    llvm::BasicBlock *lowerBoundEdgeBB = llvm::BasicBlock::Create(b.getContext(), "lowerBoundEdge", m_Function);
                    llvm::BasicBlock *inBoundsCondBB = llvm::BasicBlock::Create(b.getContext(), "inBoundsCond", m_Function);
                    llvm::BasicBlock *inBoundsBB = llvm::BasicBlock::Create(b.getContext(), "inBounds", m_Function);
                    llvm::BasicBlock *upperBoundEdgeCondBB = llvm::BasicBlock::Create(b.getContext(), "upperBoundEdgeCond", m_Function);
                    llvm::BasicBlock *upperBoundEdgeBB = llvm::BasicBlock::Create(b.getContext(), "upperBoundEdge", m_Function);
                    llvm::BasicBlock *outUpperBoundBB = outLowerBoundBB;

                    llvm::Value *cond = b.CreateFCmpOLT(controlValue, b.getFPValue(m_Module->GetLowerBound() - m_Module->GetEdgeFalloff()));
                    b.CreateCondBr(cond, outLowerBoundBB, lowerBoundEdgeCondBB);

                    b.SetInsertPoint(outLowerBoundBB);
                    b.CreateRet(m_Module->GetSourceModule(0).GetCodeGenerator()->generate(b, x, y, z, codeGenContext));

                    b.SetInsertPoint(lowerBoundEdgeCondBB);
                    cond = b.CreateFCmpOLT(controlValue, b.getFPValue(m_Module->GetLowerBound() + m_Module->GetEdgeFalloff()));
                    b.CreateCondBr(cond, lowerBoundEdgeBB, inBoundsCondBB);

                    b.SetInsertPoint(lowerBoundEdgeBB);
                    double lowerCurve = m_Module->GetLowerBound() - m_Module->GetEdgeFalloff();
                    double upperCurve = m_Module->GetLowerBound() + m_Module->GetEdgeFalloff();
                    llvm::Value *alpha = b.CreateFDiv(b.CreateFSub(controlValue, b.getFPValue(lowerCurve)), b.getFPValue(upperCurve - lowerCurve));
                    alpha = b.CreateSCurve3(alpha);
                    b.CreateRet(b.CreateLinearInterp(
                                    m_Module->GetSourceModule(0).GetCodeGenerator()->generate(b, x, y, z, codeGenContext),
                                    m_Module->GetSourceModule(1).GetCodeGenerator()->generate(b, x, y, z, codeGenContext),
                                    alpha
                                    ));

                    b.SetInsertPoint(inBoundsCondBB);
                    cond = b.CreateFCmpOLT(controlValue, b.getFPValue(m_Module->GetUpperBound() - m_Module->GetEdgeFalloff()));
                    b.CreateCondBr(cond, inBoundsBB, upperBoundEdgeCondBB);

                    b.SetInsertPoint(inBoundsBB);
                    b.CreateRet(m_Module->GetSourceModule(1).GetCodeGenerator()->generate(b, x, y, z, codeGenContext));

                    b.SetInsertPoint(upperBoundEdgeCondBB);
                    cond = b.CreateFCmpOLT(controlValue, b.getFPValue(m_Module->GetUpperBound() + m_Module->GetEdgeFalloff()));
                    b.CreateCondBr(cond, upperBoundEdgeBB, outUpperBoundBB);

                    b.SetInsertPoint(upperBoundEdgeBB);
                    lowerCurve = m_Module->GetUpperBound() - m_Module->GetEdgeFalloff();
                    upperCurve = m_Module->GetUpperBound() + m_Module->GetEdgeFalloff();
                    alpha = b.CreateFDiv(b.CreateFSub(controlValue, b.getFPValue(lowerCurve)), b.getFPValue(upperCurve - lowerCurve));
                    alpha = b.CreateSCurve3(alpha);
                    b.CreateRet(b.CreateLinearInterp(
                                    m_Module->GetSourceModule(1).GetCodeGenerator()->generate(b, x, y, z, codeGenContext),
                                    m_Module->GetSourceModule(2).GetCodeGenerator()->generate(b, x, y, z, codeGenContext),
                                    alpha
                                    ));
                }
                else {
                    llvm::BasicBlock *outBoundsBB = llvm::BasicBlock::Create(b.getContext(), "outBounds", m_Function);
                    llvm::BasicBlock *inBoundsBB = llvm::BasicBlock::Create(b.getContext(), "inBounds", m_Function);
                    llvm::Value *outBoundsCond = b.CreateOr(
                                b.CreateFCmpOLT(controlValue, b.getFPValue(m_Module->GetLowerBound())),
                                b.CreateFCmpOGT(controlValue, b.getFPValue(m_Module->GetUpperBound()))
                                );
                    b.CreateCondBr(outBoundsCond, outBoundsBB, inBoundsBB);

                    b.SetInsertPoint(outBoundsBB);
                    b.CreateRet(m_Module->GetSourceModule(0).GetCodeGenerator()->generate(b, x, y, z, codeGenContext));

                    b.SetInsertPoint(inBoundsBB);
                    b.CreateRet(m_Module->GetSourceModule(1).GetCodeGenerator()->generate(b, x, y, z, codeGenContext));
                }
            }


            b.restoreIP(ip);
        }

        return b.CreateCallModuleFunction(m_Function, x, y, z, codeGenContext);
    }
private:
    Select *m_Module;
    llvm::Function *m_Function;
};

Select::Select ():
    Module (GetSourceModuleCount (), new SelectCodeGenerator(this)),
    m_edgeFalloff (DEFAULT_SELECT_EDGE_FALLOFF),
    m_lowerBound (DEFAULT_SELECT_LOWER_BOUND),
    m_upperBound (DEFAULT_SELECT_UPPER_BOUND)
{
}


void Select::SetBounds (double lowerBound, double upperBound)
{
    assert (lowerBound < upperBound);

    m_lowerBound = lowerBound;
    m_upperBound = upperBound;

    // Make sure that the edge falloff curves do not overlap.
    SetEdgeFalloff (m_edgeFalloff);
}

void Select::SetEdgeFalloff (double edgeFalloff)
{
    // Make sure that the edge falloff curves do not overlap.
    double boundSize = m_upperBound - m_lowerBound;
    m_edgeFalloff = (edgeFalloff > boundSize / 2)? boundSize / 2: edgeFalloff;
}
