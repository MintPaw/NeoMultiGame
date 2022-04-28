void rndTest() {
	NanoTime startTime = getNanoTime();

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

	logf("Done, took %fms\n", getMsPassed(startTime));
}
