void pushRndSeed(u32 newSeed);
u32 popRndSeed();
float rnd();
float FORCE_INLINE rndFloat(float min, float max);
int FORCE_INLINE rndInt(int min, int max);
bool FORCE_INLINE rndBool();
bool FORCE_INLINE rndPerc(float perc);
int rndPick(int *weights, int weightsNum);
void rndTest();

#define LCG_SEED_STACK_MAX 4096
u32 lcgSeedStack[LCG_SEED_STACK_MAX];
int lcgSeedStackNum = 0;

u32 lcgM = pow(2, 32)-1;
u32 lcgSeed = 1;

void pushRndSeed(u32 newSeed) {
	if (lcgSeedStackNum > LCG_SEED_STACK_MAX-1) Panic("Lcg seed stack overflow\n");

	lcgSeedStack[lcgSeedStackNum++] = lcgSeed;
	lcgSeed = newSeed;
	// rnd_pcg_seed(&pcg, lcgSeed);
}

u32 popRndSeed() {
	if (lcgSeedStackNum <= 0) { 
		logf("Rnd stack was empty\n");
		lcgSeed = time(NULL);
		return lcgSeed;
	}

	u32 oldSeed = lcgSeed;
	lcgSeed = lcgSeedStack[lcgSeedStackNum-1];
	lcgSeedStackNum--;
	// rnd_pcg_seed(&pcg, lcgSeed);

	// logf("Giving %d\n", oldSeed);
	return oldSeed;
}

float rnd() {
	u32 lcgA = 1664525;
	u32 lcgC = 1013904223;
	int max = lcgM/2;

	lcgSeed = (lcgA * lcgSeed + lcgC) % lcgM;
	int value = lcgSeed % (max + 1);

	float result = (float)value/max;
	if (result == 1) result = rnd();
	return result;
}

float rndFloat(float min, float max) { return min + rnd() * (max - min); }
int rndInt(int min, int max) { return rndFloat(min, max); } //@incomplete This could be better distributed. :/
bool rndBool() { return rnd() > 0.5; }
bool rndPerc(float perc) { return rnd() < perc; }

int rndPick(int *weights, int weightsNum) {
	int totalItems = 0;
	for (int i = 0; i < weightsNum; i++) {
		totalItems += weights[i];
	}

	int choosenItem = rndInt(1, totalItems);
	for (int i = 0; i < weightsNum; i++) {
		for (int j = 0; j < weights[i]; j++) {
			choosenItem--;
			if (choosenItem <= 0) return i;
		}
	}

	logf("rndPick failed horribly");
	return 0;
}

int rndPick(float *weights, int weightsNum) {
	float sumOfWeight = 0;
	for(int i = 0; i < weightsNum; i++) {
		sumOfWeight += weights[i];
	}

	float rnd = rndFloat(0, sumOfWeight);
	for(int i = 0; i < weightsNum; i++) {
		rnd -= weights[i];
		if (rnd <= 0) return i;
	}

	logf("rndPick(float) failed horribly");
	return 0;
}

void rndTest() {
	logf("Running rng test\n");
	int iterationsPerTest = 100;
	bool doDistribution = false;

	int *values = (int *)frameMalloc(sizeof(int) * iterationsPerTest);
	int valuesNum = 0;

	for (int min = 0; min < 10; min++) { 
		for (int max = 0; max < 10; max++) { 
			if (min > max) continue;

			valuesNum = 0;
			for (int i = 0; i < iterationsPerTest; i++) {
				int value = rndInt(min, max);
				if (value < min || value > max) logf("Failed, got %d [%d, %d]\n", value, min, max);
				values[valuesNum++] = value;
			}

			for (int i = min; i <= max; i++) {
				int testValue = i;
				int count = 0;
				for (int i = 0; i < iterationsPerTest; i++) {
					if (values[i] == testValue) count++;
				}

				if (count == 0) logf("Failed, didn't get any %d's [%d, %d]\n", testValue, min, max);
				float distribution = count / (float)iterationsPerTest;
				float correctDistribution = 1 / (float)((max - min) + 1);
				float change = fabs(correctDistribution - distribution);
				float percChange = change / correctDistribution;
				if (doDistribution && percChange > 0.1) logf("Failed, bad distribution %d [%d, %d], %f should be %f\n", testValue, min, max, distribution, correctDistribution);
			}

		}
	}
}
