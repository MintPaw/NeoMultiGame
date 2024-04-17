#ifdef ZIP_HEADER
#undef ZIP_HEADER

void openAndExtractZip(void *data, int size);

#else

void openAndExtractZip(void *data, int size) {
	zip_t *zip = zip_stream_open((char *)data, size, MZ_DEFAULT_LEVEL, 'r');

	void *buffer = NULL;
	size_t bufferSize = 0;

	int totalEntries = zip_entries_total(zip);
	for (int i = 0; i < totalEntries; i++) {
		zip_entry_openbyindex(zip, i);

		char *name = (char *)zip_entry_name(zip);
		if (zip_entry_isdir(zip)) {
			createDirectory(name);
		} else {
			// u32 size = zip_entry_size(zip); // ???

			zip_entry_read(zip, &buffer, &bufferSize);

			writeFile(name, buffer, bufferSize);
		}

		zip_entry_close(zip);
	}

	zip_stream_close(zip);

	free(buffer);
}

#endif
