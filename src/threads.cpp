struct ThreadSafeQueue {
	volatile u32 mutex;

	volatile void *data;
	volatile int elementSize;
	volatile int elementMaxCount;

	volatile unsigned int currentIndex;
	volatile unsigned int pushIndex;
	volatile unsigned int shiftIndex;
	volatile unsigned int length;
};

struct Thread {
#if defined(_WIN32)
	HANDLE handle;
	DWORD id;
#else
	pthread_t posixThread;
#endif
	void *param;
	void (*callback)(void *);
};


ThreadSafeQueue *createThreadSafeQueue(int elementSize, int elementMaxCount, bool usingSemaphore=false);
bool threadSafeQueuePush(ThreadSafeQueue *queue, void *element);
bool threadSafeQueueShift(ThreadSafeQueue *queue, void *result);

Thread *createThread(void (*threadCallback)(void *), void *param=NULL);
void joinThread(Thread *thread);
void destroyThread(Thread *thread);
void exitThread();

#if USES_THREADS

#if defined(_WIN32)
DWORD WINAPI threadProc(void *threadStruct) {
	Thread *self = (Thread *)threadStruct;
	self->callback(threadStruct);
	return 0;
}
#else
void *threadProc(void *threadStruct) {
	Thread *self = (Thread *)threadStruct;
	self->callback(threadStruct);
	return 0;
}
#endif


Thread *createThread(void (*threadCallback)(void *), void *param) {
	Thread *thread = (Thread *)zalloc(sizeof(Thread));
	thread->callback = threadCallback;
	thread->param = param;
#if defined(_WIN32)
	thread->handle = CreateThread(0, 0, threadProc, thread, 0, &thread->id);
#else
	int res = pthread_create(&thread->posixThread, NULL, threadProc, thread);//non deffered
	if (res != 0) return NULL;
#endif
	return thread;
}

void destroyThread(Thread *thread) {
#if defined(_WIN32)
	TerminateThread(thread->handle, 0);
#else
	pthread_cancel(thread->posixThread);
#endif

	free(thread);
}

void exitThread() {
#if defined(_WIN32)
	ExitThread(0);
#else
	pthread_exit(0);
#endif
}

void joinThread(Thread *thread) {
#if defined(_WIN32)
	WaitForSingleObject(thread->handle, INFINITE);
#else
	pthread_join(thread->posixThread, NULL);
#endif

	free(thread);
}

ThreadSafeQueue *createThreadSafeQueue(int elementSize, int elementMaxCount, bool usingSemaphore) {
	ThreadSafeQueue *queue = (ThreadSafeQueue *)zalloc(sizeof(ThreadSafeQueue));
	queue->elementSize = elementSize;
	queue->elementMaxCount = elementMaxCount+1;
	queue->data = zalloc(queue->elementSize * queue->elementMaxCount);

	return queue;
}

bool threadSafeQueuePush(ThreadSafeQueue *queue, void *element) {
	if (!queue) {
		logf("No thread safe queue\n");
		return false;
	}

	IncMutex(&queue->mutex);

#if 1
	if (queue->length > queue->elementMaxCount-1) {
		if (queue->length > queue->elementMaxCount) logf("Bad push\n");
		DecMutex(&queue->mutex);
		return false;
	}

	void *dest = (char *)queue->data + queue->pushIndex*queue->elementSize;
	memcpy(dest, element, queue->elementSize);
	queue->length++;
	queue->pushIndex++;
	if (queue->pushIndex > queue->elementMaxCount-1) queue->pushIndex = 0;
#else
	if (queue->currentIndex > queue->elementMaxCount-1) {
		DecMutex(&queue->mutex);
		return false;
	}

	void *dest = (char *)queue->data + queue->currentIndex*queue->elementSize;
	memcpy(dest, element, queue->elementSize);
	queue->currentIndex++;
	queue->length = queue->currentIndex;
#endif

	DecMutex(&queue->mutex);
	return true;
}

bool threadSafeQueueShift(ThreadSafeQueue *queue, void *result) {
	if (!queue) {
		logf("No thread safe queue\n");
		return false;
	}

	IncMutex(&queue->mutex);

#if 1
	if (queue->length <= 0) {
		if (queue->length < 0) logf("Bad shift\n");
		DecMutex(&queue->mutex);
		return false;
	}

	void *src = (char *)queue->data + queue->shiftIndex*queue->elementSize;
	memcpy(result, src, queue->elementSize);
	queue->length--;
	queue->shiftIndex++;
	if (queue->shiftIndex > queue->elementMaxCount-1) queue->shiftIndex = 0;
#else
	if (queue->currentIndex <= 0) {
		DecMutex(&queue->mutex);
		return false;
	}

	memcpy(result, (void *)queue->data, queue->elementSize);
	arraySplice((void *)queue->data, queue->elementSize*queue->elementMaxCount, queue->elementSize, (void *)queue->data);
	queue->currentIndex--;

	queue->length = queue->currentIndex;
#endif

	DecMutex(&queue->mutex);
	return true;
}

void destroyThreadSafeQueue(ThreadSafeQueue *queue);
void destroyThreadSafeQueue(ThreadSafeQueue *queue) {
	free((void *)queue->data);
	free(queue);
}

#endif
