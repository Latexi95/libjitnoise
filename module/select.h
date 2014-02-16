#ifndef SELECT_H
#define SELECT_H
#include "module.h"
namespace jitnoise {
	const double DEFAULT_SELECT_EDGE_FALLOFF = 0.0;
	const double DEFAULT_SELECT_LOWER_BOUND = -1.0;
	const double DEFAULT_SELECT_UPPER_BOUND = 1.0;
	class Select : public Module {
		public:
			Select();
			~Select();
			CodeGenerator *codeGenerator() const { return mCodeGenerator; }
			double lowerBound() const { return mLowerBound; }
			double upperBound() const { return mUpperBound; }
			void setUpperBound(double b) { mUpperBound = b; }
			void setLowerBound(double b) { mLowerBound = b; }
			void setBounds(double lower, double upper) { mLowerBound = lower; mUpperBound = upper; }
			double edgeFalloff() const { return mEdgeFalloff; }
			void setEdgeFalloff(double ef) { mEdgeFalloff = ef; }
			void setControlModule(Module *module) { setSourceModule(2, module); }
			Module *controlModule() const { return getSourceModule(2); }

		private:
			CodeGenerator *mCodeGenerator;
			double mEdgeFalloff;
			double mLowerBound;
			double mUpperBound;

	};
}
#endif // SELECT_H
