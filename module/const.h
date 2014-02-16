#ifndef CONST_H
#define CONST_H
#include "module.h"
namespace jitnoise {
	const double CONST_DEFAULT_VALUE = 0.0;
	class Const : public Module {
		public:
			Const();
			~Const();
			void setValue(double val) { mConstValue = val; }
			double value() const { return mConstValue; }
			CodeGenerator *codeGenerator() const { return mCodeGenerator; }
		private:
			CodeGenerator *mCodeGenerator;
			double mConstValue;
	};
}
#endif // CONST_H
