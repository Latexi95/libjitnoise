#ifndef EXPONENT_H
#define EXPONENT_H

#include "module.h"
namespace jitnoise {
	const double EXPONENT_DEFAULT_EXPONENT = 1.0;
	class Exponent : public Module {
		public:
			Exponent();
			~Exponent();
			CodeGenerator *codeGenerator() const { return mCodeGenerator; }
			double exponent() const { return mExponent; }
			void setExponent(double e) { mExponent = e; }
		private:
			CodeGenerator *mCodeGenerator;
			double mExponent;
	};
}
#endif // EXPONENT_H
