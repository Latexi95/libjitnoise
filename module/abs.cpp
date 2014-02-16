#include "abs.h"
#include "../builder.h"
#include "../codegenerator.h"
namespace jitnoise {

class AbsGenerator : public CodeGenerator {
	public:
		llvm::Value *generate(Builder *builder, const Module *module, llvm::Value *x, llvm::Value *y, llvm::Value *z, llvm::Value *generationData) {
			const Abs *absModule = static_cast<const Abs*>(module);
			llvm::Value *source = absModule->getSourceModule(0)->generate(builder, x, y, z, generationData);
			return builder->fabs(source);
		}
};

Abs::Abs() : Module(1), mCodeGenerator(new AbsGenerator()){
}

Abs::~Abs() {
	delete mCodeGenerator;
}

}
