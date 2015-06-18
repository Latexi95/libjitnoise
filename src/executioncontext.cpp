#include "jitnoise/executioncontext.h"
#include "codegenerator.h"
#include "builder.h"
#include "codegencontext.h"
#include "jitnoise/module/modulebase.h"
#include <iostream>
#include <string>

using namespace noise;

unsigned fModFCalls = 0;
float fmodfCustom(float a, float b) {
    fModFCalls++;
    return fmodf(a, b);
}


struct ExecutionContext::ExecutionContextImpl {
    ExecutionContextImpl() : m_ExecutionEngine(0), m_Module(0) {}
    ~ExecutionContextImpl() { delete m_ExecutionEngine; }

    Context m_Context;
    llvm::ExecutionEngine *m_ExecutionEngine;
    llvm::Module *m_Module;
};


void initializeCodeGenContext(Builder &b, const module::Module &mod) {
    mod.GetCodeGenerator()->initializeContext(b);
    for (int i = 0; i < mod.GetSourceModuleCount(); ++i) {
        initializeCodeGenContext(b, mod.GetSourceModule(i));
    }
}


class HelpingMemoryManager : public llvm::SectionMemoryManager
{
    HelpingMemoryManager(const HelpingMemoryManager&) LLVM_DELETED_FUNCTION;
    void operator=(const HelpingMemoryManager&) LLVM_DELETED_FUNCTION;

public:
    HelpingMemoryManager() {}
    virtual ~HelpingMemoryManager() {}

    /// This method returns the address of the specified function.
    /// Our implementation will attempt to find functions in other
    /// modules associated with the MCJITHelper to cross link functions
    /// from one generated module to another.
    ///
    /// If \p AbortOnFailure is false and no function with the given name is
    /// found, this function returns a null pointer. Otherwise, it prints a
    /// message to stderr and aborts.
    virtual void *getPointerToNamedFunction(const std::string &Name,
                                            bool AbortOnFailure = true);

    virtual uint64_t getSymbolAddress(const std::string &Name);
private:
};

void *HelpingMemoryManager::getPointerToNamedFunction(const std::string &Name,
                                                      bool AbortOnFailure)
{
    std::cout << "Resolve \"" << Name << "\"\n";
    if (Name == "fmodf") {
        return (void*)&fmodfCustom;
    }

    // Try the standard symbol resolution first, but ask it not to abort.
    void *pfn = RTDyldMemoryManager::getPointerToNamedFunction(Name, false);
    if (pfn)
        return pfn;

    if (!pfn && AbortOnFailure) {
        std::cerr << ("Program used external function '" + Name +
                      "' which could not be resolved!") << std::endl;
        exit(1);
    }
    return pfn;
}

uint64_t HelpingMemoryManager::getSymbolAddress(const std::string &Name)
{
    std::cout << "Resolve \"" << Name << "\"\n";
    if (Name == "fmodf") {
        return (uint64_t)&fmodfCustom;
    }

    uint64_t pfn = RTDyldMemoryManager::getSymbolAddress(Name);
    if (pfn)
        return pfn;

    if (!pfn) {
        std::cerr << ("Program used external function '" + Name +
                      "' which could not be resolved!") << std::endl;
        exit(1);
    }
    return pfn;
}



void printFloat(float f) {
    std::cout << "Print float: " << f << std::endl;
}

void printFloatVec(float v1, float v2, float v3, float v4) {
    std::cout << "Print float vec: " << v1 << ' ' << v2 << ' ' << v3 << ' ' << v4 << std::endl;
}


void printDouble(double f) {
    std::cout << "Print double: " << f << std::endl;
}

void printDoubleVec(double v1, double v2, double v3, double v4) {
    std::cout << "Print double vec: " << v1 << ' ' << v2 << ' ' << v3 << ' ' << v4 << std::endl;
}


