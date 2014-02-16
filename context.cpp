#include "context.h"

jitnoise::Context::Context() :
	mFloatingPointPrecision(Double),
	mNoiseQuality(Standard),
	mVectorize(false),
	mFlushDenormalValuesToZero(true),
	mPreciseFloatingPointDivision(true),
	mPreciseSquareRoot(true),
	mSeed(0)
{

}
