#include "exponent.h"
#include "../builder.h"
#include "../codegenerator.h"
namespace jitnoise {

class ExponentGenerator : public CodeGenerator {
	public:
		llvm::Value *generate(Builder *builder, const Module *module, llvm::Value *x, llvm::Value *y, llvm::Value *z, llvm::Value *generationData) {
			const Exponent *exponentModule = static_cast<const Exponent*>(module);
			llvm::Value *source = exponentModule->getSourceModule(0)->generate(builder, x, y, z, generationData);
			return builder->fsub(builder->fmul(builder->fpow(builder->fabs(builder->fmul(builder->fadd(source, builder->getFPValue(1.0)), builder->getFPValue(0.5))),
															 builder->getFPValue(exponentModule->exponent())), builder->getFPValue(2.0)), builder->getFPValue(1.0));
		}
};

Exponent::Exponent() : Module(1), mCodeGenerator(new ExponentGenerator()), mExponent(EXPONENT_DEFAULT_EXPONENT) {
}

Exponent::~Exponent() {
	delete mCodeGenerator;
}

}
