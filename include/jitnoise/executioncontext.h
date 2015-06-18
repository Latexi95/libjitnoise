#ifndef EXECUTIONCONTEXT_H
#define EXECUTIONCONTEXT_H
#include "context.h"
#include "noiseprogram.h"


namespace noise {
namespace module {
class Module;
}

class ExecutionContext
{
    struct ExecutionContextImpl;
public:
    ExecutionContext(const Context &context);
    ~ExecutionContext();

    void SetContext(const Context &context);
    Context GetContext() const;

    NoiseProgram CreateNoiseProgram(const module::Module &baseModule);

private:
    ExecutionContextImpl *impl;
};

}

#endif // EXECUTIONCONTEXT_H
