#define Gigabytes(x) (Megabytes(x)*1024ll)
#define Megabytes(x) (Kilobytes(x)*1024ll)
#define Kilobytes(x) ((x)*1024ll)

#define ArrayLength(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))
#define ConsumeBytes(dest, src, count) do {memcpy(dest, src, count); src += count;}while(0);

typedef int8_t   s8;
typedef int16_t  s16;
typedef int32_t  s32;
typedef int64_t  s64;
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

struct StringBuilder {
	char *string;
	int count;
	int maxLen;
};

void *zalloc(u32 size);
char *stringClone(const char *str);
bool streq(const char *str1, const char *str2, bool caseInsentitive=false);
bool strContains(const char *haystack, const char *needle, bool caseInsentitive=false);
char *strrstr(char *haystack, const char *needle);
int countChar(const char *src, char value);
bool stringStartsWith(const char *hayStack, const char *needle);
bool stringStartsWithFast(const char *hayStack, const char *needle);
bool stringEndsWith(char *hayStack, char *needle);
int toLowerCase(int letter);
int toUpperCase(int letter);
bool isNumber(char ch) { return (ch <= '9' && ch >= '0'); }
float getNumbersFromEndOfString(char *str);
void stripNumbersFromEndOfString(char *str);
int getIntAtEndOfString(char *str);

StringBuilder createStringBuilder(int startingMaxLen=128);
void addText(StringBuilder *builder, char *string, int count=-1);

#define ArraySwap(array, index1, index2) arraySwap((array), sizeof((array)), sizeof((array)[0]), index1, index2)
bool arraySwap(void *array, int arrayMaxElementsCount, int elementSize, int index1, int index2);
#define ArraySplice(array, element) arraySplice((array), sizeof((array)), sizeof((array)[0]), element);
bool arraySplice(void *array, int arraySize, int elementSize, void *element);
bool arraySpliceIndex(void *array, int arrayMaxLength, int elementSize, int index);

void *allocateMemory(long size, const char *fileName, int lineNum);
void freeMemory(void *mem);
void *reallocMemory(void *mem, long newSize);
void fastFree(void *mem);
void *fastMalloc(long size);

char *frameMalloc(int size);
char *frameSprintf(const char *msg, ...);
char *mallocSprintf(const char *msg, ...);
char *frameStringClone(const char *str);
void freeFrameMemory();

char *convertToHexString(void *data, int size);
void *convertFromHexString(char *hex, int *outputSize=NULL);

int indexOfU32(u32 *haystack, int needle);

struct MemoryChunk {
	unsigned char *data;
	int size;
	unsigned long id;
};

#define COMPRESS_FRAME_MEMORY
#define ALLOW_FAST_MEMORY_HACKS

// #define malloc(size) allocateMemory(size)
// #define free(ptr) freeMemory(ptr)
// #define realloc(ptr, newSize) reallocMemory(ptr, newSize)

int ALLOCATOR_DEFAULT = 1;
int ALLOCATOR_FRAME = 2;
struct Allocator {
	int type;
	void *data;
	u32 memoryAllocated;
};

struct MemorySystem {
	long lastFrameTotal;
	long allTime;
	long total;

	MemoryChunk **activeChunks;
	int activeChunksNum;
	int activeChunksMax;

	MemoryChunk **emptyChunks;
	int emptyChunksNum;
	int emptyChunksMax;

	unsigned long currentChunkId;

	void *frameMemory;
	int frameMemoryCurrentIndex;
	int frameMemoryMax;

	volatile u32 _allocateFreeMutex;
	volatile u32 _frameMemoryMutex;
};

int startingFrameMemory = Megabytes(1);
MemorySystem *memSys = NULL;

#define FRAME_CHUNKS_MAX (4096*100)
MemoryChunk frameChunks[FRAME_CHUNKS_MAX];
int frameChunksNum = 0;

