#ifdef DATA_STREAM_HEADER
#undef DATA_STREAM_HEADER

struct DataStream {
	int dataMax;
	u8 *data;
	int index;
};

void readBytes(DataStream *stream, void *ptr, int size);
char *readString(DataStream *stream);
char *readFrameString(DataStream *stream);
void readStringInto(DataStream *stream, char *dest, int max);
u64 readU64(DataStream *stream);
u32 readU32(DataStream *stream);
Matrix4 readMatrix4(DataStream *stream);
Xform readXform(DataStream *stream);
Xform2 readXform2(DataStream *stream);
AABB readAABB(DataStream *stream);
u8 readU8(DataStream *stream);
u16 readU16(DataStream *stream);
float readFloat(DataStream *stream);
double readDouble(DataStream *stream);
Rect readRect(DataStream *stream);
Vec4 readVec4(DataStream *stream);
Vec3 readVec3(DataStream *stream);
Vec2 readVec2(DataStream *stream);
Vec2i readVec2i(DataStream *stream);

DataStream *newDataStream(int startSize=8);
DataStream *createDataStream(int startSize=8) { return newDataStream(startSize); }
void destroyDataStream(DataStream *stream);

void writeBytes(DataStream *stream, void *ptr, int size);
void writeString(DataStream *stream, char *string);

#ifndef NO_DATA_STREAM_IO
DataStream *loadDataStream(char *path);
DataStream *loadDataStreamFromHexString(char *hexStr);
bool writeDataStream(char *path, DataStream *stream);
char *writeToHexString(DataStream *stream);
#endif //NO_DATA_STREAM_IO


void writeU64(DataStream *stream, u64 value) { writeBytes(stream, &value, 8); }
void writeU32(DataStream *stream, u32 value) { writeBytes(stream, &value, 4); }
void writeU8(DataStream *stream, u8 value) { writeBytes(stream, &value, sizeof(u8)); }
void writeU16(DataStream *stream, u16 value) { writeBytes(stream, &value, sizeof(u16)); }
void writeFloat(DataStream *stream, float value) { writeBytes(stream, &value, 4); }
void writeDouble(DataStream *stream, double value) { writeBytes(stream, &value, 8); }
void writeRect(DataStream *stream, Rect value) { writeBytes(stream, &value, sizeof(Rect)); }
void writeVec4(DataStream *stream, Vec4 value) { writeBytes(stream, &value, sizeof(Vec4)); }
void writeVec3(DataStream *stream, Vec3 value) { writeBytes(stream, &value, sizeof(Vec3)); }
void writeVec2(DataStream *stream, Vec2 value) { writeBytes(stream, &value, sizeof(Vec2)); }
void writeVec2i(DataStream *stream, Vec2i value) { writeBytes(stream, &value, sizeof(Vec2i)); }
void writeMatrix4(DataStream *stream, Matrix4 value) { writeBytes(stream, &value, sizeof(Matrix4)); }
void writeXform(DataStream *stream, Xform value) { writeBytes(stream, &value, sizeof(Xform)); }
void writeXform2(DataStream *stream, Xform2 value) { writeBytes(stream, &value, sizeof(Xform2)); }
void writeAABB(DataStream *stream, AABB value) { writeBytes(stream, &value, sizeof(AABB)); }

#else

void readBytes(DataStream *stream, void *ptr, int size) {
	if (stream->index + size > stream->dataMax) { // This used to be stream->dataMax-1
		size = stream->dataMax - stream->index;
		logf("Read too many bytes\n");
	}

	memcpy(ptr, &stream->data[stream->index], size);
	stream->index += size;
}

char *readString(DataStream *stream) {
	int size = strlen((char *)&stream->data[stream->index]) + 1;
	if (size == 1) {
		stream->index++;
		return NULL;
	}

	if (size > stream->dataMax-stream->index) {
		logf("Read too long of a string by %d bytes\n", size - stream->dataMax-stream->index);
	}

	char *str = (char *)malloc(size);
	strncpy(str, (const char *)&stream->data[stream->index], size);
	stream->index += size;

	return str;
}

char *readFrameString(DataStream *stream) {
	int size = strlen((char *)&stream->data[stream->index]) + 1;
	if (size == 1) {
		stream->index++;
		return NULL;
	}

	if (size > stream->dataMax-stream->index) {
		logf("Read too long of a string by %d bytes\n", size - stream->dataMax-stream->index);
	}

	char *str = (char *)frameMalloc(size);
	strncpy(str, (const char *)&stream->data[stream->index], size);
	stream->index += size;

	return str;
}

