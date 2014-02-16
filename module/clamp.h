#ifndef CLAMP_H
#define CLAMP_H
#include "module.h"
namespace jitnoise {
	class Clamp : public Module {
		public:
			Clamp();
			~Clamp();
			CodeGenerator *codeGenerator() const { return mCodeGenerator; }
			double lowerBound() const { return mLowerBound; }
			double upperBound() const { return mUpperBound; }
			void setUpperBound(double b) { mUpperBound = b; }
			void setLowerBound(double b) { mLowerBound = b; }
			void setBounds(double upper, double lower) { mUpperBound = upper; mLowerBound = lower; }
		private:
			CodeGenerator *mCodeGenerator;
			double mLowerBound;
			double mUpperBound;
	};
}
#endif // CLAMP_H
