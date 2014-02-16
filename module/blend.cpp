#include "blend.h"
#include "../builder.h"
#include "../codegenerator.h"
namespace jitnoise {

class BlendGenerator : public CodeGenerator {
	public:
		llvm::Value *generate(Builder *builder, const Module *module, llvm::Value *x, llvm::Value *y, llvm::Value *z, llvm::Value *generationData) {
			const Blend *blendModule = static_cast<const Blend*>(module);
			llvm::Value *a = blendModule->getSourceModule(0)->generate(builder, x, y, z, generationData);
			llvm::Value *b = blendModule->getSourceModule(1)->generate(builder, x, y, z, generationData);
			llvm::Value *alpha = blendModule->getSourceModule(2)->generate(builder, x, y, z, generationData);
			alpha = builder->fmul(builder->fadd(alpha, builder->getFPValue(1.0)), builder->getFPValue(0.5));
			return builder->interpolateLinear(builder->getIRBuilder(), a, b, alpha);
		}
};

Blend::Blend() : Module(3), mCodeGenerator(new BlendGenerator()){
}

Blend::~Blend() {
	delete mCodeGenerator;
}

}
