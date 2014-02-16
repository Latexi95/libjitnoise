#include "clamp.h"
#include "../codegenerator.h"
namespace jitnoise {
	class ClampCodeGenerator : public CodeGenerator {
		llvm::Value *generate(Builder *builder, const Module *module, llvm::Value *x, llvm::Value *y, llvm::Value *z, llvm::Value *generationData) {
			const Clamp *clamp = static_cast<const Clamp*>(module);
			assert(clamp->getSourceModule(0));

			llvm::Value *a = clamp->getSourceModule(0)->generate(builder, x, y, z, generationData);
			return builder->clampFP(a, builder->getFPValue(clamp->lowerBound()), builder->getFPValue(clamp->upperBound()));
		}
	};

	Clamp::Clamp() :
	Module(1),
	mCodeGenerator(new ClampCodeGenerator){
	}

	Clamp::~Clamp() {
		delete mCodeGenerator;
	}

}
