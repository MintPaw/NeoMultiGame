void *compressBytes(void *inBytes, int inBytesLen, int *compressedBytesSize=NULL);
void *uncompressBytes(void *inBytes, int inBytesLen, int *uncompressedBytesSize=NULL);

void *compressBytes(void *inBytes, int inBytesLen, int *compressedBytesSize) {
	if (compressedBytesSize) *compressedBytesSize = 0;

	unsigned long newLen = compressBound(inBytesLen * 1.001 + 12); // Whatever I guess, zlib told me to
  u8 *buffer = (u8 *)malloc(newLen);
	int compressStatus = compress(buffer, &newLen, (unsigned char *)inBytes, inBytesLen);

	if (compressStatus != Z_OK) {
		printf("Error compressing %d bytes (%d)\n", inBytesLen, compressStatus);
		return NULL;
	}

	void *res = malloc(newLen);
	memcpy(res, buffer, newLen);
  free(buffer);

	if (compressedBytesSize) *compressedBytesSize = newLen;
	return res;
}

void *uncompressBytes(void *inBytes, int inBytesLen, int *uncompressedBytesSize) {
	if (uncompressedBytesSize) *uncompressedBytesSize = 0;

  unsigned long newLen = Megabytes(100);
  u8 *buffer = (u8 *)malloc(newLen);
	int compressStatus = uncompress(buffer, &newLen, (u8 *)inBytes, inBytesLen);

	if (compressStatus != Z_OK) {
		printf("Error uncompressing %d bytes (%d)\n", inBytesLen, compressStatus);
		return NULL;
	}

	void *res = malloc(newLen);
	memcpy(res, buffer, newLen);
  free(buffer);

	if (uncompressedBytesSize) *uncompressedBytesSize = newLen;
	return res;
}
