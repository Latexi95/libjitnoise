#ifndef SCALEBIAS_H
#define SCALEBIAS_H
#include "module.h"
namespace jitnoise {
	const double SCALEBIAS_DEFAULT_SCALE = 1.0;
	const double SCALEBIAS_DEFAULT_BIAS = 0.0;
	class ScaleBias: public Module {
		public:
			ScaleBias();
			~ScaleBias();
			CodeGenerator *codeGenerator() const { return mCodeGenerator; }
			void setBias(double b) { mBias = b; }
			double bias() const { return mBias; }
			void setScale(double s) { mScale = s; }
			double scale() const { return mScale; }
		private:
			CodeGenerator *mCodeGenerator;
			double mBias;
			double mScale;
	};
}
#endif // SCALEBIAS_H
