#ifndef JITEXECUTIONCONTEXT_H
#define JITEXECUTIONCONTEXT_H
#include "executioncontext.h"

namespace jitnoise {

class JITExecutionContext : public ExecutionContext {
	public:
		JITExecutionContext();
		~JITExecutionContext();
		bool initialize(Context context);
		NoiseProgram *compile(Module *baseModule);

	private:
		Context mContext;
};

}

#endif // JITEXECUTIONCONTEXT_H