ExecutionContext::ExecutionContext(const Context &context) :
    impl(new ExecutionContext::ExecutionContextImpl())
{
    impl->m_Context = context;
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
    llvm::InitializeNativeTargetAsmParser();
    impl->m_Module = new llvm::Module("jit", llvm::getGlobalContext());
    impl->m_Module->setDataLayout("e-m:e-i64:64-f80:128-n8:16:32:64-S128");
    impl->m_Module->setTargetTriple("x86_64-pc-windows-msvc-elf");
    impl->m_ExecutionEngine = 0;

    //llvm::sys::DynamicLibrary::AddSymbol("fmodf", (void*)fmodfCustom);
    llvm::sys::DynamicLibrary::AddSymbol("printFloat", (void*)printFloat);
    llvm::sys::DynamicLibrary::AddSymbol("printFloatVec", (void*)printFloatVec);
    llvm::sys::DynamicLibrary::AddSymbol("printDouble", (void*)printDouble);
    llvm::sys::DynamicLibrary::AddSymbol("printDoubleVec", (void*)printDoubleVec);
}

ExecutionContext::~ExecutionContext()
{
    delete impl;
}

void ExecutionContext::SetContext(const Context &context)
{
    impl->m_Context = context;
}

Context ExecutionContext::GetContext() const
{
    return impl->m_Context;
}

llvm::Function *constructPackedSourceFunction(const Context &context, llvm::Module *module, llvm::Function *execFunction, Builder &b)
{
    std::vector<llvm::Type*> params;
    params.resize(3);
    params[0] =  b.getSingleFPType()->getPointerTo();
    params[1] =  b.getSingleFPType()->getPointerTo();
    params[2] = b.getInt32Ty();
    llvm::FunctionType *packedSourceFuncType = llvm::FunctionType::get(b.getVoidTy(), params, false);
    llvm::Function *packedSourceFunc = llvm::Function::Create(packedSourceFuncType, llvm::Function::ExternalLinkage, "packedSourceFunc", module);

    llvm::BasicBlock *packedSourceFuncEntryBB = llvm::BasicBlock::Create(module->getContext(), "packedSourceFuncEntry", packedSourceFunc);
    llvm::BasicBlock *loopBB = llvm::BasicBlock::Create(module->getContext(), "loopBody", packedSourceFunc);
    llvm::BasicBlock *loopEndBB = llvm::BasicBlock::Create(module->getContext(), "loopEnd", packedSourceFunc);
    b.SetInsertPoint(packedSourceFuncEntryBB);

    auto argIt = packedSourceFunc->arg_begin();
    llvm::Value *packedDataPointer = argIt++;
    llvm::Value *resultPointer = argIt++;
    llvm::Value *count = argIt;

    llvm::Value *iVar = b.CreateAlloca(b.getInt32Ty());
    b.CreateStore(b.getInt32(0), iVar);
    b.CreateBr(loopBB);

    b.SetInsertPoint(loopBB);
    llvm::Value *i = b.CreateLoad(iVar);
    if (context.vectorize) {
        llvm::Value *x = llvm::UndefValue::get(b.getFPType());
        llvm::Value *y = llvm::UndefValue::get(b.getFPType());
        llvm::Value *z = llvm::UndefValue::get(b.getFPType());
        for (unsigned index = 0; index < VECTOR_ELEMENT_COUNT; ++index) {
            llvm::Value *baseIndex = b.CreateMul(i, b.getInt32(3));
            llvm::Value *xIndex = b.CreateAdd(baseIndex, b.getInt32(index));
            llvm::Value *yIndex = b.CreateAdd(xIndex, b.getInt32(1 + index));
            llvm::Value *zIndex = b.CreateAdd(xIndex, b.getInt32(2 + index));
            llvm::Value *xElem = b.CreateLoad(b.CreateGEP(packedDataPointer, xIndex));

            llvm::Value *yElem = b.CreateLoad(b.CreateGEP(packedDataPointer, yIndex));

            llvm::Value *zElem = b.CreateLoad(b.CreateGEP(packedDataPointer, zIndex));

            x = b.CreateInsertElement(x, xElem, b.getInt32(index));
            y = b.CreateInsertElement(y, yElem, b.getInt32(index));
            z = b.CreateInsertElement(z, zElem, b.getInt32(index));
        }

        llvm::Value *result = b.CreateCall3(execFunction, x, y, z);
        for (unsigned index = 0; index < VECTOR_ELEMENT_COUNT; ++index) {
            llvm::Value *resultElement = b.CreateExtractElement(result, b.getInt32(index));
            b.CreateStore(resultElement, b.CreateGEP(resultPointer, b.CreateAdd(i, b.getInt32(index))));
        }

        i = b.CreateAdd(i, b.getInt32(VECTOR_ELEMENT_COUNT));
    }
    else {
        llvm::Value *baseIndex = b.CreateMul(i, b.getInt32(3));
        llvm::Value *xIndex = baseIndex;
        llvm::Value *yIndex = b.CreateAdd(xIndex, b.getInt32(1));
        llvm::Value *zIndex = b.CreateAdd(xIndex, b.getInt32(2));
        llvm::Value *x = b.CreateLoad(b.CreateGEP(packedDataPointer, xIndex));
        llvm::Value *y = b.CreateLoad(b.CreateGEP(packedDataPointer, yIndex));
        llvm::Value *z = b.CreateLoad(b.CreateGEP(packedDataPointer, zIndex));

        llvm::Value *result = b.CreateCall3(execFunction, x, y, z);

        b.CreateStore(result, b.CreateGEP(resultPointer, i));
        i = b.CreateAdd(i, b.getInt32(1));
    }
    b.CreateStore(i, iVar);
    llvm::Value *cond = b.CreateICmpULT(i, count);
    b.CreateCondBr(cond, loopBB, loopEndBB);

    b.SetInsertPoint(loopEndBB);
    b.CreateRetVoid();

    return packedSourceFunc;
}

