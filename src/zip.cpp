#ifdef ZIP_HEADER
#undef ZIP_HEADER

struct LocalFileHeader {
	u32 signature;
	u16 extractVersion;
	u16 bitFlags;
	u16 compressionMethod;
	u16 loadModFileTime;
	u16 loadModFileDate;
	u32 crc;
	u32 compressedSize;
	u32 uncompressedSize;
	u16 fileNameLength;
	u16 extraFieldLength;

	char *fileName;
	char *extraField;
	unsigned char *compressedData;
	unsigned char *uncompressedData;
};

struct Zip {
#define ZIP_HEADERS_MAX 2048
	LocalFileHeader headers[ZIP_HEADERS_MAX];
	int headersNum;
};

Zip *openZip(unsigned char *data, int size);
void extractZip(Zip *zip);
void closeZip(Zip *zip);

#else

Zip *openZip(unsigned char *data, int size) {
	Zip *zip = (Zip *)zalloc(sizeof(Zip));
	logf("About to open a %d bytes zip\n", size);

	const int INFLATE_BUFFER_SIZE = size*2;
	unsigned char *inBuffer = (unsigned char *)malloc(INFLATE_BUFFER_SIZE);
	unsigned char *outBuffer = (unsigned char *)malloc(INFLATE_BUFFER_SIZE);

	if (!inBuffer || !outBuffer) {
		logf("Failed to malloc in/out buffers of size %fmb\n", INFLATE_BUFFER_SIZE / (float)Megabytes(1));
	}

	for (;;) {
		int signature;
		ConsumeBytes(&signature, data, 4);
		if (signature == 0x04034b50) {
			LocalFileHeader *header = &zip->headers[zip->headersNum++];
			ConsumeBytes(&header->extractVersion, data, 2);
			ConsumeBytes(&header->bitFlags, data, 2);
			ConsumeBytes(&header->compressionMethod, data, 2);
			ConsumeBytes(&header->loadModFileTime, data, 2);
			ConsumeBytes(&header->loadModFileDate, data, 2);
			ConsumeBytes(&header->crc, data, 4);
			ConsumeBytes(&header->compressedSize, data, 4);
			ConsumeBytes(&header->uncompressedSize, data, 4);
			ConsumeBytes(&header->fileNameLength, data, 2);
			ConsumeBytes(&header->extraFieldLength, data, 2);

			header->fileName = (char *)malloc(header->fileNameLength+1);
			memset(header->fileName, 0, header->fileNameLength+1);
			ConsumeBytes(header->fileName, data, header->fileNameLength);

			header->extraField = (char *)malloc(header->extraFieldLength+1);
			memset(header->extraField, 0, header->extraFieldLength+1);
			ConsumeBytes(header->extraField, data, header->extraFieldLength);

			header->compressedData = (unsigned char *)malloc(header->compressedSize);
			ConsumeBytes(header->compressedData, data, header->compressedSize);

			// if (header->uncompressedSize >= Kilobytes(500)) logf("Unzipping %s (%.2f)\n", header->fileName, (float)header->uncompressedSize / (float)Megabytes(1));
			header->uncompressedData = (unsigned char *)malloc(header->uncompressedSize);

			if (header->compressionMethod == 0) {
				memcpy(header->uncompressedData, header->compressedData, header->uncompressedSize);
			} else if (header->compressionMethod == 8) {
#if 0
				z_stream stream = {};
				stream.next_in = header->compressedData;
				stream.avail_in = header->compressedSize;
				stream.next_out = header->uncompressedData;
				stream.avail_out = header->uncompressedSize;

				int status;
				if ((status = inflateInit2(&stream, -MZ_DEFAULT_WINDOW_BITS)) == Z_OK) {
					if ((status = inflate(&stream, Z_SYNC_FLUSH)) == Z_STREAM_END) {
						if ((status = inflateEnd(&stream)) != Z_OK) {
							printf("inflateEnd() failed with %d!\n", status);
						}
					} else {
						printf("inflate() failed with %d!\n", status);
					}
				} else {
					printf("inflateInit() failed with %d!\n", status);
				}
#else
				u32 infile_remaining = header->compressedSize;
				unsigned char *inPos = header->compressedData;
				unsigned char *outPos = header->uncompressedData;
				z_stream stream = {};
				stream.next_in = inBuffer;
				stream.avail_in = 0;
				stream.next_out = outBuffer;
				stream.avail_out = INFLATE_BUFFER_SIZE;
				if (inflateInit2(&stream, -MZ_DEFAULT_WINDOW_BITS)) {
					printf("inflateInit() failed!\n");
					return NULL;
				}
				for (;;) { // I think this always happens in one iteration, unless I don't want it to for some reason
					// This actually has to go in one iteration or it crashes, INFLATE_BUFFER_SIZE was hacked to make this happen
					if (!stream.avail_in) {
						u32 bytesToGet = MinNum(INFLATE_BUFFER_SIZE, infile_remaining);
						memcpy(inBuffer, inPos, bytesToGet);
						stream.next_in = inBuffer;
						stream.avail_in = bytesToGet;
						inPos += bytesToGet;
						infile_remaining -= bytesToGet;
					}
					int status = inflate(&stream, Z_SYNC_FLUSH);
					if (status == Z_STREAM_END || !stream.avail_out) {
						u32 bytesDone = INFLATE_BUFFER_SIZE - stream.avail_out;
						memcpy(outPos, outBuffer, bytesDone);
						stream.next_out = outBuffer;
						stream.avail_out = INFLATE_BUFFER_SIZE;
					}
					if (status == Z_STREAM_END) {
						// printf("%s byte are(at unzip time):\n", header->fileName);
						// dumpHex(header->uncompressedData, header->uncompressedSize);
						break;
					} else if (status != Z_OK) {
						printf("inflate() failed with status %i!\n", status);
						return NULL;
					}
				}
				if (inflateEnd(&stream) != Z_OK) {
					printf("inflateEnd() failed!\n");
					return NULL;
				}
#endif
			}
		} else if (signature == 0x08074b50) {
			printf("We don't parse data descriptors! (Unsupported zip file)\n");
			return NULL;
		} else if (signature == 0x02014b50) { // Central directory structure
			break;
		} else if (signature == 0x08064b50) { // Archive extra data record
			break;
		} else {
			printf("Unknown signature %x (Unsupported zip file)\n", signature);
			return NULL;
		}
	}

	free(inBuffer);
	free(outBuffer);
	return zip;
}

