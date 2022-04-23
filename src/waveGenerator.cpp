struct WaveGeneratorSource {
	float rate;
	float power;
	float phase;
	float phaseShift;
	Ease ease;
	float sample;

	float *samples;
	int samplesNum;
	int samplesMax;
};

struct WaveGenerator {
#define WAVE_GENERATOR_SOURCES_MAX 8
	WaveGeneratorSource sources[WAVE_GENERATOR_SOURCES_MAX];
	int sourcesNum;

	float lastSample;
	float sample;
	float time;

	bool recordSamples;
	float *samples;
	int samplesNum;
	int samplesMax;
};

float stepWaveGenerator(WaveGenerator *gen, float elapsed);
float stepWaveGenerator(WaveGenerator *gen, float elapsed) {
	if (gen->time == 0) {
		gen->samplesNum = 0;
		for (int i = 0; i < gen->sourcesNum; i++) {
			WaveGeneratorSource *src = &gen->sources[i];
			src->phase = 0;
		}
	}

	gen->lastSample = gen->sample;
	gen->sample = 1;

	if (gen->recordSamples) {
		if (gen->samplesNum > gen->samplesMax-1) {
			gen->samples = (float *)resizeArray(gen->samples, sizeof(float), gen->samplesMax, gen->samplesMax*2 + 1);
			gen->samplesMax = gen->samplesMax*2 + 1;
		}

		for (int i = 0; i < gen->sourcesNum; i++) {
			WaveGeneratorSource *src = &gen->sources[i];
			if (gen->time == 0) src->samplesNum = 0;

			if (src->samplesNum > src->samplesMax-1) {
				src->samples = (float *)resizeArray(src->samples, sizeof(float), src->samplesMax, src->samplesMax*2 + 1);
				src->samplesMax = src->samplesMax*2 + 1;
			}
		}
	}

	for (int i = 0; i < gen->sourcesNum; i++) {
		WaveGeneratorSource *src = &gen->sources[i];
		float value = sin(src->phase + src->phaseShift)/2 + 0.5;
		value = tweenEase(value * src->power, src->ease);
		src->sample = value;
		src->phase += elapsed * src->rate;

		if (gen->recordSamples) src->samples[src->samplesNum++] = src->sample;

		gen->sample *= value;
		// gen->samples[gen->samplesNum] += value * (1.0/gen->sourcesNum);
	}

	if (gen->recordSamples) gen->samples[gen->samplesNum++] = gen->sample;

	gen->time += elapsed;
	return gen->sample;
}

float finalGetter(void *data, int idx);
float finalGetter(void *data, int idx) {
	WaveGenerator *gen = (WaveGenerator *)data;
	if (idx > gen->samplesNum-1) return 0;
	return gen->samples[idx];
};

float src0Getter(void *data, int idx);
float src0Getter(void *data, int idx) {
	WaveGenerator *gen = (WaveGenerator *)data;
	if (idx > gen->samplesNum-1) return 0;
	return gen->sources[0].samples[idx];
};

float src1Getter(void *data, int idx);
float src1Getter(void *data, int idx) {
	WaveGenerator *gen = (WaveGenerator *)data;
	if (idx > gen->samplesNum-1) return 0;
	return gen->sources[1].samples[idx];
};

