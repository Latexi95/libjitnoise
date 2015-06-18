#ifndef LLVM_H_
#define LLVM_H_
#include <llvm/Config/llvm-config.h>


#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/Module.h>
#include <llvm/PassManager.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/IR/Function.h>
#include <llvm/Analysis/Passes.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/MCJIT.h>
//#include <llvm/ExecutionEngine/JIT.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/Support/raw_os_ostream.h>
#include <llvm/IR/Verifier.h>
#include <llvm/IR/AssemblyAnnotationWriter.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/ExecutionEngine/RuntimeDyld.h>
#include <llvm/Support/DynamicLibrary.h>
#include <llvm/ExecutionEngine/SectionMemoryManager.h>
#include <llvm/Transforms/IPO/PassManagerBuilder.h>


#endif // LLVM_H_
