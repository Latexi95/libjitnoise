#ifndef TURBULENCE_H
#define TURBULENCE_H
#include "module.h"
#include "perlin.h"

namespace jitnoise {
	class Turbulence : public Module {
		public:
			Turbulence();
			~Turbulence();
			CodeGenerator *codeGenerator() const { return mCodeGenerator; }
			void setSeed(int seed);
			int seed() const;
			void setRoughness(int roughness);
			int roughness() const;
			void setPower(double power);
			double power() const;
			void setFrequency(double frequency);
			double frequency() const;
			const Perlin *xDistortModule() const { return &mXDistortModule; }
			const Perlin *yDistortModule() const { return &mYDistortModule; }
			const Perlin *zDistortModule() const { return &mZDistortModule; }

		private:
			CodeGenerator *mCodeGenerator;
			Perlin mXDistortModule;
			Perlin mYDistortModule;
			Perlin mZDistortModule;
			double mPower;
	};
}
#endif // TURBULENCE_H