void initMemory() {
	memSys = (MemorySystem *)(malloc)(sizeof(MemorySystem));
	memset(memSys, 0, sizeof(MemorySystem));

	memSys->activeChunksMax = 1;
	memSys->activeChunks = (MemoryChunk **)(malloc)(sizeof(MemoryChunk *) * memSys->activeChunksMax);

	memSys->emptyChunksMax = 1;
	memSys->emptyChunks = (MemoryChunk **)(malloc)(sizeof(MemoryChunk *) * memSys->emptyChunksMax);

	memSys->frameMemoryMax = startingFrameMemory;
	memSys->frameMemory = (malloc)(memSys->frameMemoryMax);
}

void *allocateFrom(Allocator *allocator, int size) {
	allocator->memoryAllocated += size;
	if (allocator->type == ALLOCATOR_DEFAULT) {
		return malloc(size);
	} else if (allocator->type == ALLOCATOR_FRAME) {
		return frameMalloc(size);
	}

	return NULL;
}

void freeFrom(Allocator *allocator, void *data) {
	if (allocator->type == ALLOCATOR_DEFAULT) {
		return free(data);
	} else if (allocator->type == ALLOCATOR_FRAME) {
		return;
	}
}

#ifndef IncMutex
# define IncMutex
# define DecMutex
#endif

void *allocateMemory(long size) {
	if (!memSys) initMemory();
	IncMutex(&memSys->_allocateFreeMutex);

	memSys->total += size;
	memSys->allTime += size;

	if (memSys->activeChunksNum >= memSys->activeChunksMax) {
		memSys->activeChunksMax++;
		memSys->activeChunksMax *= 2;
		memSys->activeChunks = (MemoryChunk **)(realloc)(memSys->activeChunks, sizeof(MemoryChunk *) * memSys->activeChunksMax);
	}

#if 1
	MemoryChunk *chunk;
	if (memSys->emptyChunksNum > 0) {
		chunk = memSys->emptyChunks[memSys->emptyChunksNum-1];
		memSys->emptyChunksNum--;
	} else {
		chunk = (MemoryChunk *)(malloc)(sizeof(MemoryChunk));
	}
#else
	MemoryChunk *chunk = (MemoryChunk *)(malloc)(sizeof(MemoryChunk));
#endif

	memSys->activeChunks[memSys->activeChunksNum++] = chunk;

	chunk->id = memSys->currentChunkId++;
	// if (chunk->id == 100000) Panic("Break");
	chunk->size = size;

	chunk->data = (unsigned char *)(malloc)(size);

	DecMutex(&memSys->_allocateFreeMutex);
	return chunk->data;
}

void *fastMalloc(long size) {
#if defined(ALLOW_FAST_MEMORY_HACKS)
	return (malloc)(size);
#else
	return malloc(size);
#endif
}

void freeMemory(void *mem) {
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

#if 1
		if (memSys->emptyChunksNum >= memSys->emptyChunksMax) {
			memSys->emptyChunksMax++;
			memSys->emptyChunksMax *= 2;
			memSys->emptyChunks = (MemoryChunk **)(realloc)(memSys->emptyChunks, sizeof(MemoryChunk *) * memSys->emptyChunksMax);
		}
		memSys->emptyChunks[memSys->emptyChunksNum++] = chunk;
#else
		(free)(chunk);
#endif
	}

	DecMutex(&memSys->_allocateFreeMutex);
}

void fastFree(void *mem) {
#if defined(ALLOW_FAST_MEMORY_HACKS)
	(free)(mem);
#else
	free(mem);
#endif
}

void *reallocMemory(void *mem, long newSize) {
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
}

char *frameMalloc(int size) {
	if (!memSys) initMemory();
	IncMutex(&memSys->_frameMemoryMutex);

	while (size % 8 != 0) size++;

	bool shouldUseChunk = true;

#if defined(COMPRESS_FRAME_MEMORY)
	if (memSys->frameMemoryCurrentIndex+size <= memSys->frameMemoryMax) shouldUseChunk = false;
#endif

	if (shouldUseChunk) {
		if (frameChunksNum >= FRAME_CHUNKS_MAX) {
			printf("No more frame memory\n");

			DecMutex(&memSys->_frameMemoryMutex);
			return NULL;
		}

		MemoryChunk *chunk = &frameChunks[frameChunksNum++];
		chunk->size = size;

		chunk->data = (unsigned char *)zalloc(chunk->size);

		DecMutex(&memSys->_frameMemoryMutex);
		return (char *)chunk->data;
	} else {
		void *nextMem = ((unsigned char *)memSys->frameMemory) + memSys->frameMemoryCurrentIndex;
		memset(nextMem, 0, sizeof(char) * size);
		memSys->frameMemoryCurrentIndex += size;

		DecMutex(&memSys->_frameMemoryMutex);
		return (char *)nextMem;
	}
}

