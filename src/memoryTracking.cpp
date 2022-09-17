void *allocateTrackedMemory(long size);
void freeTrackedMemory(void *mem);
void *reallocTrackedMemory(void *mem, long newSize);

void *allocateTrackedMemory(long size) {
	return (malloc)(size);
#if 0
	if (!memSys) initMemory();
	IncMutex(&memSys->_allocateFreeMutex);

	memSys->total += size;
	memSys->allTime += size;

	if (memSys->activeChunksNum >= memSys->activeChunksMax) {
		memSys->activeChunksMax++;
		memSys->activeChunksMax *= 2;
		memSys->activeChunks = (MemoryChunk **)(realloc)(memSys->activeChunks, sizeof(MemoryChunk *) * memSys->activeChunksMax);
	}

	MemoryChunk *chunk;
	if (memSys->emptyChunksNum > 0) {
		chunk = memSys->emptyChunks[memSys->emptyChunksNum-1];
		memSys->emptyChunksNum--;
	} else {
		chunk = (MemoryChunk *)(malloc)(sizeof(MemoryChunk));
	}

	memSys->activeChunks[memSys->activeChunksNum++] = chunk;

	chunk->id = memSys->currentChunkId++;
	// if (chunk->id == 100000) Panic("Break");
	chunk->size = size;

	chunk->data = (unsigned char *)(malloc)(size);

	DecMutex(&memSys->_allocateFreeMutex);
	return chunk->data;
#endif
}

void freeTrackedMemory(void *mem) {
	(free)(mem);
#if 0
	if (!memSys) initMemory();
	IncMutex(&memSys->_allocateFreeMutex);

	int chunkIndex = -1;
	for (int i = 0; i < memSys->activeChunksNum; i++) {
		if (memSys->activeChunks[i]->data == mem) {
			chunkIndex = i;
			break;
		}
	}

	// if (chunkIndex == -1) Panic("Couldn't find chunk to free!");

	(free)(mem);

	if (chunkIndex != -1) {
		MemoryChunk *chunk = memSys->activeChunks[chunkIndex];
		memSys->total -= chunk->size;
		if (chunkIndex != memSys->activeChunksNum) {
			arraySwap(memSys->activeChunks, memSys->activeChunksNum, sizeof(MemoryChunk *), chunkIndex, memSys->activeChunksNum-1);
		}
		memSys->activeChunksNum--;

		if (memSys->emptyChunksNum >= memSys->emptyChunksMax) {
			memSys->emptyChunksMax++;
			memSys->emptyChunksMax *= 2;
			memSys->emptyChunks = (MemoryChunk **)(realloc)(memSys->emptyChunks, sizeof(MemoryChunk *) * memSys->emptyChunksMax);
		}
		memSys->emptyChunks[memSys->emptyChunksNum++] = chunk;
	}

	DecMutex(&memSys->_allocateFreeMutex);
#endif
}

void *reallocTrackedMemory(void *mem, long newSize) {
	return (realloc)(mem, newSize);
#if 0
	MemoryChunk *oldChunk = NULL;
	for (int i = 0; i < memSys->activeChunksNum; i++) {
		MemoryChunk *chunk = memSys->activeChunks[i];
		if (chunk->data == mem) {
			oldChunk = chunk;
			break;
		}
	}

	if (!oldChunk) Panic("Old chunk not found");

	void *newMem = zalloc(newSize);
	memcpy(newMem, oldChunk->data, oldChunk->size);
	return newMem;
#endif
}

