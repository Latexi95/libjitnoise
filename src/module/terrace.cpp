#include "jitnoise/module/terrace.h"
#include "codegenerator.h"
#include "builder.h"
using namespace noise::module;
using namespace noise;


class TerraceCodeGenerator : public CodeGenerator
{
public:
    TerraceCodeGenerator(Terrace *a) : m_Module(a), m_Function(0) {}
    llvm::Value *generate(Builder &b, llvm::Value *x, llvm::Value *y, llvm::Value *z, llvm::Value *codeGenContext) {
        assert(m_Module->GetControlPointCount() >= 2);

        if (!m_Function) {
            llvm::Value *x, *y, *z, *codeGenContext;
            m_Function = b.CreateModuleFunction(x, y, z, codeGenContext);

            Builder::InsertPoint ip = b.saveAndClearIP();

            llvm::BasicBlock *entryBB = llvm::BasicBlock::Create(b.getContext(), "entry", m_Function);

            b.SetInsertPoint(entryBB);

            if (b.getExecutionContext().vectorize) {
                llvm::Value *sourceModuleValueVec = m_Module->GetSourceModule(0).GetCodeGenerator()->generate(b, x, y, z, codeGenContext);

                llvm::Value *resultVec = llvm::UndefValue::get(b.getFPType());
                llvm::BasicBlock *startBB[VECTOR_ELEMENT_COUNT];
                llvm::BasicBlock *breakCondBB[VECTOR_ELEMENT_COUNT];
                llvm::BasicBlock *indexBB[VECTOR_ELEMENT_COUNT];
                llvm::BasicBlock *endForBB[VECTOR_ELEMENT_COUNT];
                llvm::BasicBlock *missingPointsBB[VECTOR_ELEMENT_COUNT];
                llvm::BasicBlock *missingPointsEndBB[VECTOR_ELEMENT_COUNT];

                for (unsigned i = 0; i < VECTOR_ELEMENT_COUNT; ++i) {
                    startBB[i] = llvm::BasicBlock::Create(b.getContext(), "start", m_Function);
                    breakCondBB[i] = llvm::BasicBlock::Create(b.getContext(), "breakCond", m_Function);
                    indexBB[i] = llvm::BasicBlock::Create(b.getContext(), "index", m_Function);
                    endForBB[i] = llvm::BasicBlock::Create(b.getContext(), "endFor", m_Function);
                    missingPointsBB[i] = llvm::BasicBlock::Create(b.getContext(), "missingPoints", m_Function);
                    missingPointsEndBB[i] = llvm::BasicBlock::Create(b.getContext(), "missingPointsEnd", m_Function);
                }


                llvm::Value *indexPosVar = b.CreateAlloca(b.getInt32Ty());
                b.CreateBr(startBB[0]);
                for (unsigned i = 0; i < VECTOR_ELEMENT_COUNT; ++i) {
                    b.SetInsertPoint(startBB[i]);
                    b.CreateStore(b.getInt32(0), indexPosVar);
                    llvm::Value *sourceModuleValue = b.CreateExtractElement(sourceModuleValueVec, b.getInt32(i));
                    b.CreateBr(breakCondBB[i]);

                    b.SetInsertPoint(breakCondBB[i]);
                    llvm::Value *indexPos = b.CreateLoad(indexPosVar);
                    llvm::Value *ctrlPoint = loadControlPoint(b, indexPos);
                    llvm::Value *cond = b.CreateFCmpOLT(sourceModuleValue,ctrlPoint);
                    b.CreateCondBr(cond, endForBB[i], indexBB[i]);

                    b.SetInsertPoint(indexBB[i]);
                    indexPos = b.CreateAdd(indexPos, b.getInt32(1));
                    b.CreateStore(indexPos, indexPosVar);

                    cond = b.CreateICmpULT(indexPos, b.getInt32(m_Module->GetControlPointCount()));
                    b.CreateCondBr(cond, breakCondBB[i], endForBB[i]);

                    b.SetInsertPoint(endForBB[i]);

                    indexPos = b.CreateLoad(indexPosVar);

                    llvm::Value *index0 = b.CreateI32Clamp(b.CreateSub(indexPos, b.getInt32(1)), b.getInt32(0), b.getInt32(m_Module->GetControlPointCount() - 1));
                    llvm::Value *index1 = b.CreateI32Clamp(indexPos, b.getInt32(0), b.getInt32(m_Module->GetControlPointCount() - 1));


                    cond = b.CreateICmpEQ(index0, index1);
                    b.CreateCondBr(cond, missingPointsBB[i], missingPointsEndBB[i]);

                    b.SetInsertPoint(missingPointsBB[i]);
                    b.CreateInsertElement(resultVec, loadControlPoint(b, index1), b.getInt32(i));
                    if (i == VECTOR_ELEMENT_COUNT - 1) {
                        b.CreateRet(resultVec);
                    }
                    else {
                        b.CreateBr(startBB[i + 1]);
                        continue;
                    }

                    b.SetInsertPoint(missingPointsEndBB[i]);

                    llvm::Value *value0 = loadControlPoint(b, index0);
                    llvm::Value *value1 = loadControlPoint(b, index1);
                    llvm::Value *alpha = b.CreateFDiv(b.CreateFSub(sourceModuleValue, value0), b.CreateFSub(value1, value0));

                    if (m_Module->IsTerracesInverted()) {
                        std::swap(value0, value1);
                        alpha = b.CreateFSub(b.getSingleFPValue(1.0), alpha);
                    }
                    alpha = b.CreateFMul(alpha, alpha);



                    llvm::Value *result = b.CreateLinearInterp(
                                value0,
                                value1,
                                alpha);

                    b.CreateInsertElement(resultVec, result, b.getInt32(i));
                    if (i == VECTOR_ELEMENT_COUNT - 1) {
                        b.CreateRet(resultVec);
                    }
                    else {
                        b.CreateBr(startBB[i + 1]);
                        continue;
                    }
                }
            }
            else {
                llvm::BasicBlock *breakCondBB = llvm::BasicBlock::Create(b.getContext(), "breakCond", m_Function);
                llvm::BasicBlock *indexBB = llvm::BasicBlock::Create(b.getContext(), "index", m_Function);
                llvm::BasicBlock *endForBB = llvm::BasicBlock::Create(b.getContext(), "endFor", m_Function);
                llvm::BasicBlock *missingPointsBB = llvm::BasicBlock::Create(b.getContext(), "missingPoints", m_Function);
                llvm::BasicBlock *missingPointsEndBB = llvm::BasicBlock::Create(b.getContext(), "missingPointsEnd", m_Function);

                llvm::Value *sourceModuleValue = m_Module->GetSourceModule(0).GetCodeGenerator()->generate(b, x, y, z, codeGenContext);

                llvm::Value *indexPosVar = b.CreateAlloca(b.getInt32Ty());
                b.CreateStore(b.getInt32(0), indexPosVar);

                b.CreateBr(breakCondBB);


                b.SetInsertPoint(breakCondBB);
                llvm::Value *indexPos = b.CreateLoad(indexPosVar);
                llvm::Value *ctrlPoint = loadControlPoint(b, indexPos);
                llvm::Value *cond = b.CreateFCmpOLT(sourceModuleValue, ctrlPoint);
                b.CreateCondBr(cond, endForBB, indexBB);

                b.SetInsertPoint(indexBB);
                indexPos = b.CreateAdd(indexPos, b.getInt32(1));
                b.CreateStore(indexPos, indexPosVar);

                cond = b.CreateICmpULT(indexPos, b.getInt32(m_Module->GetControlPointCount()));
                b.CreateCondBr(cond, breakCondBB, endForBB);

                b.SetInsertPoint(endForBB);

                indexPos = b.CreateLoad(indexPosVar);

                llvm::Value *index0 = b.CreateI32Clamp(b.CreateSub(indexPos, b.getInt32(1)), b.getInt32(0), b.getInt32(m_Module->GetControlPointCount() - 1));
                llvm::Value *index1 = b.CreateI32Clamp(indexPos, b.getInt32(0), b.getInt32(m_Module->GetControlPointCount() - 1));


                cond = b.CreateICmpEQ(index0, index1);
                b.CreateCondBr(cond, missingPointsBB, missingPointsEndBB);

                b.SetInsertPoint(missingPointsBB);
                b.CreateRet(loadControlPoint(b, index1));

                b.SetInsertPoint(missingPointsEndBB);

                llvm::Value *value0 = loadControlPoint(b, index0);
                llvm::Value *value1 = loadControlPoint(b, index1);
                llvm::Value *alpha = b.CreateFDiv(b.CreateFSub(sourceModuleValue, value0), b.CreateFSub(value1, value0));

                if (m_Module->IsTerracesInverted()) {
                    std::swap(value0, value1);
                    alpha = b.CreateFSub(b.getSingleFPValue(1.0), alpha);
                }
                alpha = b.CreateFMul(alpha, alpha);



                llvm::Value *result = b.CreateLinearInterp(
                            value0,
                            value1,
                            alpha);

                b.CreateRet(result);
            }

            b.restoreIP(ip);
        }
        return b.CreateCallModuleFunction(m_Function, x, y, z, codeGenContext);
    }

