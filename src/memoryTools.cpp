#define HAS_GAME_LIB_MEMORY_TOOLS
#define Gigabytes(x) (Megabytes(x)*1024ll)
#define Megabytes(x) (Kilobytes(x)*1024ll)
#define Kilobytes(x) ((x)*1024ll)

#define ArrayLength(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))
#define ConsumeBytes(dest, src, count) do {memcpy(dest, src, count); src += count;}while(0);

#define IS_BIT_SET(n,x)   (((n & (1 << x)) != 0) ? 1 : 0)

#ifndef IncMutex
# define IncMutex
# define DecMutex
#endif

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

struct MemoryArena {
	u8 **blocks;
	int blocksNum;
	int blocksMax;

	int blockPosition;
	int blockSize;
};

struct TextTag {
#define TEXT_TAG_NAME_MAX_LEN 64
	char name[TEXT_TAG_NAME_MAX_LEN];
	int startIndex;
	int endIndex;
};
struct TaggedText {
	char *text;
#define TAGGED_TEXT_TAGS_MAX 128
	TextTag tags[TAGGED_TEXT_TAGS_MAX];
	int tagsNum;
};

void *zalloc(u32 size);
char *stringClone(const char *str);
void *memClone(void *ptr, u32 size);
bool streq(const char *str1, const char *str2, bool caseInsentitive=false);
bool strContains(const char *haystack, const char *needle, bool caseInsentitive=false);
char *strrstr(char *haystack, const char *needle);
int countChar(const char *src, char value, int len=0);
int countString(char *src, char *value);
bool stringStartsWith(const char *hayStack, const char *needle);
bool stringEndsWith(char *hayStack, char *needle);
void toLowerCase(char *str);
int toLowerCase(int letter);
int toUpperCase(int letter);
bool isNumber(char ch) { return (ch <= '9' && ch >= '0'); }
float getNumbersFromEndOfString(char *str);
void stripNumbersFromEndOfString(char *str);
int getIntAtEndOfString(char *str);

StringBuilder createStringBuilder(int startingMaxLen=128);
void addText(StringBuilder *builder, char *string, int count=-1);
void destroy(StringBuilder builder);

char **frameSplitString(char *str, char *delim, int *outStringsNum);
char **mallocSplitString(char *str, char *delim, int *outStringsNum);

char *getStringReplaced(char *string, char *from, char *to);

#define ArraySwap(array, index1, index2) arraySwap((array), sizeof((array)), sizeof((array)[0]), index1, index2)
bool arraySwap(void *array, int arrayMaxElementsCount, int elementSize, int index1, int index2);
// #define ArraySplice(array, element) arraySplice((array), sizeof((array)), sizeof((array)[0]), element);
// #define ArraySpliceIndex(array, arrayMaxLength, index) arraySpliceIndex((array), (arrayMaxLength), sizeof((array)[0]), (index));
// bool arraySplice(void *array, int arraySize, int elementSize, void *element);
bool arraySpliceIndex(void *array, int arrayMaxLength, int elementSize, int index);

char *frameMalloc(int size);
char *frameSprintf(const char *msg, ...);
char *mallocSprintf(const char *msg, ...);
char *frameStringClone(const char *str);
void freeFrameMemory();

char *convertToHexString(void *data, int size);
unsigned char hexCharToByte(unsigned char hex);
void *convertFromHexString(char *hex, int *outputSize=NULL);
u32 stringHash32(const char *s);
u32 hashU32(u32 x);
void printBinary(u32 n);
void dumpHex(const void* data, size_t size);

int bitExtract(int number, int start, int count);
u32 bitMask(u32 a, u32 b);
u8 reverseBits(u8 b);
u32 reverseBits(u32 number, u32 bits);

void *resizeArray(void *array, u64 elementSize, u64 currentCount, u64 newCount);
void *frameResizeArray(void *array, u64 elementSize, u64 currentCount, u64 newCount);

void flipBitmapData(u8 *bitmapData, int width, int height);

int indexOfU32(u32 *haystack, int needle);

char *epochToLocalTimeFrameString(u64 time);