char *mallocSprintf(const char *msg, ...) {
	va_list args;
	va_start(args, msg);
	int size = stbsp_vsnprintf(NULL, 0, msg, args);
	va_end(args);
	char *str = (char *)malloc(size+1);

	va_start(args, msg);
	stbsp_vsnprintf(str, size+1, msg, args);
	va_end(args);
	return str;
}

char *frameSprintf(const char *msg, ...) {
	va_list args;
	va_start(args, msg);
	int size = stbsp_vsnprintf(NULL, 0, msg, args);
	va_end(args);
	char *str = frameMalloc(size+1);

	va_start(args, msg);
	stbsp_vsnprintf(str, size+1, msg, args);
	va_end(args);
	return str;
}

char *frameStringClone(const char *str) {
	char *ret = (char *)frameMalloc((strlen(str)+1));
	strcpy(ret, str);
	return ret;
}

void freeFrameMemory() {
	if (!memSys) return;

	IncMutex(&memSys->_frameMemoryMutex);

	memSys->frameMemoryCurrentIndex = 0;

	int extraMemoryNeeded = 0;
	for (int i = 0; i < frameChunksNum; i++) {
		extraMemoryNeeded += frameChunks[i].size;
		free(frameChunks[i].data);
	}
	frameChunksNum = 0;

#if defined(COMPRESS_FRAME_MEMORY)
	if (extraMemoryNeeded > 0) {
		memSys->frameMemoryMax += extraMemoryNeeded;
		if (memSys->frameMemoryMax > startingFrameMemory * 2) {
			printf("Overflowed frame mem *2 (%.1fmb now)\n", (float)memSys->frameMemoryMax/Megabytes(1));
			// memSys->frameMemoryMax = startingFrameMemory * 2;
		}
		free(memSys->frameMemory);
		memSys->frameMemory = malloc(memSys->frameMemoryMax);
	}
#endif

	memSys->lastFrameTotal = memSys->total;

	DecMutex(&memSys->_frameMemoryMutex);
}

void *zalloc(u32 size) {
	void *mem = malloc(size);
	memset(mem, 0, size);
	return mem;
}

char *stringClone(const char *str) {
	char *ret = (char *)malloc((strlen(str)+1));
	strcpy(ret, str);
	return ret;
}

void *memClone(void *ptr, u32 size);
void *memClone(void *ptr, u32 size) {
	void *ret = malloc(size);
	memcpy(ret, ptr, size);
	return ret;
}

bool streq(const char *str1, const char *str2, bool caseInsentitive) {
	if (!str1 || !str2) return false;

	for (int i = 0; true; i++) {
		if (str1[i] == 0 && str2[i] == 0) return true;

		char char1 = str1[i];
		char char2 = str2[i];

		if (caseInsentitive) {
			if (char1 >= 97) char1 -= 32;
			if (char2 >= 97) char2 -= 32;
		}

		if (char1 != char2) return false;
	}
}

bool strContains(const char *haystack, const char *needle, bool caseInsentitive) {
	if (!caseInsentitive) return strstr(haystack, needle) != NULL;
#if 0
#else
	if (!haystack || !needle) return false;

	int matchingOn = 0;
	for (int i = 0; true; i++) {
		if (haystack[i] == 0) return false;

		char hayChar = haystack[i];
		char needleChar = needle[matchingOn];
		if (hayChar >= 97 && hayChar <= 122) hayChar -= 32;
		if (needleChar >= 97 && needleChar <= 122) needleChar -= 32;
		if (hayChar == needleChar) {
			matchingOn++;
			if (needle[matchingOn] == 0) return true;
		} else {
			if (matchingOn != 0) {
				matchingOn = 0;
				i--;
				continue;
			}
		}
	}

	return false;
#endif
}