    void initializeContext(Builder &b) {
        llvm::ArrayType *controlPointArrayType = llvm::ArrayType::get(b.getSingleFPType(), m_Module->GetControlPointCount());
        std::vector<llvm::Constant*> ctrlPoints(m_Module->GetControlPointCount(), 0);
        for (int i = 0; i < m_Module->GetControlPointCount(); ++i) {
            ctrlPoints[i] = b.getSingleFPValue(m_Module->GetControlPointArray()[i]);
        }

        llvm::Constant *initializer = llvm::ConstantArray::get(controlPointArrayType, ctrlPoints);
        m_ControlPoints = new llvm::GlobalVariable(b.getModule(), controlPointArrayType, true, llvm::GlobalVariable::PrivateLinkage, initializer);
    }

private:
    llvm::Value *loadControlPoint(Builder &b, llvm::Value *index) {
        std::vector<llvm::Value*> gepParams(2, b.getInt32(0));
        gepParams[1] = index;
        llvm::Value *ctrlPoint = b.CreateLoad(b.CreateGEP(m_ControlPoints, gepParams));
        return ctrlPoint;
    }


    Terrace *m_Module;
    llvm::Function *m_Function;
    llvm::GlobalVariable *m_ControlPoints;
};

using namespace noise::module;

using namespace noise;