void extractZip(Zip *zip, const char *destPath) {
	char *realName = (char *)malloc(PATH_MAX_LEN);

	for (int i = 0; i < zip->headersNum; i++) {
		LocalFileHeader *header = &zip->headers[i];
		// logf("Writing path: %s\n", header->fileName);
		snprintf(realName, PATH_MAX_LEN, "%s/%s", destPath, header->fileName);
		if (header->fileName[strlen(header->fileName)-1] == '/') {
			realName[strlen(realName)-1] = 0;
			createDirectory(realName);
		} else {
			writeFile(realName, header->uncompressedData, header->uncompressedSize);
		}
	}

}

void extractZip(Zip *zip) {
	for (int i = 0; i < zip->headersNum; i++) {
		LocalFileHeader *header = &zip->headers[i];
		// logf("Writing path: %s\n", header->fileName);
		if (header->fileName[strlen(header->fileName)-1] == '/') {
			char *dirName = frameMalloc(PATH_MAX_LEN);
			strcpy(dirName, header->fileName);
			dirName[strlen(dirName)-1] = 0;
			createDirectory(dirName);
		} else {
			writeFile(header->fileName, header->uncompressedData, header->uncompressedSize);
		}
	}
}

void closeZip(Zip *zip) {
	for (int i = 0; i < zip->headersNum; i++) {
		LocalFileHeader *header = &zip->headers[i];

		if (header->fileName) free(header->fileName);
		if (header->extraField) free(header->extraField);
		if (header->compressedData) free(header->compressedData);
		if (header->uncompressedData) free(header->uncompressedData);
	}

	free(zip);
}

#endif