char *strrstr(char *haystack, const char *needle) {
	if (*needle == '\0')
		return (char *) haystack;

	char *result = NULL;
	for (;;) {
		char *p = strstr(haystack, needle);
		if (p == NULL) break;

		result = p;
		haystack = p + 1;
	}

	return result;
}

int countChar(const char *src, char value) {
	int total = 0;

	for (int i = 0; ; i++) {
		if (src[i] == value) total++;
		if (src[i] == 0) break;
	}

	return total;
}

bool stringStartsWith(const char *hayStack, const char *needle) {
	if (strlen(needle) > strlen(hayStack)) return false;

	int len = strlen(needle);
	bool areSame = true;
	for (int i = 0; i < len; i++)
		if (needle[i] != hayStack[i])
			areSame = false;

	return areSame;
}

bool stringStartsWithFast(const char *hayStack, const char *needle) {
	int len = strlen(needle);
	bool areSame = true;
	for (int i = 0; i < len; i++)
		if (needle[i] != hayStack[i])
			areSame = false;

	return areSame;
}

bool stringEndsWith(char *hayStack, char *needle) {
	char *shouldBeAt = hayStack + strlen(hayStack) - strlen(needle);
	return streq(shouldBeAt, needle);
}

int toLowerCase(int letter) {
	if (letter >= 'A' && letter <= 'Z') return letter + ('a'-'A');
	return letter;
}

int toUpperCase(int letter) {
	if (letter >= 'a' && letter <= 'z') return letter - ('a'-'A');
	return letter;
}

float getNumbersFromEndOfString(char *str) {
	int len = strlen(str);

	int charsToCut = 0;
	for (int i = len-1; i >= 0; i--) {
		if (isNumber(str[i]) || str[i] == '.' || str[i] == '-') {
			charsToCut++;
		} else {
			break;
		}
	}

	if (charsToCut == 0) return 0;

	char *numString = &str[len - charsToCut];
	return atof(numString);
}

void stripNumbersFromEndOfString(char *str) {
	for (;;) {
		if (isNumber(str[strlen(str)-1])) {
			str[strlen(str)-1] = 0;
		} else {
			break;
		}
	}
}

int getIntAtEndOfString(char *str) {
	int numberOfNumbers = 0;
	int strLen = strlen(str);
	for (int i = strLen-1; i >= 0; i--) {
		if (isNumber(str[i])) {
			numberOfNumbers++;
		} else {
			break;
		}
	}

	char *strNumber = &str[strlen(str)-numberOfNumbers];
	return atoi(strNumber);
}


StringBuilder createStringBuilder(int startingMaxLen) {
	StringBuilder builder = {}; //createStringBuilder
	builder.maxLen = startingMaxLen;
	builder.string = (char *)zalloc(builder.maxLen);
	return builder;
}

void addText(StringBuilder *builder, char *string, int count) {
	if (count == -1) count = strlen(string);

	int charsLeft = builder->maxLen - builder->count - 1;

	int extraNeeded = count - charsLeft;
	if (extraNeeded > 0) {
		char *newString = (char *)zalloc(builder->maxLen + extraNeeded);
		builder->maxLen += extraNeeded;

		strncpy(newString, builder->string, builder->count);
		free(builder->string);
		builder->string = newString;
	}

	strncat(builder->string, string, count);
	builder->count += count;
}

unsigned char *elementBuffer = NULL;
int elementBufferSize = -1;

bool arraySwap(void *array, int arrayMaxElementsCount, int elementSize, int index1, int index2) {
	if (elementSize > elementBufferSize) {
		if (elementBuffer) (free)(elementBuffer);
		elementBuffer = (unsigned char *)(malloc)(elementSize+1);
		elementBufferSize = elementSize;
	}

	if (arrayMaxElementsCount <= 1) logf("Max array size is only 1 element, you probably made a mistake!\n");
	if (
		index1 > arrayMaxElementsCount-1 ||
		index2 > arrayMaxElementsCount-1 ||
		index1 < 0 ||
		index2 < 0
	) return false;

	unsigned char *ptr1 = (unsigned char *)array + elementSize*index1;
	unsigned char *ptr2 = (unsigned char *)array + elementSize*index2;

	memcpy(elementBuffer, ptr1, elementSize);
	memcpy(ptr1, ptr2, elementSize);
	memcpy(ptr2, elementBuffer, elementSize);

	return true;
}

