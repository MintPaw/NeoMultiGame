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

  u64 bufferSize = Megabytes(5);
	u8 *buffer = (u8 *)malloc(bufferSize);

	for (;;) {
		int compressStatus = uncompress(buffer, (unsigned long *)&bufferSize, (u8 *)inBytes, inBytesLen);
		if (compressStatus == Z_OK) break;

		if (compressStatus == Z_BUF_ERROR) {
			bufferSize *= 2;
			if (bufferSize > Gigabytes(1)) {
				logf("Refusing to uncompressed over 1gb\n");
				free(buffer);
				return NULL;
			}

			free(buffer);
			buffer = (u8 *)malloc(bufferSize);
			continue;
		}

		logf("Zlib uncompress error %d bytes (%d)\n", inBytesLen, compressStatus);
		free(buffer);
		return NULL;
	}

	void *res = malloc(bufferSize);
	memcpy(res, buffer, bufferSize);
  free(buffer);

	if (uncompressedBytesSize) *uncompressedBytesSize = bufferSize;
	return res;
}
