#ifndef LLVM_H
#define LLVM_H
#include <llvm/Config/llvm-config.h>

#if (LLVM_VERSION_MAJOR != 3)
	#error libJitNoise requires LLVM 3.2 or newer
#endif

#if LLVM_VERSION_MINOR < 2
	#error libJitNoise requires LLVM 3.2 or newer
#endif

#if LLVM_VERSION_MINOR == 2

#include <llvm/GlobalVariable.h>
#include <llvm/Module.h>

#include <llvm/DefaultPasses.h>
#include <llvm/PassManager.h>
#include <llvm/PassManagers.h>
#include <llvm/Analysis/Passes.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/Function.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/LLVMContext.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/ExecutionEngine/JIT.h>
#include <llvm/Support/raw_os_ostream.h>
#include <llvm/Assembly/AssemblyAnnotationWriter.h>
#elif LLVM_VERSION_MINOR == 4
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/Module.h>
#include <llvm/PassManager.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/IR/Function.h>
#include <llvm/Analysis/Passes.h>
#include <llvm/ExecutionEngine/JIT.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/MCJIT.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/Analysis/Verifier.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/Assembly/AssemblyAnnotationWriter.h>
#include <llvm/Support/raw_os_ostream.h>
#endif


#endif // LLVM_H
