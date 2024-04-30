#ifdef LOGGING_HEADER
#undef LOGGING_HEADER

struct LogfBuffer {
	char *buffer;
	int size;
	float logTime;
	bool isInfo;
};

struct LogGroupBuffer {
	char *text;
	float logTime;
};

struct LogGroup {
#define LOG_GROUP_NAME_MAX_LEN 64
	char name[LOG_GROUP_NAME_MAX_LEN];
#define LOG_GROUP_BUFFERS_MAX 1024
	LogGroupBuffer buffers[LOG_GROUP_BUFFERS_MAX];
	int buffersNum;
};

#define PI_LOG_PATH "P:/logs/log.txt"

void initLoggingSystem();
void logf(const char *msg, ...);
LogfBuffer *loggerLogString(char *msg);
void loggerAssert(bool expr, const char *fileName, int lineNum);
void loggerPanic(const char *msg, const char *fileName, int lineNum);
void logfToFile(char *fileName, char *msg, ...);

char *getLogfBufferString();
void showLogfBufferErrorWindow();
void writeCrashLog();

LogGroup *getLogGroup(char *groupName);
void logTo(char *groupName, const char *msg, ...);
void guiDrawLogging();

#define Assert(expr) loggerAssert(expr, __FILE__, __LINE__)
#define Panic(msg) loggerPanic(msg, __FILE__, __LINE__)
/// FUNCTIONS ^

#else

struct LoggingSystem {
#define LOGF_BUFFERS_MAX 1024
	LogfBuffer logs[LOGF_BUFFERS_MAX];
	volatile u32 logfMutex; 
	DataStream *logStream;

	char *redirectPath;

#define LOG_GROUPS_MAX 32
	LogGroup groups[LOG_GROUPS_MAX];
	int groupsNum;
};

LoggingSystem *logSys = NULL;


void initLoggingSystem() {
	logSys = (LoggingSystem *)zalloc(sizeof(LoggingSystem));
	logSys->logStream = newDataStream();
}

// void infof(const char *msg, ...) {
// 	if (!logSys) initLoggingSystem();

// 	va_list args;
// 	va_start(args, msg);
// 	int size = stbsp_vsnprintf(NULL, 0, msg, args);
// 	va_end(args);

// 	char *str = frameMalloc(size+1);

// 	va_start(args, msg);
// 	stbsp_vsnprintf(str, size+1, msg, args);
// 	va_end(args);

// 	LogfBuffer *buffer = loggerLogString(str);
// 	buffer->isInfo = true;
// 	writeString(logSys->logStream, frameSprintf("[info] %.1f: %s", platform->time, str));
// }

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
		float time = 0;
		if (platform) time = platform->time;
		char *logStr = frameSprintf("[log] %.1f: %s", time, str);
		logStr[strlen(logStr)-1] = 0;
		writeString(logSys->logStream, logStr);
	}

	printf("%s", str);
	fflush(stdout);
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
	if (platform) log->logTime = platform->time;

	if (log->buffer[strlen(log->buffer)-1] == '\n') {
		log->buffer[strlen(log->buffer)-1] = 0;
	}

	if (logSys->redirectPath) logfToFile(logSys->redirectPath, msg);

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

	char *timeStr = epochToLocalTimeFrameString(time(NULL));
	str = frameSprintf("[%s] %s", timeStr, str);

	void appendFile(const char *fileName, void *data, int length); //@headerHack
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
	if (!writeFile(logPath, buf, strlen(buf))) {
		exit(0);
	}
}

LogGroup *getLogGroup(char *groupName) {
	for (int i = 0; i < logSys->groupsNum; i++) {
		LogGroup *group = &logSys->groups[i];
		if (streq(group->name, groupName)) {
			return group;
		}
	}

	if (logSys->groupsNum > LOG_GROUPS_MAX-1) {
		printf("Too many log groups! (They will leak)\n");
		logSys->groupsNum--;
	}

	LogGroup *group = &logSys->groups[logSys->groupsNum++];
	memset(group, 0, sizeof(LogGroup));
	strncpy(group->name, groupName, LOG_GROUP_NAME_MAX_LEN);
	return group;
}

void logTo(char *groupName, const char *msg, ...) {
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

	LogGroup *group = getLogGroup(groupName);
	if (group->buffersNum > LOG_GROUP_BUFFERS_MAX-1) {
		free(group->buffers[0].text);
		arraySpliceIndex(group->buffers, group->buffersNum, sizeof(LogGroupBuffer), 0);
		group->buffersNum--;
	}

	LogGroupBuffer *buffer = &group->buffers[group->buffersNum++];
	memset(buffer, 0, sizeof(LogGroupBuffer));
	buffer->logTime = platform->time;
	buffer->text = stringClone(str);

	DecMutex(&logSys->logfMutex);
}

void guiDrawLogging() {
	for (int i = 0; i < logSys->groupsNum; i++) {
		LogGroup *group = &logSys->groups[i];
		if (ImGui::TreeNode(frameSprintf("%s", group->name))) {

			ImGui::BeginChild(frameSprintf("%s logChild", group->name), ImVec2(platform->windowWidth*0.2, platform->windowHeight*0.2));
			for (int i = 0; i < group->buffersNum; i++) {
				LogGroupBuffer *buffer = &group->buffers[i];
				ImGui::Text("[%.2f] %s", buffer->logTime, buffer->text);
			}
			ImGui::EndChild();

			ImGui::TreePop();
		}
	}
}
#endif
