#include <iostream>
#include "jitnoise/module/module.h"
#include "jitnoise/executioncontext.h"

#include <chrono>
#include <random>


using namespace std;
using namespace noise;
typedef double FP;
extern unsigned fModFCalls;
const unsigned BUFFER_SIZE = 256 * 256;
const unsigned TEST_COUNT = 200;

template <typename FPTYPE> FloatingPointMode getFloatingPointMode() {
    return FLOATING_POINT_MODE_SINGLE;
}

template <> FloatingPointMode getFloatingPointMode<float>() {
    return FLOATING_POINT_MODE_SINGLE;
}

template <> FloatingPointMode getFloatingPointMode<double>() {
    return FLOATING_POINT_MODE_DOUBLE;
}


int main()
{
    module::Perlin perlin;

    FP *sourceX = new FP[BUFFER_SIZE + 1];
    FP *sourceY = new FP[BUFFER_SIZE + 1];
    FP *sourceZ = new FP[BUFFER_SIZE + 1];
    for (unsigned i = 0; i < BUFFER_SIZE; ++i) {
        sourceX[i] = i * 5.2;
        sourceY[i] = i * 2.6;
        sourceZ[i] = i * 12.24;
    }

    Context ctx;
    ctx.floatingPointMode = getFloatingPointMode<FP>();
    ctx.vectorize = true;

    std::cout << "Creating noise program\n";
    ExecutionContext execContext(ctx);
    NoiseProgram program = execContext.CreateNoiseProgram(perlin);

    std::cout << "Testing\n";
    auto startTime = std::chrono::high_resolution_clock::now();

    for (unsigned i = 0; i < TEST_COUNT; ++i) {
        program.execute(sourceX, sourceY, sourceZ, sourceX, BUFFER_SIZE);
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    std::cout << "Time: " << std::chrono::duration<double>(endTime - startTime).count() << std::endl;
    std::cout << "fModFCalls: " << fModFCalls << std::endl;
    return 0;
}

