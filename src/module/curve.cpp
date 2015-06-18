#include "jitnoise/module/curve.h"
#include "codegenerator.h"
#include "builder.h"
using namespace noise::module;
using namespace noise;


class CurveCodeGenerator : public CodeGenerator
{
public:
    CurveCodeGenerator(Curve *a) : m_Module(a), m_Function(0) {}
    llvm::Value *generate(Builder &b, llvm::Value *x, llvm::Value *y, llvm::Value *z, llvm::Value *codeGenContext) {
        assert(m_Module->GetControlPointCount() >= 4);

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
                    llvm::Value *inputCtrlPoint = loadInputControlPoint(b, indexPos);
                    llvm::Value *cond = b.CreateFCmpOLT(sourceModuleValue, inputCtrlPoint);
                    b.CreateCondBr(cond, endForBB[i], indexBB[i]);

                    b.SetInsertPoint(indexBB[i]);
                    indexPos = b.CreateAdd(indexPos, b.getInt32(1));
                    b.CreateStore(indexPos, indexPosVar);

                    cond = b.CreateICmpULT(indexPos, b.getInt32(m_Module->GetControlPointCount()));
                    b.CreateCondBr(cond, breakCondBB[i], endForBB[i]);

                    b.SetInsertPoint(endForBB[i]);

                    indexPos = b.CreateLoad(indexPosVar);

                    llvm::Value *index1 = b.CreateI32Clamp(b.CreateSub(indexPos, b.getInt32(2)), b.getInt32(0), b.getInt32(m_Module->GetControlPointCount() - 1));
                    llvm::Value *index2 = b.CreateI32Clamp(indexPos, b.getInt32(0), b.getInt32(m_Module->GetControlPointCount() - 1));


                    cond = b.CreateICmpEQ(index1, index2);
                    b.CreateCondBr(cond, missingPointsBB[i], missingPointsEndBB[i]);

                    b.SetInsertPoint(missingPointsBB[i]);
                    b.CreateInsertElement(resultVec, loadOutputControlPoint(b, index1), b.getInt32(i));
                    if (i == VECTOR_ELEMENT_COUNT - 1) {
                        b.CreateRet(resultVec);
                    }
                    else {
                        b.CreateBr(startBB[i + 1]);
                        continue;
                    }

                    b.SetInsertPoint(missingPointsEndBB[i]);

                    llvm::Value *index0 = b.CreateI32Clamp(b.CreateSub(indexPos, b.getInt32(2)), b.getInt32(0), b.getInt32(m_Module->GetControlPointCount() - 1));
                    llvm::Value *index3 = b.CreateI32Clamp(b.CreateAdd(indexPos, b.getInt32(1)), b.getInt32(0), b.getInt32(m_Module->GetControlPointCount() - 1));

                    llvm::Value *input0 = loadInputControlPoint(b, index1);
                    llvm::Value *input1 = loadInputControlPoint(b, index2);
                    llvm::Value *alpha = b.CreateFDiv(b.CreateFSub(sourceModuleValue, input0), b.CreateFSub(input1, input0));

                    llvm::Value *result = b.CreateCubicInterp(
                                loadOutputControlPoint(b, index0),
                                loadOutputControlPoint(b, index1),
                                loadOutputControlPoint(b, index2),
                                loadOutputControlPoint(b, index3),
                                alpha
                                );

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
                llvm::Value *inputCtrlPoint = loadInputControlPoint(b, indexPos);
                llvm::Value *cond = b.CreateFCmpOLT(sourceModuleValue, inputCtrlPoint);
                b.CreateCondBr(cond, endForBB, indexBB);

                b.SetInsertPoint(indexBB);
                indexPos = b.CreateAdd(indexPos, b.getInt32(1));
                b.CreateStore(indexPos, indexPosVar);

                cond = b.CreateICmpULT(indexPos, b.getInt32(m_Module->GetControlPointCount()));
                b.CreateCondBr(cond, breakCondBB, endForBB);

                b.SetInsertPoint(endForBB);

                indexPos = b.CreateLoad(indexPosVar);

                llvm::Value *index1 = b.CreateI32Clamp(b.CreateSub(indexPos, b.getInt32(2)), b.getInt32(0), b.getInt32(m_Module->GetControlPointCount() - 1));
                llvm::Value *index2 = b.CreateI32Clamp(indexPos, b.getInt32(0), b.getInt32(m_Module->GetControlPointCount() - 1));


                cond = b.CreateICmpEQ(index1, index2);
                b.CreateCondBr(cond, missingPointsBB, missingPointsEndBB);

                b.SetInsertPoint(missingPointsBB);
                b.CreateRet(loadOutputControlPoint(b, index1));

                b.SetInsertPoint(missingPointsEndBB);

                llvm::Value *index0 = b.CreateI32Clamp(b.CreateSub(indexPos, b.getInt32(2)), b.getInt32(0), b.getInt32(m_Module->GetControlPointCount() - 1));
                llvm::Value *index3 = b.CreateI32Clamp(b.CreateAdd(indexPos, b.getInt32(1)), b.getInt32(0), b.getInt32(m_Module->GetControlPointCount() - 1));

                llvm::Value *input0 = loadInputControlPoint(b, index1);
                llvm::Value *input1 = loadInputControlPoint(b, index2);
                llvm::Value *alpha = b.CreateFDiv(b.CreateFSub(sourceModuleValue, input0), b.CreateFSub(input1, input0));

                llvm::Value *result = b.CreateCubicInterp(
                            loadOutputControlPoint(b, index0),
                            loadOutputControlPoint(b, index1),
                            loadOutputControlPoint(b, index2),
                            loadOutputControlPoint(b, index3),
                            alpha
                            );
                b.CreateRet(result);
            }

