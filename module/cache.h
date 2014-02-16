#ifndef CACHE_H
#define CACHE_H
#include "module.h"
namespace jitnoise {
	class Cache : public Module {
		public:
			Cache();
			~Cache();
			CodeGenerator *codeGenerator() const { return mCodeGenerator; }
		private:
			CodeGenerator *mCodeGenerator;
	};
}

#endif // CACHE_H
