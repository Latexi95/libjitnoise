#ifndef VORONOI_H
#define VORONOI_H
#include "module.h"
namespace jitnoise {

const double VORONOI_DEFAULT_FREQUENCY = 1.0;
const double VORONOI_DEFAULT_DISPLACEMENT = 1.0;
const int VORONOI_DEFAULT_SEED = 0;

class Voronoi : public Module {
	public:
		Voronoi();
		~Voronoi();

		CodeGenerator *codeGenerator() const;

		double frequency() const { return mFrequency; }
		void setFrequency(double f) { mFrequency = f; }
		int seed() const { return mSeed; }
		void setSeed(int seed) { mSeed = seed; }
		double displacement() const { return mDisplacement; }
		void setDisplacement(double d) { mDisplacement = d; }
		void enableDistance(bool t) { mEnableDistance = t; }
		bool isDistanceEnabled() const { return mEnableDistance; }
	private:
		double mFrequency;
		double mDisplacement;
		bool mEnableDistance;
		int mSeed;
		CodeGenerator *mCodeGenerator;
};

}
#endif // VORONOI_H
