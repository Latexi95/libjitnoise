#ifndef CONTEXT_H
#define CONTEXT_H
namespace jitnoise {

struct Context {
	enum FloatingPointPrecision {
		Single,
		Double
	};
	enum NoiseQuality {
		Fast,
		Standard,
		Best
	};

	Context();
	FloatingPointPrecision mFloatingPointPrecision;
	NoiseQuality mNoiseQuality;
	bool mVectorize;
	bool mFlushDenormalValuesToZero;
	bool mPreciseFloatingPointDivision;
	bool mPreciseSquareRoot;
	int mSeed;
};

}

#endif // CONTEXT_H
