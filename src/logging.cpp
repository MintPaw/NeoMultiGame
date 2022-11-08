#ifdef LOGGING_HEADER
#undef LOGGING_HEADER

struct LogfBuffer {
	char *buffer;
	int size;
	float logTime;
	bool isInfo;
};

LogfBuffer *loggerLogString(char *msg);
char *getLogfBufferString();
void showLogfBufferErrorWindow();
void writeCrashLog();
void logf(const char *msg, ...);
void loggerAssert(bool expr, const char *fileName, int lineNum);
void loggerPanic(const char *msg, const char *fileName, int lineNum);
bool logfToFile(const char *msg, ...);
#define Assert(expr) loggerAssert(expr, __FILE__, __LINE__)
#define Panic(msg) loggerPanic(msg, __FILE__, __LINE__)
/// FUNCTIONS ^


#else

struct LoggingSystem {
#define LOGF_BUFFERS_MAX 1024
	LogfBuffer logs[LOGF_BUFFERS_MAX];
	volatile u32 logfMutex; 
	DataStream *logStream;

	float time;
};

LoggingSystem *logSys = NULL;


void initLoggingSystem() {
	logSys = (LoggingSystem *)zalloc(sizeof(LoggingSystem));
	logSys->logStream = newDataStream();
}

void infof(const char *msg, ...) {
	if (!logSys) initLoggingSystem();

	va_list args;
	va_start(args, msg);
	int size = stbsp_vsnprintf(NULL, 0, msg, args);
	va_end(args);

	char *str = frameMalloc(size+1);

	va_start(args, msg);
	stbsp_vsnprintf(str, size+1, msg, args);
	va_end(args);

	LogfBuffer *buffer = loggerLogString(str);
	buffer->isInfo = true;
	writeString(logSys->logStream, frameSprintf("[info] %.1f: %s", logSys->time, str));
}

void logf(const char *msg, ...) {
	if (!logSys) initLoggingSystem();
	IncMutex(&logSys->logfMutex);

	va_list args;

	va_start(args, msg);
	int size = stbsp_vsnprintf(NULL, 0, msg, args);
	va_end(args);

	char *str = frameMalloc(size+1);

	va_start(args, msg);
	stbsp_vsnprintf(str, size+1, msg, args);
	va_end(args);

	{
		char *logStr = frameSprintf("[log] %.1f: %s", logSys->time, str);
		logStr[strlen(logStr)-1] = 0;
		writeString(logSys->logStream, logStr);
	}

	LogfBuffer *buffer = loggerLogString(str);

	DecMutex(&logSys->logfMutex);
}

LogfBuffer *loggerLogString(char *msg) {
	if (logSys->logs[0].buffer == NULL) {
		for (int i = 0; i < LOGF_BUFFERS_MAX; i++) {
			LogfBuffer *log = &logSys->logs[i];
			log->size = 256;
			log->buffer = (char *)malloc(sizeof(char) * log->size);
			log->buffer[0] = 0;
		}
	}

	for (int i = LOGF_BUFFERS_MAX-1; i >= 0; i--) {
		if (i == LOGF_BUFFERS_MAX-1) continue;
		ArraySwap(logSys->logs, i, i+1);
	}

	LogfBuffer *log = &logSys->logs[0];
	log->isInfo = false;

	int size = strlen(msg);
	if (size >= log->size) {
		free(log->buffer);
		log->size = size+1;
		log->buffer = (char *)malloc(sizeof(char) * log->size);
	}
	strncpy(log->buffer, msg, log->size);
	log->logTime = logSys->time;

#if defined(__EMSCRIPTEN__)
		printf("%s", msg);
#else
		// printf("\x1B[33m");
		printf("%s", msg);
		// printf("\x1B[37m");
#endif
	fflush(stdout);

	if (log->buffer[strlen(log->buffer)-1] == '\n') {
		log->buffer[strlen(log->buffer)-1] = 0;
	}

	return log;
}

void loggerAssert(bool expr, const char *fileName, int lineNum) {
	if (!expr) {
		loggerPanic("Assert failed", fileName, lineNum);
	}
}

void loggerPanic(const char *msg, const char *fileName, int lineNum) {
	logf("Panic: file %s line %d\n", fileName, lineNum);
	logf("%s\n", msg);
	fflush(NULL);

	writeCrashLog();

#if defined(_WIN32)
	fflush(NULL);
	__debugbreak();
#elif defined(__EMSCRIPTEN__)
	showLogfBufferErrorWindow();
	assert(0);
#else
	*(volatile char *)0 = 0;
#endif
}

void logfToFile(char *fileName, char *msg, ...) {
	if (!logSys) initLoggingSystem();
	IncMutex(&logSys->logfMutex);

	va_list args;

	va_start(args, msg);
	int size = stbsp_vsnprintf(NULL, 0, msg, args);
	va_end(args);

	char *str = frameMalloc(size+1);

	va_start(args, msg);
	stbsp_vsnprintf(str, size+1, msg, args);
	va_end(args);

	void appendFile(const char *fileName, void *data, int length); //@hack
	appendFile(fileName, str, strlen(str));

	DecMutex(&logSys->logfMutex);
}

char *getLogfBufferString() {
	int totalMessages = 0;
	int bufSize = 0;
	for (int i = 0; i < LOGF_BUFFERS_MAX; i++) {
		LogfBuffer *log = &logSys->logs[i];
		if (!log->buffer[0]) continue;
		totalMessages++;
		bufSize += strlen(log->buffer)+1;
	}

	char *buf = (char *)malloc(bufSize+1);
	buf[0] = 0;

	for (int i = 0; i < LOGF_BUFFERS_MAX; i++) {
		LogfBuffer *log = &logSys->logs[LOGF_BUFFERS_MAX-i-1];
		if (!log->buffer[0]) continue;
		strcat(buf, log->buffer);
		strcat(buf, "\n");
	}

	return buf;
}

void showLogfBufferErrorWindow() {
	char *buf = getLogfBufferString();
	void showErrorWindow(char *msg); //@hack
	showErrorWindow(buf);
}

void writeCrashLog() {
#if defined(__EMSCRIPTEN__)
	return;
#endif

	int bufSize = 0;
	for (int i = 0; i < LOGF_BUFFERS_MAX; i++) {
		LogfBuffer *log = &logSys->logs[i];
		bufSize += strlen(log->buffer)+1;
	}

	char *buf = (char *)malloc(bufSize+1);
	buf[0] = 0;

	for (int i = 0; i < LOGF_BUFFERS_MAX; i++) {
		LogfBuffer *log = &logSys->logs[LOGF_BUFFERS_MAX-i-1];
		if (!log->buffer[0]) continue;
		strcat(buf, log->buffer);
		strcat(buf, "\n");
	}

	char *logPath = frameSprintf("%s/crashlog.txt", exeDir);

	bool writeFile(const char *fileName, void *data, int length); //@headerHack
	writeFile(logPath, buf, strlen(buf));
}

#endif