bool arraySpliceIndex(void *array, int arrayMaxLength, int elementSize, int index) {
	void *dest = (unsigned char *)array + elementSize*index;
	void *src = (unsigned char *)array + elementSize*(index+1);
	int size = (arrayMaxLength - index - 1) * elementSize;

	memmove(dest, src, size);

	return true;
}

bool arraySplice(void *array, int arraySize, int elementSize, void *element) {
	if (arraySize <= 8) Panic("This is probably a pointer, not an array\n");

	int elementIndex = ((unsigned char *)element - (unsigned char *)array) / elementSize;
	if (elementIndex < 0) {
		logf("Can't splice element that isn't in array!\n");
		return false;
	}

	void *dest = element;
	void *src = (unsigned char *)element + elementSize;
	int size = (arraySize - ((elementIndex+1)*elementSize));

	memmove(dest, src, size);

	return true;
}

void arraySpread(void *array, int arrayNum, int elementSize, int afterIndex, int spreadAmount) {
	void *dest = ((char *)array) + (afterIndex + spreadAmount)*elementSize;
	void *src = ((char *)array) + (afterIndex)*elementSize;
	int size = (arrayNum - afterIndex)*elementSize;

	if (size < 0) {
		Panic("Trying to spread an array negatively");
	}

	memmove(dest, src, size);
}

char *convertToHexString(void *data, int size) {
	int hexSize = size*2 + 256;
	char *result = (char *)zalloc(hexSize);
	unsigned char *pin = (unsigned char *)(data);
	const char *hex = "0123456789ABCDEF";
	char *pout = result + strlen(result);
	for(int i = 0; i < size-1; i++){
		*pout++ = hex[(*pin>>4)&0xF];
		*pout++ = hex[(*pin++)&0xF];
	}
	*pout++ = hex[(*pin>>4)&0xF];
	*pout++ = hex[(*pin)&0xF];
	*pout = 0;

	return result;
}

unsigned char hexCharToByte(unsigned char hex);
unsigned char hexCharToByte(unsigned char hex) {
	if (hex >= '0' && hex <= '9') {
		return hex - '0';
	} else if (hex >= 'A' && hex <= 'F') {
		return hex - 'A' + 10;
	} else if (hex >= 'a' && hex <= 'f') {
		return hex - 'a' + 10;
	} else {
		return 0;
	}
}

void *convertFromHexString(char *hex, int *outputSize) {
	int hexLen = strlen(hex);

	u8 *output = (u8 *)malloc(hexLen/2);

	for (int i = 0; i < hexLen/2; i++) {
		output[i] = (hexCharToByte(hex[i*2]) << 4) | hexCharToByte(hex[i*2 + 1]);
	}

	if (outputSize) *outputSize = hexLen/2;

	return output;
}


u32 stringHash32(const char *s);
u32 stringHash32(const char *s) {
	u32 hash = 0;

	for(; *s; ++s) {
		hash += *s;
		hash += (hash << 10);
		hash ^= (hash >> 6);
	}

	hash += (hash << 3);
	hash ^= (hash >> 11);
	hash += (hash << 15);

	return hash;
}

u32 hashU32(u32 x);
u32 hashU32(u32 x) {
	x = ((x >> 16) ^ x) * 0x45d9f3b;
	x = ((x >> 16) ^ x) * 0x45d9f3b;
	x = (x >> 16) ^ x;
	return x;
}

void printBinary(u32 n);
void printBinary(u32 n) {
	u32 i;
	for (i = 1 << 31; i > 0; i = i / 2) {
		(n & i) ? printf("1") : printf("0");
	}
}

