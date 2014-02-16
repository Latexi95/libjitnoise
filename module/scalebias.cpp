#include "scalebias.h"
#include "../codegenerator.h"
namespace jitnoise {
	class ScaleBiasCodeGenerator : public CodeGenerator {
		llvm::Value *generate(Builder *builder, const Module *module, llvm::Value *x, llvm::Value *y, llvm::Value *z, llvm::Value *generationData) {
			const ScaleBias *scaleBias = static_cast<const ScaleBias*>(module);
			assert(scaleBias->getSourceModule(0));

			llvm::Value *a = scaleBias->getSourceModule(0)->generate(builder, x, y, z, generationData);
			return builder->fadd(builder->fmul(a, builder->getFPValue(scaleBias->scale())), builder->getFPValue(scaleBias->bias()));
		}
	};

	ScaleBias::ScaleBias() :
	Module(1),
	mCodeGenerator(new ScaleBiasCodeGenerator),
	mBias(SCALEBIAS_DEFAULT_BIAS),
	mScale(SCALEBIAS_DEFAULT_SCALE) {
	}

	ScaleBias::~ScaleBias() {
		delete mCodeGenerator;
	}

}
