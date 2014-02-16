#ifndef MAX_H
#define MAX_H
#include "module.h"
namespace jitnoise {
	class Max : public Module {
		public:
			Max();
			~Max();
			CodeGenerator *codeGenerator() const { return mCodeGenerator; }
		private:
			CodeGenerator *mCodeGenerator;
	};
}
#endif // MAX_H