llvm::Function *constructUnpackedSourceFunction(const Context &context, llvm::Module *module, llvm::Function *execFunction, Builder &b)
{
    std::vector<llvm::Type*> params;
    params.resize(5);
    params[0] =  b.getSingleFPType()->getPointerTo();
    params[1] =  b.getSingleFPType()->getPointerTo();
    params[2] =  b.getSingleFPType()->getPointerTo();
    params[3] =  b.getSingleFPType()->getPointerTo();
    params[4] = b.getInt32Ty();
    llvm::FunctionType *unpackedSourceFuncType = llvm::FunctionType::get(b.getVoidTy(), params, false);
    llvm::Function *unpackedSourceFunc = llvm::Function::Create(unpackedSourceFuncType, llvm::Function::ExternalLinkage, "unpackedSourceFunc", module);

    llvm::BasicBlock *unpackedSourceFuncEntryBB = llvm::BasicBlock::Create(module->getContext(), "packedSourceFuncEntry", unpackedSourceFunc);
    llvm::BasicBlock *loopBB = llvm::BasicBlock::Create(module->getContext(), "loopBody", unpackedSourceFunc);
    llvm::BasicBlock *loopEndBB = llvm::BasicBlock::Create(module->getContext(), "loopEnd", unpackedSourceFunc);
    b.SetInsertPoint(unpackedSourceFuncEntryBB);

    auto argIt = unpackedSourceFunc->arg_begin();
    llvm::Value *xPointer = argIt++;
    llvm::Value *yPointer = argIt++;
    llvm::Value *zPointer = argIt++;
    llvm::Value *resultPointer = argIt++;
    llvm::Value *count = argIt;

    llvm::Value *iVar = b.CreateAlloca(b.getInt32Ty());
    b.CreateStore(b.getInt32(0), iVar);
    b.CreateBr(loopBB);

    b.SetInsertPoint(loopBB);
    llvm::Value *i = b.CreateLoad(iVar);
    if (context.vectorize) {
        llvm::Value *x = llvm::UndefValue::get(b.getFPType());
        llvm::Value *y = llvm::UndefValue::get(b.getFPType());
        llvm::Value *z = llvm::UndefValue::get(b.getFPType());
        for (unsigned index = 0; index < VECTOR_ELEMENT_COUNT; ++index) {
            llvm::Value *sourceIndex = b.CreateAdd(i, b.getInt32(index));
            llvm::Value *xElem = b.CreateLoad(b.CreateGEP(xPointer, sourceIndex));
            llvm::Value *yElem = b.CreateLoad(b.CreateGEP(yPointer, sourceIndex));
            llvm::Value *zElem = b.CreateLoad(b.CreateGEP(zPointer, sourceIndex));

            x = b.CreateInsertElement(x, xElem, b.getInt32(index));
            y = b.CreateInsertElement(y, yElem, b.getInt32(index));
            z = b.CreateInsertElement(z, zElem, b.getInt32(index));
        }

        llvm::Value *result = b.CreateCall3(execFunction, x, y, z);
        for (unsigned index = 0; index < VECTOR_ELEMENT_COUNT; ++index) {
            llvm::Value *resultElement = b.CreateExtractElement(result, b.getInt32(index));
            b.CreateStore(resultElement, b.CreateGEP(resultPointer, b.CreateAdd(i, b.getInt32(index))));
        }

        i = b.CreateAdd(i, b.getInt32(VECTOR_ELEMENT_COUNT));
    }
    else {
        llvm::Value *x = b.CreateLoad(b.CreateGEP(xPointer, i));
        llvm::Value *y = b.CreateLoad(b.CreateGEP(yPointer, i));
        llvm::Value *z = b.CreateLoad(b.CreateGEP(zPointer, i));

        llvm::Value *result = b.CreateCall3(execFunction, x, y, z);
        b.CreateStore(result, b.CreateGEP(resultPointer, i));
        i = b.CreateAdd(i, b.getInt32(1));
    }
    b.CreateStore(i, iVar);
    llvm::Value *cond = b.CreateICmpULT(i, count);
    b.CreateCondBr(cond, loopBB, loopEndBB);

    b.SetInsertPoint(loopEndBB);
    b.CreateRetVoid();

    return unpackedSourceFunc;
}