MemoryArena *createMemoryArena(int blockSize=Kilobytes(16), int blocksMax=8192);
void *allocateMemory(MemoryArena *arena, int size);
void destroyMemoryArena(MemoryArena *arena);

TaggedText *parseTaggedText(char *inText);
bool inTextTag(TaggedText *taggedText, char *tagName, int index);

struct MemoryChunk {
	u8 *data;
	int size;
	u32 id;
};

#define COMPRESS_FRAME_MEMORY

void *allocateTrackedMemory(long size);
void freeTrackedMemory(void *mem);
void *reallocTrackedMemory(void *mem, long newSize);
// #define malloc(size) allocateTrackedMemory(size)
// #define free(ptr) freeTrackedMemory(ptr)
// #define realloc(ptr, newSize) reallocTrackedMemory(ptr, newSize)

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

#define FRAME_CHUNKS_MAX (4096*100)
	MemoryChunk frameChunks[FRAME_CHUNKS_MAX];
	int frameChunksNum = 0;

	void *frameMemory;
	int frameMemoryCurrentIndex;
	int frameMemoryMax;

	volatile u32 _allocateFreeMutex;
	volatile u32 _frameMemoryMutex;
};

int startingFrameMemory = Megabytes(1);
MemorySystem *memSys = NULL;

