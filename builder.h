#ifndef BUILDER_H
#define BUILDER_H
#include "llvm.h"
#include "context.h"
#include <memory>
namespace jitnoise {

class Builder {
	public:
		Builder(const Context &context);
		void initialize(llvm::Module *module);
		void begin(llvm::Function *func, llvm::BasicBlock *basicBlock);
		llvm::Constant *getFPValue(double value) const;
		llvm::Constant *getIntValue(int val) const;
		llvm::Type *getFPType() const;
		llvm::Type *getIntType() const;
		llvm::Type *getSingleFPType() const;
		llvm::Type *getSingleIntType() const;
		llvm::Value *gradientNoise3D(llvm::IRBuilder<> *irBuilder, llvm::Value *fx, llvm::Value *fy, llvm::Value *fz, llvm::Value *ix, llvm::Value *iy, llvm::Value *iz, llvm::Value *seed);

		llvm::Function *function() const { return mFunction; }
		llvm::Module *module() const { return mBuilderContext->mModule; }

		llvm::Value *gradientCoherentNoise3D(llvm::IRBuilder<> *irBuilder, llvm::Value *x, llvm::Value *y, llvm::Value *z, llvm::Value *seed);
		llvm::Value *valueNoise3D(llvm::Value *x, llvm::Value *y, llvm::Value *z, llvm::Value *seed);

		llvm::Value *interpolateCubic(llvm::IRBuilder<> *irBuilder, llvm::Value *n0, llvm::Value *n1, llvm::Value *n2, llvm::Value *n3, llvm::Value *a);
		llvm::Value *interpolateLinear(llvm::IRBuilder<> *irBuilder, llvm::Value *n0, llvm::Value *n1, llvm::Value *a);
		llvm::Value *sCurve3(llvm::IRBuilder<> *irBuilder, llvm::Value *a);
		llvm::Value *sCurve5(llvm::IRBuilder<> *irBuilder, llvm::Value *a);

		void debugPrintDouble(llvm::IRBuilder<> *irBuilder, int id, llvm::Value *d);

		llvm::Value *makeInt32Range(llvm::IRBuilder<> *irBuilder, llvm::Value *val) const;

		llvm::Value *vectorizeSingleValue(llvm::IRBuilder<> *irBuilder, llvm::Value *value);

		llvm::Constant *getSingleIntValue(int value) const;
		llvm::Constant *getSingleFPValue(double value) const;

		llvm::Value *fmul(llvm::Value *a, llvm::Value *b);
		llvm::Value *fadd(llvm::Value *a, llvm::Value *b);
		llvm::Value *fsub(llvm::Value *a, llvm::Value *b);
		llvm::Value *fdiv(llvm::Value *a, llvm::Value *b);
		llvm::Value *fpow(llvm::Value *v, llvm::Value *p);
		llvm::Value *fsqrt(llvm::Value *v);

		llvm::Value *fabs(llvm::Value *a);

		llvm::Value *extractElement(llvm::Value *vec, int i);
		llvm::Value *insertElement(llvm::Value *vec, llvm::Value *newVal, int i);

		llvm::IRBuilder<> *getIRBuilder() const { return mIRBuilder; }
		const Context &context() const { return mBuilderContext->mContext; }

		llvm::BasicBlock *createBasicBlock(const llvm::Twine &bbName, llvm::BasicBlock *insertBefore = 0);
		llvm::Function *debugPrintDoubleFunction() const { return mBuilderContext->mDebugPrintDouble; }

		void runFunctionPassManager(llvm::Function *func);

		void setGenerationType(llvm::StructType *generationDataType);


		llvm::Value *createModuleFunctionCall(llvm::Function *func, llvm::Value *x, llvm::Value *y, llvm::Value *z, llvm::Value *generationData);
		Builder *createModuleFunction(const llvm::Twine &name, llvm::Value **xParam, llvm::Value **yParam, llvm::Value **zParam, llvm::Value **generationData, llvm::BasicBlock **basicBlock);
		llvm::Value *load(llvm::Value *var);
		void store(llvm::Value *value, llvm::Value *var);

		llvm::Value *clampInt(llvm::Value *val, llvm::Value *low, llvm::Value *high);
		llvm::Value *clampFP(llvm::Value *val, llvm::Value *low, llvm::Value *high);

		llvm::Value *minFP(llvm::Value *a, llvm::Value *b);
		llvm::Value *maxFP(llvm::Value *a, llvm::Value *b);

		llvm::Value *i1x4toi1(llvm::Value *v);

		void setInsertPoint(llvm::BasicBlock *bb);
		void branch(llvm::BasicBlock *bb);
		void conditionalBranch(llvm::Value *cond, llvm::BasicBlock *ifTrue, llvm::BasicBlock *ifFalse);

	private:
		struct BuilderContext {
			BuilderContext(const Context &context);
			const Context mContext;

			llvm::GlobalVariable *mRandomVectors;
			llvm::Function *mGradientNoise3D;
			llvm::Function *mGradientCoherentNoise3D;
			llvm::Function *mDebugPrintDouble;
			llvm::Function *mMakeInt32Range;
			llvm::Function *mValueNoise3D;
			llvm::StructType *mGenerationDataType;
			llvm::Function *mFPPow;
			llvm::Function *mFPSqrt;
			llvm::Function *mFPVectorSqrt;

			llvm::Module *mModule;

			llvm::FunctionPassManager *mFPM;
		};

		Builder(std::shared_ptr<BuilderContext> context);

		void createRandomVectors(llvm::Module *module);
		void generateGradientNoise3DFunction(llvm::Module *module);
		void generateGradientCoherentNoise3DFunction(llvm::Module *module);
		void generateDebugPrinterFunction(llvm::Module *module);
		void generateIntMake32RangeFunction(llvm::Module *module);
		void generateValueNoise3DFunction(llvm::Module *module);
		void defineStandardFunctions(llvm::Module *module);

		std::shared_ptr<BuilderContext> mBuilderContext;
		llvm::IRBuilder<> *mIRBuilder;
		llvm::Function *mFunction;
};

}

#endif // BUILDER_H