NoiseProgram ExecutionContext::CreateNoiseProgram(const module::Module &baseModule)
{


    Builder b(*impl->m_Module, impl->m_Context);


    std::vector<llvm::Type*> params;
    params.push_back(b.getFPType());
    params.push_back(b.getFPType());
    params.push_back(b.getFPType());

    llvm::FunctionType *execFunctionType = llvm::FunctionType::get(b.getFPType(), params, false);
    llvm::Function *execFunction = llvm::Function::Create(execFunctionType, llvm::Function::InternalLinkage, "execFunction", impl->m_Module);

    llvm::BasicBlock *execEntryBB = llvm::BasicBlock::Create(b.getContext(), "entry", execFunction);
    b.SetInsertPoint(execEntryBB);

    auto argIt = execFunction->arg_begin();
    llvm::Value *x, *y, *z;
    x = argIt++;
    y = argIt++;
    z = argIt;


    initializeCodeGenContext(b, baseModule);
    llvm::Value *codeGenContext = b.getCodeGenContext().CreateAllocaContext();

    llvm::Value *result = baseModule.GetCodeGenerator()->generate(b, x, y, z, codeGenContext);

    b.CreateRet(result);

    llvm::Function *packedSourceFunc = constructPackedSourceFunction(impl->m_Context, impl->m_Module, execFunction, b);
    llvm::Function *unpackedSourceFunc = constructUnpackedSourceFunction(impl->m_Context, impl->m_Module, execFunction, b);

    std::error_code fileOpenError;
    llvm::AssemblyAnnotationWriter asmAnnoWriter;
    llvm::raw_fd_ostream moduleOut("module.ll", fileOpenError, llvm::sys::fs::F_None);
    impl->m_Module->print(moduleOut, &asmAnnoWriter);

    llvm::raw_fd_ostream out("verifier.log", fileOpenError, llvm::sys::fs::F_None);
    if (llvm::verifyModule(*impl->m_Module, &out)) {
        assert("Invalid module" && 0);
    }

    llvm::FunctionPassManager *FPM = new llvm::FunctionPassManager(impl->m_Module);
    llvm::PassManager *PM = new llvm::PassManager();

    llvm::PassManagerBuilder passManagerBuilder;
    passManagerBuilder.SizeLevel = 0;
    passManagerBuilder.OptLevel = 3;

    passManagerBuilder.populateFunctionPassManager(*FPM);
    passManagerBuilder.populateModulePassManager(*PM);
    //passManagerBuilder.populateLTOPassManager(*PM, true, true);

    // Create a function pass manager for this engine

    /*// Set up the optimizer pipeline.  Start with registering info about how the
    // target lays out data structures.
    //FPM->add(impl->m_ExecutionEngine->getDataLayout());
    // Provide basic AliasAnalysis support for GVN.
    FPM->add(llvm::createBasicAliasAnalysisPass());
    // Promote allocas to registers.
    FPM->add(llvm::createPromoteMemoryToRegisterPass());
    // Do simple "peephole" optimizations and bit-twiddling optzns.
    FPM->add(llvm::createInstructionCombiningPass());
    // Reassociate expressions.
    FPM->add(llvm::createReassociatePass());
    // Eliminate Common SubExpressions.
    FPM->add(llvm::createGVNPass());
    // Simplify the control flow graph (deleting unreachable blocks, etc).
    FPM->add(llvm::createCFGSimplificationPass());
    */

    FPM->doInitialization();
    for (llvm::Module::iterator funcIt = impl->m_Module->begin(); funcIt != impl->m_Module->end(); ++funcIt) {
        FPM->run(*funcIt);
    }
    delete FPM;

    PM->run(*impl->m_Module);
    delete PM;

    llvm::raw_fd_ostream moduleOutOpt("moduleOpt.ll", fileOpenError, llvm::sys::fs::F_None);
    impl->m_Module->print(moduleOutOpt, &asmAnnoWriter);

//    std::string errorStr;
//    impl->m_ExecutionEngine = llvm::EngineBuilder(impl->m_Module)
//            .setErrorStr(&errorStr)
//            .setEngineKind(llvm::EngineKind::JIT)
//            .setUseMCJIT(false)
//            .setOptLevel(llvm::CodeGenOpt::Aggressive)
//            .create();
//    if (!impl->m_ExecutionEngine) {
//        std::cerr <<errorStr << std::endl;
//    }

//    return NoiseProgram(impl->m_Context,
//                         (void*)impl->m_ExecutionEngine->getPointerToFunction(unpackedSourceFunc),
//                         (void*)impl->m_ExecutionEngine->getPointerToFunction(packedSourceFunc));

    std::string errorStr;
    impl->m_ExecutionEngine = llvm::EngineBuilder(std::unique_ptr<llvm::Module>(impl->m_Module))
            .setErrorStr(&errorStr)
            .setMCJITMemoryManager(std::unique_ptr<llvm::RTDyldMemoryManager>(new HelpingMemoryManager()))
            .setEngineKind(llvm::EngineKind::JIT)
            .setOptLevel(llvm::CodeGenOpt::Aggressive)
            .setMAttrs(std::vector<std::string>({"+sse2", "+sse3", "+sse4.1", "+64bit", "+avx", "+mmx"}))
            .create();
    if (!impl->m_ExecutionEngine) {
        std::cerr <<errorStr << std::endl;
    }

    impl->m_ExecutionEngine->finalizeObject();

    uint64_t unpackedAddress = impl->m_ExecutionEngine->getFunctionAddress(unpackedSourceFunc->getName());
    uint64_t packedAddress = impl->m_ExecutionEngine->getFunctionAddress(packedSourceFunc->getName());


    assert(unpackedAddress);
    assert(packedAddress);
    return NoiseProgram(impl->m_Context,
                         (void*)unpackedAddress,
                         (void*)packedAddress);



}

