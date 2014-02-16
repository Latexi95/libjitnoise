#include "turbulence.h"
#include "../builder.h"
#include "../codegenerator.h"
namespace jitnoise {

class TurbulenceGenerator : public CodeGenerator {
	public:
		TurbulenceGenerator() : mFunction(0) {}
		llvm::Value *generate(Builder *builder, const Module *module, llvm::Value *x, llvm::Value *y, llvm::Value *z, llvm::Value *generationData) {
			const Turbulence *turbulenceModule = static_cast<const Turbulence*>(module);
			if (!mFunction) generateFunction(builder, turbulenceModule);

			return builder->createModuleFunctionCall(mFunction, x, y, z, generationData);
		}

		void generateFunction(Builder *tempBuilder, const Turbulence* turbulence) {
			assert(turbulence->getSourceModule(0));
			llvm::Value *x, *y, *z, *generationData;
			llvm::BasicBlock *basicBlock;
			Builder *builder = tempBuilder->createModuleFunction("turbulence", &x, &y, &z, &generationData, &basicBlock);
			mFunction = builder->function();
			llvm::Value *x0 = builder->fadd(x, builder->getFPValue(12414.0 / 65536.0));
			llvm::Value *y0 = builder->fadd(x, builder->getFPValue(65124.0 / 65536.0));
			llvm::Value *z0 = builder->fadd(x, builder->getFPValue(31337.0 / 65536.0));
			llvm::Value *x1 = builder->fadd(x, builder->getFPValue(26519.0 / 65536.0));
			llvm::Value *y1 = builder->fadd(x, builder->getFPValue(18128.0 / 65536.0));
			llvm::Value *z1 = builder->fadd(x, builder->getFPValue(60493.0 / 65536.0));
			llvm::Value *x2 = builder->fadd(x, builder->getFPValue(53820.0 / 65536.0));
			llvm::Value *y2 = builder->fadd(x, builder->getFPValue(11213.0 / 65536.0));
			llvm::Value *z2 = builder->fadd(x, builder->getFPValue(44845.0 / 65536.0));
			llvm::Value *xDistort = builder->fadd(x, builder->fmul(turbulence->xDistortModule()->generate(builder, x0, y0, z0, generationData), builder->getFPValue(turbulence->power())));
			llvm::Value *yDistort = builder->fadd(y, builder->fmul(turbulence->yDistortModule()->generate(builder, x1, y1, z1, generationData), builder->getFPValue(turbulence->power())));
			llvm::Value *zDistort = builder->fadd(z, builder->fmul(turbulence->zDistortModule()->generate(builder, x2, y2, z2, generationData), builder->getFPValue(turbulence->power())));

			llvm::Value *ret = turbulence->getSourceModule(0)->generate(builder, xDistort, yDistort, zDistort, generationData);
			builder->getIRBuilder()->CreateRet(ret);
		}

	private:
		llvm::Function *mFunction;
};

Turbulence::Turbulence() : Module(1), mCodeGenerator(new TurbulenceGenerator()){
}

Turbulence::~Turbulence() {
	delete mCodeGenerator;
}

void Turbulence::setSeed(int seed) {
	mXDistortModule.setSeed(seed);
	mYDistortModule.setSeed(seed + 1);
	mZDistortModule.setSeed(seed + 2);
}

int Turbulence::seed() const {
	return mXDistortModule.seed();
}

void Turbulence::setRoughness(int roughness) {
	mXDistortModule.setOctaveCount(roughness);
	mYDistortModule.setOctaveCount(roughness);
	mZDistortModule.setOctaveCount(roughness);
}

int Turbulence::roughness() const {
	return mXDistortModule.octaveCount();
}

void Turbulence::setPower(double power) {
	mPower = power;
}

double Turbulence::power() const {
	return mPower;
}

void Turbulence::setFrequency(double frequency) {
	mXDistortModule.setFrequency(frequency);
	mYDistortModule.setFrequency(frequency);
	mZDistortModule.setFrequency(frequency);
}

double Turbulence::frequency() const {
	return mXDistortModule.frequency();
}


}