Terrace::Terrace ():
    Module (GetSourceModuleCount (), new TerraceCodeGenerator(this)),
    m_controlPointCount (0),
    m_invertTerraces (false),
    m_pControlPoints (NULL)
{
}

Terrace::~Terrace ()
{
    delete[] m_pControlPoints;
}

void Terrace::AddControlPoint (double value)
{
    // Find the insertion point for the new control point and insert the new
    // point at that position.  The control point array will remain sorted by
    // value.
    int insertionPos = FindInsertionPos (value);
    InsertAtPos (insertionPos, value);
}

void Terrace::ClearAllControlPoints ()
{
    delete[] m_pControlPoints;
    m_pControlPoints = NULL;
    m_controlPointCount = 0;
}

int Terrace::FindInsertionPos (double value)
{
    int insertionPos;
    for (insertionPos = 0; insertionPos < m_controlPointCount; insertionPos++) {
        if (value < m_pControlPoints[insertionPos]) {
            // We found the array index in which to insert the new control point.
            // Exit now.
            break;
        } else if (value == m_pControlPoints[insertionPos]) {
            // Each control point is required to contain a unique value, so throw
            // an exception.
            assert("Multiple control points have same value" && 0);
        }
    }
    return insertionPos;
}

void Terrace::InsertAtPos (int insertionPos, double value)
{
    // Make room for the new control point at the specified position within
    // the control point array.  The position is determined by the value of
    // the control point; the control points must be sorted by value within
    // that array.
    double* newControlPoints = new double[m_controlPointCount + 1];
    for (int i = 0; i < m_controlPointCount; i++) {
        if (i < insertionPos) {
            newControlPoints[i] = m_pControlPoints[i];
        } else {
            newControlPoints[i + 1] = m_pControlPoints[i];
        }
    }
    delete[] m_pControlPoints;
    m_pControlPoints = newControlPoints;
    ++m_controlPointCount;

    // Now that we've made room for the new control point within the array,
    // add the new control point.
    m_pControlPoints[insertionPos] = value;
}

void Terrace::MakeControlPoints (int controlPointCount)
{
    assert(controlPointCount >= 2);

    ClearAllControlPoints ();

    double terraceStep = 2.0 / ((double)controlPointCount - 1.0);
    double curValue = -1.0;
    for (int i = 0; i < (int)controlPointCount; i++) {
        AddControlPoint (curValue);
        curValue += terraceStep;
    }
}
