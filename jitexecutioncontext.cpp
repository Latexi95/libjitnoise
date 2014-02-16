#include "jitexecutioncontext.h"
#include "llvm.h"
#include "noiseprogram.h"
#include "builder.h"
#include "codegenerator.h"
#include <cassert>
#include <iostream>

extern "C" void debugPrintDouble(double d, int id) {
	std::cout << id << ": Debug print: " << d << std::endl;
}


namespace jitnoise {

class JITNoiseProgram : public NoiseProgram {
	public:
		JITNoiseProgram(Module *module, llvm::ExecutionEngine *executionEngine, llvm::Module *llvmModule, llvm::Function *generationFunction) :
			NoiseProgram(module),
			mLLVMModule(llvmModule),
			mExecutionEngine(executionEngine),
			mGenerationFunction(generationFunction)
		{
		}
		~JITNoiseProgram() {
			delete mLLVMModule;
			delete mExecutionEngine;
		}

		void run(double *inputVector, double *outputValues, unsigned int count) {
			std::vector<llvm::GenericValue> params;
			params.push_back(llvm::GenericValue(inputVector));
			params.push_back(llvm::GenericValue(outputValues));
			llvm::GenericValue countP;
			countP.IntVal = llvm::APInt(32, count);
			params.push_back(countP);
			mExecutionEngine->runFunction(mGenerationFunction, params);
		}

		void run(float *inputVector, float *outputValues, unsigned int count) {
			std::vector<llvm::GenericValue> params;
			params.push_back(llvm::GenericValue(inputVector));
			params.push_back(llvm::GenericValue(outputValues));
			llvm::GenericValue countP;
			countP.IntVal = llvm::APInt(32, count);
			params.push_back(countP);
			mExecutionEngine->runFunction(mGenerationFunction, params);
		}

