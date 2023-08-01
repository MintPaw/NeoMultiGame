void pushRndSeed(u32 newSeed);
u32 popRndSeed();
u32 rnd();
float FORCE_INLINE rndFloat(float min, float max);
int FORCE_INLINE rndInt(int min, int max);
bool FORCE_INLINE rndBool();
bool FORCE_INLINE rndPerc(float perc);
int rndPick(int *weights, int weightsNum);
int rndPick(float *weights, int weightsNum);
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

	return oldSeed;
}

u32 rnd() {
	u32 lcgA = 1664525;
	u32 lcgC = 1013904223;

	lcgSeed = (lcgA * lcgSeed + lcgC) & lcgM;
	return lcgSeed;
}

float rndFloat(float min, float max) { return min + (rnd() / (float)lcgM) * (max - min); }
int rndInt(int min, int max) { 
	if (min >= max) return min;
	return min + rnd() / (lcgM / (max - min + 1) + 1);
}
bool rndBool() { return rndFloat(0, 1) > 0.5; }
bool rndPerc(float perc) { return rndFloat(0, 1) < perc; }
Vec2 rndVec2(float min, float max) { return v2(rndFloat(min, max), rndFloat(min, max)); }

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

	logf("rndPick failed horribly\n");
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

	return 0;
}