            b.restoreIP(ip);
        }
        return b.CreateCallModuleFunction(m_Function, x, y, z, codeGenContext);
    }

    void initializeContext(Builder &b) {
        llvm::ArrayType *controlPointArrayType = llvm::ArrayType::get(b.getSingleFPType(), m_Module->GetControlPointCount());
        std::vector<llvm::Constant*> inputCtrlPoints(m_Module->GetControlPointCount(), 0);
        std::vector<llvm::Constant*> outputCtrlPoints(m_Module->GetControlPointCount(), 0);
        for (int i = 0; i < m_Module->GetControlPointCount(); ++i) {
            inputCtrlPoints[i] = b.getSingleFPValue(m_Module->GetControlPointArray()[i].inputValue);
            outputCtrlPoints[i] = b.getSingleFPValue(m_Module->GetControlPointArray()[i].outputValue);
        }

        llvm::Constant *inputInitializer = llvm::ConstantArray::get(controlPointArrayType, inputCtrlPoints);
        llvm::Constant *outputInitializer = llvm::ConstantArray::get(controlPointArrayType, outputCtrlPoints);
        m_InputControlPoints = new llvm::GlobalVariable(b.getModule(), controlPointArrayType, true, llvm::GlobalVariable::PrivateLinkage, inputInitializer);
        m_OutputControlPoints = new llvm::GlobalVariable(b.getModule(), controlPointArrayType, true, llvm::GlobalVariable::PrivateLinkage, outputInitializer);
    }

private:
    llvm::Value *loadInputControlPoint(Builder &b, llvm::Value *index) {
        std::vector<llvm::Value*> gepParams(2, b.getInt32(0));
        gepParams[1] = index;
        llvm::Value *inputCtrlPoint = b.CreateLoad(b.CreateGEP(m_InputControlPoints, gepParams));
        return inputCtrlPoint;
    }

    llvm::Value *loadOutputControlPoint(Builder &b, llvm::Value *index) {
        std::vector<llvm::Value*> gepParams(2, b.getInt32(0));
        gepParams[1] = index;
        llvm::Value *outputCtrlPoint = b.CreateLoad(b.CreateGEP(m_OutputControlPoints, gepParams));
        return outputCtrlPoint;
    }

    Curve *m_Module;
    llvm::Function *m_Function;
    llvm::GlobalVariable *m_InputControlPoints;
    llvm::GlobalVariable *m_OutputControlPoints;
};



Curve::Curve ():
    Module (GetSourceModuleCount (), new CurveCodeGenerator(this)),
    m_pControlPoints (NULL)
{
    m_controlPointCount = 0;
}

Curve::~Curve ()
{
    delete[] m_pControlPoints;
}

void Curve::AddControlPoint (double inputValue, double outputValue)
{
    // Find the insertion point for the new control point and insert the new
    // point at that position.  The control point array will remain sorted by
    // input value.
    int insertionPos = FindInsertionPos (inputValue);
    InsertAtPos (insertionPos, inputValue, outputValue);
}

void Curve::ClearAllControlPoints ()
{
    delete[] m_pControlPoints;
    m_pControlPoints = NULL;
    m_controlPointCount = 0;
}

int Curve::FindInsertionPos (double inputValue)
{
    int insertionPos;
    for (insertionPos = 0; insertionPos < m_controlPointCount; insertionPos++) {
        if (inputValue < m_pControlPoints[insertionPos].inputValue) {
            // We found the array index in which to insert the new control point.
            // Exit now.
            break;
        } else if (inputValue == m_pControlPoints[insertionPos].inputValue) {
            // Each control point is required to contain a unique input value
            assert("Multiple control points with same input value" && 0);
        }
    }
    return insertionPos;
}


void Curve::InsertAtPos (int insertionPos, double inputValue,
                         double outputValue)
{
    // Make room for the new control point at the specified position within the
    // control point array.  The position is determined by the input value of
    // the control point; the control points must be sorted by input value
    // within that array.
    ControlPoint* newControlPoints = new ControlPoint[m_controlPointCount + 1];
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

    // Now that we've made room for the new control point within the array, add
    // the new control point.
    m_pControlPoints[insertionPos].inputValue  = inputValue ;
    m_pControlPoints[insertionPos].outputValue = outputValue;
}