	private:
		llvm::Module *mLLVMModule;
		llvm::ExecutionEngine *mExecutionEngine;
		llvm::Function *mGenerationFunction;
};

JITExecutionContext::JITExecutionContext() {
}

JITExecutionContext::~JITExecutionContext() {

}

bool JITExecutionContext::initialize(Context context) {
	mContext = context;
	return !llvm::InitializeNativeTarget();
}

NoiseProgram *JITExecutionContext::compile(Module *baseModule) {
	llvm::Module *module = new llvm::Module("ebin jit", llvm::getGlobalContext());
	//module->setDataLayout("32:32:32-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v16:16:16-v32:32:32-v64:64:64-v128:128:128-n16:32:64");

	Builder builder(mContext);
	builder.initialize(module);
	std::vector<llvm::Type*> params;
	params.push_back(llvm::Type::getInt8Ty(module->getContext())->getPointerTo());
	params.push_back(llvm::Type::getInt8Ty(module->getContext())->getPointerTo());
	params.push_back(builder.getSingleIntType());
	llvm::FunctionType *funcTy = llvm::FunctionType::get(llvm::Type::getVoidTy(module->getContext()), params, false);
	llvm::Function *mainFunc = llvm::Function::Create(funcTy, llvm::Function::PrivateLinkage, "generate", module);

	params.clear();
	params.push_back(builder.getFPType());
	params.push_back(builder.getFPType());
	params.push_back(builder.getFPType());
	funcTy = llvm::FunctionType::get(builder.getFPType(), params, false);
	llvm::Function *moduleFunc = llvm::Function::Create(funcTy, llvm::Function::PrivateLinkage, "baseModule", module);
	llvm::Function::arg_iterator argI = moduleFunc->arg_begin();
	llvm::Value *x = argI; argI++;
	llvm::Value *y = argI; argI++;
	llvm::Value *z = argI;
	llvm::BasicBlock *moduleBB = llvm::BasicBlock::Create(module->getContext(), "moduleBB", moduleFunc);
	builder.begin(moduleFunc, moduleBB);

	llvm::Value *generationData = constructGenerationType(&builder, baseModule);
	if (generationData) {
		builder.setGenerationType(llvm::cast<llvm::StructType>(generationData->getType()->getPointerElementType()));
	}

	CodeGenerator *codeGen = baseModule->codeGenerator();
	llvm::Value *result = codeGen->generate(&builder, baseModule, x, y, z, generationData);
	builder.getIRBuilder()->CreateRet(result);

	argI = mainFunc->arg_begin();
	llvm::Value *inputParam = argI; argI++;
	llvm::Value *outputParam = argI; argI++;
	llvm::Value *countParam = argI;


	llvm::BasicBlock *bb1 = llvm::BasicBlock::Create(module->getContext(), "bb1", mainFunc);
	llvm::BasicBlock *condBB = llvm::BasicBlock::Create(module->getContext(), "condBB", mainFunc);
	llvm::BasicBlock *loopBB = llvm::BasicBlock::Create(module->getContext(), "loopBB", mainFunc);
	llvm::BasicBlock *endBB = llvm::BasicBlock::Create(module->getContext(), "endBB", mainFunc);
	builder.begin(mainFunc, bb1);


	llvm::Value *i = builder.getIRBuilder()->CreateAlloca(builder.getSingleIntType());
	llvm::Value *inputPtr = builder.getIRBuilder()->CreateAlloca(builder.getSingleFPType()->getPointerTo());
	llvm::Value *outputPtr = builder.getIRBuilder()->CreateAlloca(builder.getFPType()->getPointerTo());
	llvm::Value *countVar = builder.getIRBuilder()->CreateAlloca(builder.getSingleIntType());
	builder.getIRBuilder()->CreateStore(builder.getIRBuilder()->getInt32(0), i);
	builder.getIRBuilder()->CreateStore(builder.getIRBuilder()->CreateBitCast(inputParam, builder.getSingleFPType()->getPointerTo()), inputPtr);
	builder.getIRBuilder()->CreateStore(builder.getIRBuilder()->CreateBitCast(outputParam, builder.getFPType()->getPointerTo()), outputPtr);
	builder.getIRBuilder()->CreateStore(countParam, countVar);
	builder.getIRBuilder()->CreateBr(condBB);

	builder.getIRBuilder()->SetInsertPoint(condBB);
	llvm::Value *iVal = builder.getIRBuilder()->CreateLoad(i);
	llvm::Value *cond = builder.getIRBuilder()->CreateICmpULT(iVal, builder.getIRBuilder()->CreateLoad(countVar));
	builder.getIRBuilder()->CreateCondBr(cond, loopBB, endBB);

	builder.getIRBuilder()->SetInsertPoint(loopBB);

	if (mContext.mVectorize) {
		llvm::Value *inputVecPos = builder.getIRBuilder()->CreateMul(iVal, builder.getIRBuilder()->getInt32(3 * 4));
		x = llvm::UndefValue::get(builder.getFPType());
		y = llvm::UndefValue::get(builder.getFPType());
		z = llvm::UndefValue::get(builder.getFPType());
		llvm::Value *input = builder.getIRBuilder()->CreateLoad(inputPtr);
		for (int iv = 0; iv != 4; iv++) {
			llvm::Value *xLoc = builder.getIRBuilder()->CreateAdd(inputVecPos, builder.getIRBuilder()->getInt32(iv * 3));
			x = builder.getIRBuilder()->CreateInsertElement(x, builder.getIRBuilder()->CreateLoad(builder.getIRBuilder()->CreateGEP(input, xLoc), false), builder.getIRBuilder()->getInt32(iv));
			llvm::Value *yLoc =  builder.getIRBuilder()->CreateAdd(inputVecPos, builder.getIRBuilder()->getInt32(1 + iv * 3));
			y = builder.getIRBuilder()->CreateInsertElement(y, builder.getIRBuilder()->CreateLoad(builder.getIRBuilder()->CreateGEP(input, yLoc), false), builder.getIRBuilder()->getInt32(iv));
			llvm::Value *zLoc = builder.getIRBuilder()->CreateAdd(inputVecPos, builder.getIRBuilder()->getInt32(2 + iv * 3));
			z = builder.getIRBuilder()->CreateInsertElement(z, builder.getIRBuilder()->CreateLoad(builder.getIRBuilder()->CreateGEP(input, zLoc), false), builder.getIRBuilder()->getInt32(iv));
		}
	}
	else {
		llvm::Value *inputVecPos = builder.getIRBuilder()->CreateMul(iVal, builder.getIRBuilder()->getInt32(3));
		llvm::Value *input = builder.getIRBuilder()->CreateLoad(inputPtr);
		x = builder.getIRBuilder()->CreateLoad(builder.getIRBuilder()->CreateGEP(input, inputVecPos), false);
		llvm::Value *yLoc =  builder.getIRBuilder()->CreateAdd(inputVecPos, builder.getIRBuilder()->getInt32(1));
		y = builder.getIRBuilder()->CreateLoad(builder.getIRBuilder()->CreateGEP(input, yLoc), false);
		llvm::Value *zLoc = builder.getIRBuilder()->CreateAdd(inputVecPos, builder.getIRBuilder()->getInt32(2));
		z = builder.getIRBuilder()->CreateLoad(builder.getIRBuilder()->CreateGEP(input, zLoc), false);
	}

	llvm::Value *output = builder.getIRBuilder()->CreateLoad(outputPtr);
	std::vector<llvm::Value*> args;
	args.push_back(x);
	args.push_back(y);
	args.push_back(z);
	result = builder.getIRBuilder()->CreateCall(moduleFunc, args);
	builder.getIRBuilder()->CreateStore(result, builder.getIRBuilder()->CreateGEP(output, iVal));

	builder.getIRBuilder()->CreateStore(builder.getIRBuilder()->CreateAdd(iVal, builder.getIRBuilder()->getInt32(1)), i);
	builder.getIRBuilder()->CreateBr(condBB);

	builder.getIRBuilder()->SetInsertPoint(endBB);
	builder.getIRBuilder()->CreateRetVoid();

	builder.runFunctionPassManager(moduleFunc);
	builder.runFunctionPassManager(mainFunc);

	std::string errorString;
	llvm::ExecutionEngine *executionEngine = llvm::EngineBuilder(module).setErrorStr(&errorString).setOptLevel(llvm::CodeGenOpt::Aggressive).setEngineKind(llvm::EngineKind::JIT).create();
	if (!executionEngine) {
		std::cerr << errorString;
		return 0;
	}

	std::string errorInfo;
	std::string fileOpenErrorInfo;
	llvm::AssemblyAnnotationWriter asmAnnoWriter;
	llvm::raw_fd_ostream out("verifier.log", fileOpenErrorInfo);
	if (llvm::verifyModule(*module, llvm::ReturnStatusAction, &errorInfo)) { //Invalid module
		out << errorInfo;
		out << "\n\n\n-----LLVM-IR-----\n\n\n";
		module->print(out, &asmAnnoWriter);
		out.close();
		return 0;
	}
	else {
		out << "No errors";
	}
	out << "\n\n\n-----LLVM-IR-----\n\n\n";
	module->print(out, &asmAnnoWriter);
	out.close();

	//executionEngine->addGlobalMapping(builder.debugPrintDoubleFunction(), &debugPrintDouble);

	executionEngine->runJITOnFunction(mainFunc);

	return new JITNoiseProgram(baseModule, executionEngine, module, mainFunc);
	/*
	std::vector<llvm::GenericValue> genericValues;
	llvm::GenericValue genVal;
	genVal.DoubleVal = 2.0;
	genericValues.push_back(genVal);
	genVal.DoubleVal = 1.4;
	genericValues.push_back(genVal);
	genVal.DoubleVal = -0.3;
	genericValues.push_back(genVal);

	llvm::GenericValue resultR = executionEngine->runFunction(mainFunc, genericValues);
	std::cout << resultR.DoubleVal << std::endl; */

}

}