void readStringInto(DataStream *stream, char *dest, int max) {
	dest[0] = 0;

	int size = strlen((char *)&stream->data[stream->index]) + 1;
	if (size == 1) {
		stream->index++;
		return;
	}

	if (size > stream->dataMax-stream->index) {
		logf("Read too long of a string by %d bytes\n", size - stream->dataMax-stream->index);
	}

	strncpy(dest, (const char *)&stream->data[stream->index], MinNum(max, size));
	stream->index += size;
}

u32 readU32(DataStream *stream) {
	u32 ret;
	readBytes(stream, &ret, 4);
	return ret;
}

u64 readU64(DataStream *stream) {
	u64 ret;
	readBytes(stream, &ret, 8);
	return ret;
}

Matrix4 readMatrix4(DataStream *stream) {
	Matrix4 ret;
	readBytes(stream, &ret, sizeof(Matrix4));
	return ret;
}

Xform readXform(DataStream *stream) {
	Xform ret;
	readBytes(stream, &ret, sizeof(Xform));
	return ret;
}

Xform2 readXform2(DataStream *stream) {
	Xform2 ret;
	readBytes(stream, &ret, sizeof(Xform2));
	return ret;
}

AABB readAABB(DataStream *stream) {
	AABB ret;
	readBytes(stream, &ret, sizeof(AABB));
	return ret;
}

u8 readU8(DataStream *stream) {
	u8 ret;
	readBytes(stream, &ret, sizeof(u8));
	return ret;
}

u16 readU16(DataStream *stream) {
	u16 ret;
	readBytes(stream, &ret, sizeof(u16));
	return ret;
}

float readFloat(DataStream *stream) {
	float ret;
	readBytes(stream, &ret, 4);
	return ret;
}

double readDouble(DataStream *stream) {
	double ret;
	readBytes(stream, &ret, 8);
	return ret;
}

Rect readRect(DataStream *stream) {
	Rect ret;
	readBytes(stream, &ret, sizeof(Rect));
	return ret;
}

Vec4 readVec4(DataStream *stream) {
	Vec4 ret;
	readBytes(stream, &ret, sizeof(Vec4));
	return ret;
}

Vec3 readVec3(DataStream *stream) {
	Vec3 ret;
	readBytes(stream, &ret, sizeof(Vec3));
	return ret;
}

Vec2 readVec2(DataStream *stream) {
	Vec2 ret;
	readBytes(stream, &ret, sizeof(Vec2));
	return ret;
}

Vec2i readVec2i(DataStream *stream) {
	Vec2i ret;
	readBytes(stream, &ret, sizeof(Vec2i));
	return ret;
}

DataStream *newDataStream(int startSize) {
	DataStream *stream = (DataStream *)zalloc(sizeof(DataStream));
	stream->dataMax = startSize;
	stream->data = (u8 *)zalloc(stream->dataMax);
	return stream;
}

void destroyDataStream(DataStream *stream) {
	free(stream->data);
	free(stream);
}

void writeBytes(DataStream *stream, void *ptr, int size) {
	while (stream->index + size > stream->dataMax-1) {
		stream->data = (u8 *)resizeArray(stream->data, 1, stream->dataMax, stream->dataMax*2);
		stream->dataMax *= 2;
	}

	if (size == 0) Panic("No 0\n");
	memcpy(&stream->data[stream->index], ptr, size);
	stream->index += size;
}

void writeString(DataStream *stream, char *string) {
	if (!string) {
		char data = 0;
		writeBytes(stream, &data, 1);
		return;
	}

	writeBytes(stream, string, strlen(string)+1);
}

#ifndef NO_DATA_STREAM_IO
DataStream *loadDataStream(char *path) {
	if (!fileExists(path)) return NULL;
	DataStream *stream = (DataStream *)zalloc(sizeof(DataStream));

	void *fileData = readFile(path, &stream->dataMax);
	stream->data = (u8 *)zalloc(stream->dataMax);
	memcpy(stream->data, fileData, stream->dataMax);
	free(fileData);
	return stream;
}

DataStream *loadDataStreamFromHexString(char *hexStr) {
	if (!hexStr || !hexStr[0]) return NULL;

	int smallDataSize;
	void *smallData = convertFromHexString(hexStr, &smallDataSize);

	DataStream *stream = (DataStream *)zalloc(sizeof(DataStream));
	stream->data = (u8 *)uncompressBytes(smallData, smallDataSize, &stream->dataMax);

	free(smallData);
	return stream;
}

bool writeDataStream(char *path, DataStream *stream) {
	return writeFile(path, stream->data, stream->index);
}

char *writeToHexString(DataStream *stream) {
	int newDataSize;
	void *newData = compressBytes(stream->data, stream->index, &newDataSize);
	char *hexStr = convertToHexString(newData, newDataSize);
	free(newData);
	return hexStr;
}
#endif //NO_DATA_STREAM_IO

#endif
