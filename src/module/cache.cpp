#include "jitnoise/module/cache.h"
#include "codegenerator.h"
#include "codegencontext.h"
using namespace noise::module;
using namespace noise;

class CacheCodeGenerator : public CodeGenerator
{
public:
    CacheCodeGenerator(Cache *a) : m_Module(a), m_Function(0) {}
    llvm::Value *generate(Builder &b, llvm::Value *x, llvm::Value *y, llvm::Value *z, llvm::Value *codeGenContext) {

        if (!m_Function) {
            llvm::Value *x, *y, *z, *codeGenContext;
            m_Function = b.CreateModuleFunction(x, y, z, codeGenContext);

            Builder::InsertPoint ip = b.saveAndClearIP();

            llvm::BasicBlock *entryBB = llvm::BasicBlock::Create(b.getContext(), "entry", m_Function);
            llvm::BasicBlock *checkCachedBB = llvm::BasicBlock::Create(b.getContext(), "checkCached", m_Function);
            llvm::BasicBlock *regenBB = llvm::BasicBlock::Create(b.getContext(), "regen", m_Function);
            llvm::BasicBlock *returnCachedBB = llvm::BasicBlock::Create(b.getContext(), "returnCached", m_Function);


            b.SetInsertPoint(entryBB);
            llvm::Value *isCached = b.getCodeGenContext().CreateLoadVariable(codeGenContext, m_IsCachedVarId);
            b.CreateCondBr(isCached, checkCachedBB, regenBB);


            b.SetInsertPoint(checkCachedBB);

            llvm::Value *xCached = b.getCodeGenContext().CreateLoadVariable(codeGenContext, m_xCacheVarId);
            llvm::Value *yCached = b.getCodeGenContext().CreateLoadVariable(codeGenContext, m_yCacheVarId);
            llvm::Value *zCached = b.getCodeGenContext().CreateLoadVariable(codeGenContext, m_zCacheVarId);


            llvm::Value *xEq = b.CreateFCmpEQUnite(x, xCached);
            llvm::Value *yEq = b.CreateFCmpEQUnite(y, yCached);
            llvm::Value *zEq = b.CreateFCmpEQUnite(z, zCached);

            llvm::Value *eq = b.CreateAnd(xEq, b.CreateAnd(yEq, zEq));

            b.CreateCondBr(eq, returnCachedBB, regenBB);


            b.SetInsertPoint(regenBB);

            llvm::Value *value = m_Module->GetSourceModule(0).GetCodeGenerator()->generate(b, x, y, z, codeGenContext);
            b.getCodeGenContext().CreateStoreVariable(codeGenContext, m_xCacheVarId, x);
            b.getCodeGenContext().CreateStoreVariable(codeGenContext, m_yCacheVarId, x);
            b.getCodeGenContext().CreateStoreVariable(codeGenContext, m_zCacheVarId, x);
            b.getCodeGenContext().CreateStoreVariable(codeGenContext, m_CachedValueVarId, value);
            b.CreateRet(value);


            b.SetInsertPoint(returnCachedBB);

            value = b.getCodeGenContext().CreateLoadVariable(codeGenContext, m_CachedValueVarId);
            b.CreateRet(value);


            b.restoreIP(ip);
        }
        return b.CreateCallModuleFunction(m_Function, x, y, z, codeGenContext);
    }

    void initializeContext(Builder &builder) {
        m_IsCachedVarId = builder.getCodeGenContext().CreateVariable(builder.getInt1Ty(), builder.getInt1(false));
        m_xCacheVarId = builder.getCodeGenContext().CreateVariable(builder.getFPType(), builder.getFPValue(0));
        m_yCacheVarId = builder.getCodeGenContext().CreateVariable(builder.getFPType(), builder.getFPValue(0));
        m_zCacheVarId = builder.getCodeGenContext().CreateVariable(builder.getFPType(), builder.getFPValue(0));
        m_CachedValueVarId = builder.getCodeGenContext().CreateVariable(builder.getFPType(), builder.getFPValue(0));
    }

private:
    Cache *m_Module;
    llvm::Function *m_Function;
    uint32_t m_IsCachedVarId;
    uint32_t m_xCacheVarId;
    uint32_t m_yCacheVarId;
    uint32_t m_zCacheVarId;
    uint32_t m_CachedValueVarId;
};
Cache::Cache ():
  Module (GetSourceModuleCount (), new CacheCodeGenerator(this))
{
}