void dumpHex(const void* data, size_t size);
void dumpHex(const void* data, size_t size) {
	static char line[128] = {};
	memset(line, 0, 128);

	char ascii[17];
	size_t i, j;
	ascii[16] = '\0';
	for (i = 0; i < size; ++i) {
		sprintf(line+strlen(line), "%02X ", ((unsigned char*)data)[i]);
		if (((unsigned char*)data)[i] >= ' ' && ((unsigned char*)data)[i] <= '~') {
			ascii[i % 16] = ((unsigned char*)data)[i];
		} else {
			ascii[i % 16] = '.';
		}
		if ((i+1) % 8 == 0 || i+1 == size) {
			sprintf(line+strlen(line), " ");
			if ((i+1) % 16 == 0) {
				sprintf(line+strlen(line), "|  %s \n", ascii);
				logf("%s", line);
				memset(line, 0, 128);
			} else if (i+1 == size) {
				ascii[(i+1) % 16] = '\0';
				if ((i+1) % 16 <= 8) {
					sprintf(line+strlen(line), " ");
				}
				for (j = (i+1) % 16; j < 16; ++j) {
					sprintf(line+strlen(line), "   ");
				}
				sprintf(line+strlen(line), "|  %s \n", ascii);
				logf("%s", line);
				memset(line, 0, 128);
			}
		}
	}
}

int bitExtract(int number, int start, int count) { 
	return (((1 << count) - 1) & (number >> (start - 1))); 
} 

u32 bitMask(u32 a, u32 b) {
	return ((1ull << (b - a)) - 1ull) << a;
}

u8 reverseBits(u8 b) {
	b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
	b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
	b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
	return b;
}

u32 reverseBits(u32 number, u32 bits) {
	u32 v = number;
	u32 r = bits;
	int s = bits - 1; // extra shift needed at end

	for (v >>= 1; v; v >>= 1) {   
		r <<= 1;
		r |= v & 1;
		s--;
	}
	r <<= s; // shift when v's highest bits are zero

	return r;
}

void *resizeArray(void *array, u64 elementSize, u64 currentCount, u64 newCount);
void *resizeArray(void *array, u64 elementSize, u64 currentCount, u64 newCount) {
	if (newCount == 0) {
		logf("Called resizeArray with a newCount of 0\n");
		return NULL;
	}

	u64 currentSize = elementSize * currentCount;
	u64 newSize = elementSize * newCount;

	u64 copySize = currentSize < newSize ? currentSize : newSize;

	void *newArray = zalloc(newSize);

	if (currentCount != 0) {
		memcpy(newArray, array, copySize);
		free(array);
	}

	return newArray;
}

void flipBitmapData(u8 *bitmapData, int width, int height);
void flipBitmapData(u8 *bitmapData, int width, int height) {
	u8 *tempRow = (u8 *)frameMalloc(width * 4);
	for (int y = 0; y < height/2; y++) {
		int curTopRow = y;
		int curBottomRow = height - y - 1;
		u8 *topRowStart = (u8 *)bitmapData + curTopRow * width * 4;
		u8 *bottomRowStart = (u8 *)bitmapData + curBottomRow * width * 4;

		memcpy(tempRow, topRowStart, width * 4);
		memcpy(topRowStart, bottomRowStart, width * 4);
		memcpy(bottomRowStart, tempRow, width * 4);
	}
}

int indexOfU32(u32 *haystack, int haystackNum, u32 needle) {
	for (int i = 0; i < haystackNum; i++) {
		if (haystack[i] == needle) return i;
	}

	return -1;
}

/// Dynamic array

template <typename T>
struct Array {
	T *elements;
	int count;
	int max;

	T *operator [](int i) {
		return &this->elements[i];
	}

	T *get(int index) {
		return &this->elements[index];
	}

};

template <typename T>
void resizeArray(Array<T> *array, int newMax) {
	array->elements = (T *)resizeArray(array->elements, sizeof(T), array->count, newMax);
	array->max = newMax;
	if (array->count > array->max-1) array->count = array->max-1;
}

template <typename T>
T *getNewElement(Array<T> *array) {
	if (array->count > array->max-1) {
		resizeArray(array, (array->max*1.5)+1);
	}

	T *element = array->get(array->count);
	array->count++;
	return element;
}

/// /Dynamic array