void initMemory() {
	memSys = (MemorySystem *)malloc(sizeof(MemorySystem));
	memset(memSys, 0, sizeof(MemorySystem));

	memSys->frameMemoryMax = startingFrameMemory;
	memSys->frameMemory = malloc(memSys->frameMemoryMax);
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

char *frameMalloc(int size) {
	if (!memSys) initMemory();
	IncMutex(&memSys->_frameMemoryMutex);

	while (size % 8 != 0) size++;

	bool shouldUseChunk = true;

#if defined(COMPRESS_FRAME_MEMORY)
	if (memSys->frameMemoryCurrentIndex+size <= memSys->frameMemoryMax) shouldUseChunk = false;
#endif

	if (shouldUseChunk) {
		if (memSys->frameChunksNum >= FRAME_CHUNKS_MAX) {
			printf("No more frame memory, will leak\n");

			DecMutex(&memSys->_frameMemoryMutex);
			return (char *)zalloc(size);
		}

		MemoryChunk *chunk = &memSys->frameChunks[memSys->frameChunksNum++];
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

	if (memSys->frameChunksNum > 30) printf("Overflowed frame %d extra allocation\n", memSys->frameChunksNum);

	int extraMemoryNeeded = 0;
	for (int i = 0; i < memSys->frameChunksNum; i++) {
		extraMemoryNeeded += memSys->frameChunks[i].size;
		free(memSys->frameChunks[i].data);
	}
	memSys->frameChunksNum = 0;

#if defined(COMPRESS_FRAME_MEMORY)
	if (extraMemoryNeeded > 0) {
		memSys->frameMemoryMax += extraMemoryNeeded;
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

int countChar(const char *src, char value, int len) {
	int total = 0;

	int loopCount = 0;
	for (int i = 0; ; i++) {
		loopCount++;
		if (len && loopCount == len) break;
		if (src[i] == 0) break;
		if (src[i] == value) total++;
	}

	return total;
}

int countString(char *src, char *value) {
	int count = 0;
	const char *tmp = src;
	while ((tmp = strstr(tmp, value))) {
		count++;
		tmp++;
	}
	return count;
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

bool stringEndsWith(char *hayStack, char *needle) {
	char *shouldBeAt = hayStack + strlen(hayStack) - strlen(needle);
	return streq(shouldBeAt, needle);
}

void toLowerCase(char *str) {
	int len = strlen(str);
	for (int i = 0; i < len; i++) {
		str[i] = toLowerCase(str[i]);
	}
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
	StringBuilder builder = {};
	builder.maxLen = startingMaxLen;
	builder.string = (char *)zalloc(builder.maxLen);
	return builder;
}

void addText(StringBuilder *builder, char *string, int count) {
	if (count == -1) count = strlen(string);

	for (;;) {
		int charsLeft = builder->maxLen - builder->count - 1;
		int extraNeeded = count - charsLeft;
		if (extraNeeded <= 0) break;

		char *newString = (char *)zalloc((builder->maxLen+1) * 2);
		builder->maxLen = (builder->maxLen+1) * 2;

		strncpy(newString, builder->string, builder->count);
		free(builder->string);
		builder->string = newString;
	}

	strncpy(&builder->string[builder->count], string, count);
	// strncat(builder->string, string, count);
	builder->count += count;
	builder->string[builder->count] = 0;
}

void destroy(StringBuilder builder) {
	free(builder.string);
}

char **frameSplitString(char *str, char *delim, int *outStringsNum) {
	int outMax = countString(str, delim)+1;
	char **out = (char **)frameMalloc(sizeof(char *) * outMax);
	int outNum = 0;

	int delimLen = strlen(delim);

	char *lineStart = str;
	for (;;) {
		char *lineEnd = strstr(lineStart, delim);

		bool shouldBreak = false;
		if (!lineEnd) {
			lineEnd = &lineStart[strlen(lineStart)];
			shouldBreak = true;
		}

		int lineLen = lineEnd - lineStart;
		if (lineLen != 0) {
			char *line = (char *)frameMalloc(lineLen+1);
			line = strncpy(line, lineStart, lineLen);
			out[outNum++] = line;
		}

		if (shouldBreak) break;
		lineStart = lineEnd+delimLen;
	}

	*outStringsNum = outNum;
	return out;
}

char **mallocSplitString(char *str, char *delim, int *outStringsNum) {
	int frameStringsNum = 0;
	char **frameStrings = frameSplitString(str, delim, &frameStringsNum);
	char **mallocStrings = (char **)malloc(sizeof(char *) * frameStringsNum);
	for (int i = 0; i < frameStringsNum; i++) mallocStrings[i] = stringClone(frameStrings[i]);

	*outStringsNum = frameStringsNum;
	return mallocStrings;
}

char *getStringReplaced(char *orig, char *rep, char *with) {
	if (!orig || !rep) return NULL;

	int len_rep = strlen(rep);
	if (len_rep == 0) return NULL;

	if (!with) with = "";
	int len_with = strlen(with);

	// count the number of replacements needed
	char *ins = orig;
	int count = 0;
	char *tmp;
	for (count = 0; (tmp = strstr(ins, rep)); ++count) {
		ins = tmp + len_rep;
	}

	char *result = (char *)malloc(strlen(orig) + (len_with - len_rep) * count + 1);
	tmp = result;

	// first time through the loop, all the variable are set correctly
	// from here on,
	//    tmp points to the end of the result string
	//    ins points to the next occurrence of rep in orig
	//    orig points to the remainder of orig after "end of rep"
	while (count--) {
		ins = strstr(orig, rep);
		int len_front = ins - orig;
		tmp = strncpy(tmp, orig, len_front) + len_front;
		tmp = strcpy(tmp, with) + len_with;
		orig += len_front + len_rep; // move to next "end of rep"
	}
	strcpy(tmp, orig);
	return result;
}

unsigned char *elementBuffer = NULL;
int elementBufferSize = -1;

bool arraySwap(void *array, int arrayMaxElementsCount, int elementSize, int index1, int index2) {
	if (elementSize > elementBufferSize) {
		if (elementBuffer) free(elementBuffer);
		elementBuffer = (unsigned char *)malloc(elementSize+1);
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

// bool arraySplice(void *array, int arraySize, int elementSize, void *element) {
// 	if (arraySize <= 8) Panic("This is probably a pointer, not an array\n");

// 	int elementIndex = ((unsigned char *)element - (unsigned char *)array) / elementSize;
// 	if (elementIndex < 0) {
// 		logf("Can't splice element that isn't in array!\n");
// 		return false;
// 	}

// 	void *dest = element;
// 	void *src = (unsigned char *)element + elementSize;
// 	int size = (arraySize - ((elementIndex+1)*elementSize));

// 	memmove(dest, src, size);

// 	return true;
// }

// Maybe remove this, what if the array is too small?
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

u32 hashU32(u32 x) {
	x = ((x >> 16) ^ x) * 0x45d9f3b;
	x = ((x >> 16) ^ x) * 0x45d9f3b;
	x = (x >> 16) ^ x;
	return x;
}

void printBinary(u32 n) {
	u32 i;
	for (i = 1 << 31; i > 0; i = i / 2) {
		(n & i) ? printf("1") : printf("0");
	}
}

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

void *frameResizeArray(void *array, u64 elementSize, u64 currentCount, u64 newCount) {
	if (newCount == 0) {
		logf("Called frameResizeArray with a newCount of 0\n");
		return NULL;
	}

	u64 currentSize = elementSize * currentCount;
	u64 newSize = elementSize * newCount;

	u64 copySize = currentSize < newSize ? currentSize : newSize;

	void *newArray = frameMalloc(newSize);

	if (currentCount != 0) {
		memcpy(newArray, array, copySize);
	}

	return newArray;
}

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

char *epochToLocalTimeFrameString(u64 time) {
	time_t timeCopy = time;
	char *buffer = (char *)frameMalloc(1024);
	tm *timeInfoPtr = localtime(&timeCopy);
	if (!timeInfoPtr) return NULL;

	tm timeInfo = *timeInfoPtr;
	strftime(buffer, 1024, "%m/%d/%y %H:%M:%S", &timeInfo);
	return buffer;
}

MemoryArena *createMemoryArena(int blockSize, int blocksMax) {
	MemoryArena *arena = (MemoryArena *)zalloc(sizeof(MemoryArena));
	arena->blockSize = blockSize;
	arena->blocksMax = blocksMax;
	arena->blocks = (u8 **)zalloc(sizeof(u8 *) * blocksMax);
	arena->blocks[arena->blocksNum++] = (u8 *)zalloc(arena->blockSize);
	return arena;
}

void *allocateMemory(MemoryArena *arena, int size) {
	if (size > arena->blockSize) {
		logf("Request allocation is too large for arena, it will leak!!! (%d/%d)\n", size, arena->blockSize);
		return zalloc(size);
	}

	int bytesLeftInBlock = arena->blockSize - arena->blockPosition;
	if (size > bytesLeftInBlock) {
		if (arena->blocksNum > arena->blocksMax-1) {
			logf("Too many allocation in arena, it will leak!!! (>%d)\n", arena->blocksMax);
			return zalloc(size);
		}

		arena->blocks[arena->blocksNum++] = (u8 *)zalloc(arena->blockSize);
		arena->blockPosition = 0;
	}

	u8 *currentBlock = arena->blocks[arena->blocksNum-1];
	void *mem = currentBlock + arena->blockPosition;
	arena->blockPosition += size;
	return mem;
}

void destroyMemoryArena(MemoryArena *arena) {
	for (int i = 0; i < arena->blocksNum; i++) {
		free(arena->blocks[i]);
	}
	free(arena->blocks);
	free(arena);
}

TaggedText *parseTaggedText(char *inText) {
	TaggedText *taggedText = (TaggedText *)zalloc(sizeof(TaggedText));

	StringBuilder builder = createStringBuilder();
	int builderCount = 0;

	int tokenMaxLen = TEXT_TAG_NAME_MAX_LEN;
	char *token = (char *)frameMalloc(TEXT_TAG_NAME_MAX_LEN);
	char *inTextStart = inText;
	for (;;) {
		char *inTextEnd = strchr(inTextStart, '[');
		bool shouldBreak = false;
		if (!inTextEnd) {
			inTextEnd = &inTextStart[strlen(inTextStart)];
			shouldBreak = true;
		}

		int charsToCopy = inTextEnd - inTextStart;
		addText(&builder, inTextStart, charsToCopy);
		builderCount += charsToCopy + countChar(inTextStart, '\n', charsToCopy);
		if (shouldBreak) break;

		char *tokenStart = inTextEnd+1;
		char *tokenEnd = strchr(tokenStart, ']');
		if (!tokenEnd) {
			logf("Couldn't find ']'\n");
			break;
		}

		int tokenLen = tokenEnd - tokenStart;
		if (tokenLen > TEXT_TAG_NAME_MAX_LEN) {
			logf("Tag too long!\n"); break;
		}

		strncpy(token, tokenStart, tokenLen);
		token[tokenLen] = 0;

		if (taggedText->tagsNum > TAGGED_TEXT_TAGS_MAX-1) {
			logf("Too many tags!\n");
			break;
		}

		if (token[0] == '/') {
			TextTag *tag = &taggedText->tags[taggedText->tagsNum-1];
			if (tag->endIndex != -1) logf("Double ending %s tag???\n", tag->name);
			tag->endIndex = builderCount;
		} else {
			TextTag *tag = &taggedText->tags[taggedText->tagsNum++];
			strcpy(tag->name, token);
			tag->startIndex = builderCount;
			tag->endIndex = -1;
		}

		inTextStart = tokenEnd+1;
	}

	taggedText->text = stringClone(builder.string);
	destroy(builder);
	return taggedText;
}

bool inTextTag(TaggedText *taggedText, char *tagName, int index) {
	for (int i = 0; i < taggedText->tagsNum; i++) {
		TextTag *tag = &taggedText->tags[i];
		if (index < tag->startIndex) continue;
		if (index > tag->endIndex-1) continue;
		if (!streq(tag->name, tagName)) continue;
		return true;
	}

	return false;
}

#define NV_STRING_JOIN2(arg1, arg2) NV_DO_STRING_JOIN2(arg1, arg2)
#define NV_DO_STRING_JOIN2(arg1, arg2) arg1 ## arg2

#define ForEach(_itDecl, _array) \
					for (int NV_STRING_JOIN2(i, __LINE__) = 0; NV_STRING_JOIN2(i, __LINE__) < (_array)->count; NV_STRING_JOIN2(i, __LINE__)++) \
						if (int i = NV_STRING_JOIN2(i, __LINE__)) \
							if (_itDecl = (*(_array))[i])

#define Each(_array) \
					for (int NV_STRING_JOIN2(i, __LINE__) = 0; NV_STRING_JOIN2(i, __LINE__) < (_array)->count; NV_STRING_JOIN2(i, __LINE__)++) \
						if (int i = NV_STRING_JOIN2(i, __LINE__)) \
							if (auto it = (*(_array))[i])

/// Dynamic array

template <typename T>
struct Array {
	T *elements;
	int count;
	int max;
	bool doNotMemsetNewElements;

	T *operator [](int index) {
		return &this->elements[index];
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
T *push(Array<T> *array, T element) {
	T *nextElement = next(array);
	if (nextElement) *nextElement = element;
	return nextElement;
}

template <typename T>
T *next(Array<T> *array) {
	if (array->count > array->max-1) {
		resizeArray(array, (array->max*1.5)+1);
	}

	T *element = array->get(array->count);
	array->count++;
	if (!array->doNotMemsetNewElements) memset(element, 0, sizeof(T));
	return element;
}

template <typename T>
void spliceIndex(Array<T> *array, int index) {
	arraySpliceIndex(array->elements, array->count, sizeof(T), index);
	array->count--;
}

/// /Dynamic array

/// Static Array
template <typename T, int N>
struct StaticArray {
	T elements[N];
	int count;
	int max = N;

	bool noLogOnOverflow;
	bool noDeleteLastOnOverFlow;

	T *operator [](int index) {
		return &this->elements[index];
	}

	T *get(int index) {
		return &this->elements[index];
	}
};

template <typename T, int N>
StaticArray<T, N> *frameMallocStaticArray() {
	StaticArray<T, N> array = frameMalloc(sizeof(StaticArray<T, N>));
	return array;
}

template <typename T, int N>
T *push(StaticArray<T, N> *array, T element) {
	T *nextElement = next(array);
	if (nextElement) *nextElement = element;
	return nextElement;
}

template <typename T, int N>
T *next(StaticArray<T, N> *array) {
	if (array->count > N-1) {
		if (!array->noLogOnOverflow) logf("Static array overflow\n");
		if (array->noDeleteLastOnOverFlow) {
			return NULL;
		} else {
			array->count--;
		}
	}

	T *element = array->get(array->count);
	array->count++;
	return element;
}

template <typename T, int N>
void spliceIndex(StaticArray<T, N> *array, int index) {
	arraySpliceIndex(array->elements, array->count, sizeof(T), index);
	array->count--;
}

/// /Static Array
