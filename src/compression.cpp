void *compressBytes(void *inBytes, int inBytesLen, int *compressedBytesSize=NULL);
void *uncompressBytes(void *inBytes, int inBytesLen, int *uncompressedBytesSize=NULL);

#define COMPRESS_BYTES_MAX (Megabytes(10))
unsigned char byteArrayBuffer[COMPRESS_BYTES_MAX];

void *compressBytes(void *inBytes, int inBytesLen, int *compressedBytesSize) {
	if (compressedBytesSize) *compressedBytesSize = 0;

	unsigned long newLen = compressBound((COMPRESS_BYTES_MAX) * 1.001 + 12); // Whatever I guess, zlib told me to
	int compressStatus = compress((unsigned char *)byteArrayBuffer, &newLen, (unsigned char *)inBytes, inBytesLen);

	if (compressStatus != Z_OK) {
		printf("Error compressing %d bytes (%d)\n", inBytesLen, compressStatus);
		return NULL;
	}

	void *res = malloc(newLen);
	memcpy(res, byteArrayBuffer, newLen);

	if (compressedBytesSize) *compressedBytesSize = newLen;
	return res;
}

void *uncompressBytes(void *inBytes, int inBytesLen, int *uncompressedBytesSize) {
	if (uncompressedBytesSize) *uncompressedBytesSize = 0;

	unsigned long newLen = compressBound((COMPRESS_BYTES_MAX) * 1.001 + 12); // Whatever I guess, zlib told me to
	int compressStatus = uncompress((unsigned char *)byteArrayBuffer, &newLen, (unsigned char *)inBytes, inBytesLen);

	if (compressStatus != Z_OK) {
		printf("Error uncompressing %d bytes (%d)\n", inBytesLen, compressStatus);
		return NULL;
	}

	void *res = malloc(newLen);
	memcpy(res, byteArrayBuffer, newLen);

	if (uncompressedBytesSize) *uncompressedBytesSize = newLen;
	return res;
}
