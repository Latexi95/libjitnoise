#ifndef BUILDER_H
#define BUILDER_H
#include "llvm.h"
#include "jitnoise/context.h"
#include <map>

namespace noise
{
class CodeGenContext;
const unsigned VECTOR_ELEMENT_COUNT = 4;
class Builder : public llvm::IRBuilder<>
{
public:
    Builder(llvm::Module &module, const noise::Context &execContext);
    ~Builder();

    noise::Context getExecutionContext() const;
    void SetExecutionContext(const noise::Context &execContext);

    llvm::Value *CreateFAbs(llvm::Value *val);
    llvm::Value *CreateFSqrt(llvm::Value *val);
    llvm::Value *CreateFPow(llvm::Value *val, llvm::Value *power);
    llvm::Value *CreateFloor(llvm::Value *val);

    llvm::VectorType *getVectorType(llvm::Value *val);
    llvm::Type *getFPType();
    llvm::Type *getSingleFPType();
    llvm::Type *getIType();
    llvm::Type *getSingleIType();
    llvm::Constant *getFPValue(double v);
    llvm::Constant *getSingleFPValue(double v);
    llvm::Constant *getIValue(int32_t i);
    llvm::Constant *getSingleIValue(int32_t i);

    template <typename FUNC>
    llvm::Value *vectorUnpackApply(llvm::Value *vec, const FUNC &func);
    template <typename FUNC>
    llvm::Value *vectorUnpackApply(llvm::Value *vec1, llvm::Value *vec2, const FUNC &func);

    llvm::Value *CreateGradientCoherentNoise3D(llvm::Value *x, llvm::Value *y, llvm::Value *z, llvm::Value *seed, NoiseQuality noiseQuality);
    llvm::Value *CreateGradientNoise3D(llvm::Value *fx, llvm::Value *fy, llvm::Value *fz, llvm::Value *ix, llvm::Value *iy, llvm::Value *iz, llvm::Value *seed);
    llvm::Value *CreateLinearInterp(llvm::Value *n0, llvm::Value *n1, llvm::Value *a);
    llvm::Value *CreateCubicInterp(llvm::Value *n0, llvm::Value *n1, llvm::Value *n2, llvm::Value *n3, llvm::Value *a);
    llvm::Value *CreateSCurve3(llvm::Value *a);
    llvm::Value *CreateSCurve5(llvm::Value *a);

    llvm::Value *CreateMakeInt32Range(llvm::Value *a);

    llvm::Function *CreateModuleFunction(llvm::Value *&x, llvm::Value *&y, llvm::Value *&z, llvm::Value *&codeGenContext);
    llvm::Value *CreateCallModuleFunction(llvm::Function *func, llvm::Value *x, llvm::Value *y, llvm::Value *z, llvm::Value *codeGenContext);

    llvm::Value *CreateFCmpEQUnite(llvm::Value *a, llvm::Value *b);

    llvm::Value *CreateI32Clamp(llvm::Value *v, llvm::Value *lowerBound, llvm::Value *upperBound);

    llvm::Value *CreateFClamp(llvm::Value *value, double lowerBoundVal, double upperBoundVal);

    llvm::Value *CreateFMin(llvm::Value *a, llvm::Value *b);
    llvm::Value *CreateFMax(llvm::Value *a, llvm::Value *b);

    llvm::Value *CreateVecFCmpLT(llvm::Value *a, llvm::Value *b);
    llvm::Value *CreateVecFCmpGT(llvm::Value *a, llvm::Value *b);

    llvm::Value *CreateIntValueNoise3D(llvm::Value *x, llvm::Value *y, llvm::Value *z, llvm::Value *seed);
    llvm::Value *CreateValueNoise3D(llvm::Value *x, llvm::Value *y, llvm::Value *z, llvm::Value *seed);


    void CreatePrintFP(llvm::Value *val);

    CodeGenContext &getCodeGenContext() const;

    llvm::Module &getModule() const;

protected:
    void createRandomVectors();

    llvm::Module & m_Module;
    noise::Context m_ExecutionContext;
    llvm::Function *m_FAbsFunction;
    llvm::Function *m_FSqrtFunction;
    llvm::Function *m_FPowFunction;
    llvm::Function *m_FAbsVecFunction;
    llvm::Function *m_FSqrtVecFunction;
    llvm::Function *m_FPowVecFunction;
    llvm::Function *m_FloorFunction;
    llvm::Function *m_FloorVecFunction;
    llvm::Function *m_PrintFPFunction;
    llvm::Function *m_MakeInt32RangeFunction;
    std::map<NoiseQuality, llvm::Function*> m_GradientCoherentNoiseFunction;
    llvm::Function *m_GradientNoiseFunction;
    llvm::GlobalVariable *m_RandomVectors;

    CodeGenContext *m_CodeGenContext;

};

template <typename FUNC>
llvm::Value *Builder::vectorUnpackApply(llvm::Value *vec, const FUNC &func) {
    if (llvm::VectorType *type = getVectorType(vec)) {
        llvm::Value *result = llvm::UndefValue::get(type);
        for (unsigned i = 0; i < type->getVectorNumElements(); ++i) {
            llvm::Value *single = CreateExtractElement(vec, getInt32(i));
            single = func(single);
            result = CreateInsertElement(result, single, getInt32(i));
        }
        return result;
    }
    else {
        return func(vec);
    }
}

template <typename FUNC>
llvm::Value *Builder::vectorUnpackApply(llvm::Value *vec1, llvm::Value *vec2, const FUNC &func) {
    if (llvm::VectorType *type = getVectorType(vec1)) {
        llvm::Value *result = llvm::UndefValue::get(type);
        if (llvm::VectorType *type2 = getVectorType(vec2)) {
            assert(type->getVectorNumElements() == type2->getVectorNumElements());
            for (unsigned i = 0; i < type->getVectorNumElements(); ++i) {
                llvm::Value *single1 = CreateExtractElement(vec1, getInt32(i));
                llvm::Value *single2 = CreateExtractElement(vec2, getInt32(i));
                single1 = func(single1, single2);
                result = CreateInsertElement(result, single1, getInt32(i));
            }
        }
        else {
            for (unsigned i = 0; i < type->getVectorNumElements(); ++i) {
                llvm::Value *single = CreateExtractElement(vec1, getInt32(i));
                single = func(single, vec2);
                result = CreateInsertElement(result, single, getInt32(i));
            }
        }

        return result;
    }
    else {
        if (llvm::VectorType *type = getVectorType(vec2)) {
            llvm::Value *result = llvm::UndefValue::get(type);
            for (unsigned i = 0; i < type->getVectorNumElements(); ++i) {
                llvm::Value *single = CreateExtractElement(vec2, getInt32(i));
                single = func(vec1, single);
                result = CreateInsertElement(result, single, getInt32(i));
            }
            return result;
        }
        else {
           return func(vec1, vec2);
        }

    }
}

}
#endif // BUILDER_H
