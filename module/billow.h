#ifndef BILLOW_H
#define BILLOW_H
#include "module.h"
namespace jitnoise {

const double BILLOW_DEFAULT_FREQUENCY = 1.0;
const double BILLOW_DEFAULT_LACUNARITY = 2.0;
const int BILLOW_DEFAULT_OCTAVE_COUNT = 6;
const double BILLOW_DEFAULT_PERSISTENCE = 0.5;
const int BILLOW_DEFAULT_SEED = 0;
const int BILLOW_MAX_OCTAVE = 30;

class Billow : public Module {
	public:
		Billow();
		~Billow();

		CodeGenerator *codeGenerator() const;

		double frequency() const { return mFrequency; }
		void setFrequency(double f) { mFrequency = f; }
		double lacunarity() const { return mLacunarity; }
		void setLacunarity(double l) { mLacunarity = l; }
		int octaveCount() const { return mOctaveCount; }
		void setOctaveCount(int c) { mOctaveCount = c; }
		double persistence() const { return mPersistence; }
		void setPersistence(double p) { mPersistence = p; }
		int seed() const { return mSeed; }
		void setSeed(int seed) { mSeed = seed; }
	private:
		double mFrequency;
		double mLacunarity;
		int mOctaveCount;
		double mPersistence;
		int mSeed;
		CodeGenerator *mCodeGenerator;
};

}

#endif // BILLOW_H
