#ifdef ZIP_HEADER
#undef ZIP_HEADER

void openAndExtractZip(void *data, int size);

#else // Not ZIP_HEADER

void openAndExtractZip(void *data, int size) {
	zip_t *zip = zip_stream_open((char *)data, size, MZ_DEFAULT_LEVEL, 'r');

	void *buffer = NULL;
	size_t bufferSize = 0;

	int totalEntries = zip_entries_total(zip);
	for (int i = 0; i < totalEntries; i++) {
		zip_entry_openbyindex(zip, i);

		char *name = (char *)zip_entry_name(zip);
		int isdir = zip_entry_isdir(zip);

		if (isdir) {
			createDirectory(name);
		} else {
			u32 size = zip_entry_size(zip);

			zip_entry_read(zip, &buffer, &bufferSize);

			writeFile(name, buffer, bufferSize);
		}

		zip_entry_close(zip);
	}

	zip_stream_close(zip);

	free(buffer);
}

// void *buf = NULL;
// size_t bufsize;

// struct zip_t *zip = zip_open("foo.zip", 0, 'r');
// {
//     zip_entry_open(zip, "foo-1.txt");
//     {
//         zip_entry_read(zip, &buf, &bufsize);
//     }
//     zip_entry_close(zip);
// }
// zip_close(zip);

// free(buf);

#endif // End !ZIP_HEADER
