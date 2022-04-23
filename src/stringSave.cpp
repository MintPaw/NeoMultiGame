const char *stringSaveEntryDelim = "\n--|--\n";

enum StringSaveEntryType {
	STRING_SAVE_ENTRY_TYPE_BYTES,
};

struct StringSaveEntry {
	char *name;
	void *value;
	StringSaveEntryType type;
	int size;

	void *getBytes();
};

struct StringSave {
	StringSaveEntry *entries;
	int entriesNum;
	int entriesMax;

	StringSaveEntry *getNewEntry(const char *name);
	void addBytes(const char *name, void *value, int size);

	char *getAsString();
	void loadString(const char *data);
	void destroy();
};

StringSave initStringSave();

StringSave initStringSave() {
	StringSave save = {};
	memset(&save, 0, sizeof(StringSave));
	save.entriesNum = 0;
	save.entriesMax = 1;
	save.entries = (StringSaveEntry *)malloc(sizeof(StringSaveEntry) * save.entriesMax);
	return save;
}

void StringSave::loadString(const char *data) {
	StringSave *save = this;

	const int TYPE_STR_MAX_LEN = 32;
	char typeStr[TYPE_STR_MAX_LEN];

	const int NAME_STR_MAX_LEN = 64;
	char nameStr[NAME_STR_MAX_LEN];

	const char *lineStart = data;
	for (int i = 0; ; i++) {
		const char *lineEnd = strstr(lineStart, stringSaveEntryDelim);
		if (!lineEnd) break;

		const char *typeStrStart = lineStart;
		const char *typeStrEnd = strchr(typeStrStart, ' ');
		int typeLen = typeStrEnd - typeStrStart;

		const char *nameStrStart = typeStrEnd+1;
		const char *nameStrEnd = strchr(nameStrStart, '=');
		int nameLen = nameStrEnd - nameStrStart;

		const char *dataStrStart = nameStrEnd+1;
		const char *dataStrEnd = lineEnd;
		int dataLen = dataStrEnd - dataStrStart;

		if (typeLen >= TYPE_STR_MAX_LEN) Panic("typeStr would be too long!\n");
		strncpy(typeStr, typeStrStart, typeLen);
		typeStr[typeLen] = 0;

		if (nameLen >= NAME_STR_MAX_LEN) Panic("nameStr would be too long!\n");
		strncpy(nameStr, nameStrStart, nameLen);
		nameStr[nameLen] = 0;

		char *dataStr = (char *)malloc(dataLen+1);

		strncpy(dataStr, dataStrStart, dataLen);
		dataStr[dataLen] = 0;

		if (streq(typeStr, "void")) {
			char *dataCur = dataStr;

			int sizeStrLen = strchr(dataCur, ',') - dataCur;
			char sizeStr[128];
			if (sizeStrLen > 128) Panic("String too long");

			strncpy(sizeStr, dataCur, sizeStrLen);
			sizeStr[sizeStrLen] = 0;
			dataCur += sizeStrLen+1;

			int size = atol(sizeStr);
			// logf("var |%s| size: |%s| = |%d|\n", nameStr, sizeStr, size);

			int dataSize;
			unsigned char *data = (unsigned char *)convertFromHex(dataCur, &dataSize, true);

			save->addBytes(nameStr, data, size);
			free(data);
		} else {
			printf("Unknown entry type: %s\n", typeStr);
			Assert(0);
		}

		free(dataStr);

		lineStart = lineEnd + strlen(stringSaveEntryDelim);
	}
}

StringSaveEntry *StringSave::getNewEntry(const char *name) {
	StringSave *save = this;

	if (save->entriesNum >= save->entriesMax) {
		int oldMax = entriesMax;
		save->entriesMax *= 2;
		// save->entriesMax++;
		// printf("Reallocing\n");

		StringSaveEntry *newEntries = (StringSaveEntry *)malloc(sizeof(StringSaveEntry) * save->entriesMax);
		memset(newEntries, 0xAB, sizeof(StringSaveEntry) * oldMax);
		memcpy(newEntries, save->entries, sizeof(StringSaveEntry) * oldMax);

		free(save->entries);
		save->entries = newEntries;
	}

	StringSaveEntry *entry = &save->entries[save->entriesNum++];

	memset(entry, 0, sizeof(StringSaveEntry));

	entry->name = (char *)malloc(sizeof(char) * strlen(name) + 1);
	strcpy(entry->name, name);

	return entry;
}

void StringSave::addBytes(const char *name, void *value, int size) {
	StringSave *save = this;

	StringSaveEntry *entry = save->getNewEntry(name);
	entry->type = STRING_SAVE_ENTRY_TYPE_BYTES;
	entry->size = size;
	entry->value = malloc(size);
	memcpy(entry->value, value, size);
}

char *StringSave::getAsString() {
	StringSave *save = this;

	int strMax = 1;
	char *str = (char *)malloc(strMax);
	memset(str, 0, strMax);
	int strLen = strlen(str);

	int lineMax = 1;
	char *line = (char *)malloc(lineMax);

	for (int i = 0; i < save->entriesNum; i++) {
		StringSaveEntry *entry = &save->entries[i];

		for (;;) {
			int charsWritten = 0;

			if (entry->type == STRING_SAVE_ENTRY_TYPE_BYTES) {
				char *hexStr = convertToHex(entry->value, entry->size);
				int neededLineMax = strlen(hexStr) + strlen(entry->name) + 256;
				if (lineMax < neededLineMax) {
					free(line);
					lineMax = neededLineMax;
					line = (char *)malloc(lineMax);
				}

				snprintf(line, lineMax, "void %s=%d,", entry->name, entry->size);
				strcat(line, hexStr);
				free(hexStr);
				charsWritten = neededLineMax-1;

				strcat(line, stringSaveEntryDelim);
			}

			if (charsWritten < lineMax) {
				break;
			} else {
				int oldLineMax = lineMax;
				lineMax *= 2;
				char *newLine = (char *)malloc(lineMax);
				memcpy(newLine, line, oldLineMax);

				free(line);
				line = newLine;
			}
		}

		int lineLen = strlen(line);
		while (strLen + lineLen + 1 >= strMax) {
			int prevStrLen = strlen(str);
			strMax *= 2;
			char *newStr = (char *)malloc(strMax);
			memcpy(newStr, str, prevStrLen);
			newStr[prevStrLen] = 0;

			free(str);
			str = newStr;
		}

		char *strEnd = str + strLen;
		strcpy(strEnd, line);
		strLen += lineLen;
	}

	free(line);
	return str;
}

void *StringSaveEntry::getBytes() {
	if (this->type != STRING_SAVE_ENTRY_TYPE_BYTES) printf("Loading bytes value from non-bytes entry! %s\n", this->name);
	return this->value;
}

void StringSave::destroy() {
	StringSave *save = this;

	for (int i = 0; i < save->entriesNum; i++) {
		StringSaveEntry *entry = &save->entries[i];
		free(entry->name);
		free(entry->value);
	}
	free(save->entries);
}
