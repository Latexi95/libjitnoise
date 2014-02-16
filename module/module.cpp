#include "module.h"
#include <cassert>
#include "../codegenerator.h"
#include <algorithm>
namespace jitnoise {

Module::Module(int sourceModuleCount) :
	mSourceModuleCount(sourceModuleCount),
	mSourceModules(0)
{
	if (sourceModuleCount > 0) {
		mSourceModules = new Module*[sourceModuleCount];
		std::fill(mSourceModules, mSourceModules + sourceModuleCount, (Module*)0);
	}
}

Module::~Module() {
	delete[] mSourceModules;
}


Module *Module::getSourceModule(int index) const {
	assert(mSourceModules && index >= 0 && index < mSourceModuleCount);
	return mSourceModules[index];
}

void Module::setSourceModule(int index, Module *source) {
	assert(mSourceModules && index >= 0 && index < mSourceModuleCount);
	mSourceModules[index] = source;
}

llvm::Value *Module::generate(Builder *builder, llvm::Value *x, llvm::Value *y, llvm::Value *z, llvm::Value *generateData) const {
	return this->codeGenerator()->generate(builder, this, x, y, z, generateData);
}
}
