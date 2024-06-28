void rndSetSeed(u32 seed);
void pushRndSeed(u32 seed);
u32 popRndSeed();
u32 rnd();
float FORCE_INLINE rndFloat(float min, float max);
int FORCE_INLINE rndInt(int min, int max);
bool FORCE_INLINE rndBool();
bool FORCE_INLINE rndPerc(float perc);
int rndPick(int *weights, int weightsNum);
int rndPick(float *weights, int weightsNum);
Vec2 getRandomPoint(Tri2 tri);

void rndTest();

#define LCG_SEED_STACK_MAX 4096
u32 lcgSeedStack[LCG_SEED_STACK_MAX];
int lcgSeedStackNum = 0;

u32 rndMax = pow(2, 32)-1;
u32 lcgSeed = 1;

void pushRndSeed(u32 seed) {
	if (lcgSeedStackNum > LCG_SEED_STACK_MAX-1) Panic("Lcg seed stack overflow\n");

	lcgSeedStack[lcgSeedStackNum++] = lcgSeed;
	rndSetSeed(seed);
}

u32 popRndSeed() {
	if (lcgSeedStackNum <= 0) { 
		logf("Rnd stack was empty\n");
		lcgSeed = time(NULL);
		return lcgSeed;
	}

	u32 oldSeed = lcgSeed;
	rndSetSeed(lcgSeedStack[lcgSeedStackNum-1]);
	lcgSeedStackNum--;

	return oldSeed;
}

// https://github.com/ESultanik/mtwister
#define STATE_VECTOR_LENGTH 624
#define STATE_VECTOR_M      397 /* changes to STATE_VECTOR_LENGTH also require changes to this */

u32 mtArray[STATE_VECTOR_LENGTH];
s32 mtIndex;

void rndSetSeed(u32 seed) {
  mtArray[0] = seed & 0xffffffff;
  for(mtIndex=1; mtIndex<STATE_VECTOR_LENGTH; mtIndex++) {
    mtArray[mtIndex] = (6069 * mtArray[mtIndex-1]) & 0xffffffff;
  }
	lcgSeed = seed;
}

u32 rnd() {
#define MT_UPPER_MASK       0x80000000
#define MT_LOWER_MASK       0x7fffffff
#define MT_TEMPERING_MASK_B 0x9d2c5680
#define MT_TEMPERING_MASK_C 0xefc60000

  u32 y;
  static u32 mag[2] = {0x0, 0x9908b0df}; /* mag[x] = x * 0x9908b0df for x = 0,1 */
  if (mtIndex >= STATE_VECTOR_LENGTH || mtIndex < 0) {
    s32 kk;
    if (mtIndex >= STATE_VECTOR_LENGTH+1 || mtIndex < 0) rndSetSeed(4357);
    for (kk=0; kk<STATE_VECTOR_LENGTH-STATE_VECTOR_M; kk++) {
      y = (mtArray[kk] & MT_UPPER_MASK) | (mtArray[kk+1] & MT_LOWER_MASK);
      mtArray[kk] = mtArray[kk+STATE_VECTOR_M] ^ (y >> 1) ^ mag[y & 0x1];
    }
    for (; kk<STATE_VECTOR_LENGTH-1; kk++) {
      y = (mtArray[kk] & MT_UPPER_MASK) | (mtArray[kk+1] & MT_LOWER_MASK);
      mtArray[kk] = mtArray[kk+(STATE_VECTOR_M-STATE_VECTOR_LENGTH)] ^ (y >> 1) ^ mag[y & 0x1];
    }
    y = (mtArray[STATE_VECTOR_LENGTH-1] & MT_UPPER_MASK) | (mtArray[0] & MT_LOWER_MASK);
    mtArray[STATE_VECTOR_LENGTH-1] = mtArray[STATE_VECTOR_M-1] ^ (y >> 1) ^ mag[y & 0x1];
    mtIndex = 0;
  }
  y = mtArray[mtIndex++];
  y ^= (y >> 11);
  y ^= (y << 7) & MT_TEMPERING_MASK_B;
  y ^= (y << 15) & MT_TEMPERING_MASK_C;
  y ^= (y >> 18);
  return y;
}

float rndFloat(float min, float max) { return min + (rnd() / (float)rndMax) * (max - min); }
int rndInt(int min, int max) { 
	if (min >= max) return min;
	return min + rnd() / (rndMax / (max - min + 1) + 1);
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

Vec2 getRandomPoint(Tri2 tri) {
	float r1 = rndFloat(0, 1);
	float r2 = rndFloat(0, 1);
	Vec2 point;
	point.x = (1 - sqrt(r1)) * tri.verts[0].x + (sqrt(r1) * (1 - r2)) * tri.verts[1].x + (sqrt(r1) * r2) * tri.verts[2].x;
	point.y = (1 - sqrt(r1)) * tri.verts[0].y + (sqrt(r1) * (1 - r2)) * tri.verts[1].y + (sqrt(r1) * r2) * tri.verts[2].y;
	return point;
}

void rndTest() {
	// NanoTime startTime = getNanoTime();

	logf("Running rng test\n");
	int iterationsPerTest = 100000;
	bool doDistribution = true;

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

	// logf("Done, took %fms\n", getMsPassed(startTime));
}
