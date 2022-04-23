struct MemberInfo {
#define TYPE_NAME_MAX_LEN 32 //@incomplete Actually enforce
	char name[TYPE_NAME_MAX_LEN];
	int offset;
	int size;
};

struct StructInfo {
#define STRUCT_NAME_MAX_LEN 64 // Actually enforce
	char name[STRUCT_NAME_MAX_LEN];

#define STRUCT_MEMBER_MAX 128 // Actually enforce
	MemberInfo memberInfos[STRUCT_MEMBER_MAX];
	int memberInfosNum;
};

#define STRUCT_INFOS_MAX 128 // Actually enforce
StructInfo structInfos[STRUCT_INFOS_MAX] = {};
int structInfosNum = 0;

StructInfo *getStructInfo(char *structName);

void saveStruct(char *structName, char *savePath, void *structPtr);
char *saveStructString(char *structName, void *structPtr);

void loadStruct(char *structName, char *loadPath, void *structPtr);
void loadStructString(char *structName, char *dataString, void *structPtr);

void saveStructArray(char *structName, char *savePath, void *arrayPtr, int arraySize, int elementSize);
char *saveStructArrayString(char *structName, void *arrayPtr, int arraySize, int elementSize);

void loadStructArray(char *structName, char *loadPath, void *arrayPtr, int arraySize, int elementSize);
void loadStructArrayString(char *structName, char *dataStr, void *arrayPtr, int arraySize, int elementSize);

#define RegMem(Struct, Member) regMem(STRINGIFY(Struct), STRINGIFY(Member), sizeof(((Struct *)0)->Member), offsetof(Struct, Member))

void regMem(char *structName, char *memberName, int memberSize, int offset) {
	StructInfo *info = getStructInfo(structName);

	for (int i = 0; i < info->memberInfosNum; i++) {
		if (streq(info->memberInfos[i].name, memberName)) return;
	}

	MemberInfo *memberInfo = &info->memberInfos[info->memberInfosNum++];
	strcpy(memberInfo->name, memberName);
	memberInfo->size = memberSize;
	memberInfo->offset = offset;
}

char *saveStructString(char *structName, void *structPtr) {
	StringSave save = initStringSave();
	StructInfo *info = getStructInfo(structName);
	unsigned char *realStruct = (unsigned char *)structPtr;

	for (int memberInfoI = 0; memberInfoI < info->memberInfosNum; memberInfoI++) {
		MemberInfo *memberInfo = &info->memberInfos[memberInfoI];
		void *readFrom = realStruct + memberInfo->offset;
		save.addBytes(memberInfo->name, readFrom, memberInfo->size);
	}

	char *str = save.getAsString();
	save.destroy();

	return str;
}

void saveStruct(char *structName, char *savePath, void *structPtr) {
	char *str = saveStructString(structName, structPtr);
	writeFile(savePath, str, strlen(str));
}

void loadStructString(char *structName, char *dataString, void *structPtr) {
	unsigned char *realStruct = (unsigned char *)structPtr;

	StructInfo *info = getStructInfo(structName);
	StringSave save = initStringSave();
	save.loadString(dataString);

	for (int i = 0; i < save.entriesNum; i++) {
		StringSaveEntry *entry = &save.entries[i];
		for (int memberInfoI = 0; memberInfoI < info->memberInfosNum; memberInfoI++) {
			MemberInfo *memberInfo = &info->memberInfos[memberInfoI];

			if (streq(entry->name, memberInfo->name)) {
				void *writeTo = realStruct + memberInfo->offset;
				memcpy(writeTo, entry->value, entry->size);
			}
		}
	}

	save.destroy();
}

void loadStruct(char *structName, char *loadPath, void *structPtr) {
	if (fileExists(loadPath)) {
		char *str = (char *)readFile(loadPath);
		loadStructString(structName, str, structPtr);
		free(str);
	}
}

char *saveStructArrayString(char *structName, void *arrayPtr, int arraySize, int elementSize) {
	StringSave save = initStringSave();
	StructInfo *info = getStructInfo(structName);

	for (int i = 0; i < arraySize; i++) {
		unsigned char *realStruct = (unsigned char *)arrayPtr + elementSize*i;
		for (int memberInfoI = 0; memberInfoI < info->memberInfosNum; memberInfoI++) {
			MemberInfo *memberInfo = &info->memberInfos[memberInfoI];
			void *readFrom = realStruct + memberInfo->offset;
			save.addBytes(memberInfo->name, readFrom, memberInfo->size);
		}
	}

	char *str = save.getAsString();
	save.destroy();
	return str;
}

void saveStructArray(char *structName, char *savePath, void *arrayPtr, int arraySize, int elementSize) {
	char *str = saveStructArrayString(structName, arrayPtr, arraySize, elementSize);
	writeFile(savePath, str, strlen(str));
}

void loadStructArrayString(char *structName, char *dataStr, void *arrayPtr, int arraySize, int elementSize) {
	StructInfo *info = getStructInfo(structName);

	StringSave save = initStringSave();
	save.loadString(dataStr);

	unsigned char *realStruct = NULL;
	memset(arrayPtr, 0, elementSize*arraySize);

	int index = 0;
	for (int i = 0; i < save.entriesNum; i++) {
		StringSaveEntry *entry = &save.entries[i];
		for (int memberInfoI = 0; memberInfoI < info->memberInfosNum; memberInfoI++) {
			MemberInfo *memberInfo = &info->memberInfos[memberInfoI];

			if (streq(entry->name, memberInfo->name)) {
				if (memberInfo->offset == 0) {
					if (index >= arraySize) {
						logf("Attempted to load too many elements!\n");
						save.destroy();
						return;
					}
					realStruct = (unsigned char *)arrayPtr + elementSize*(index++);
				}
				void *writeTo = realStruct + memberInfo->offset;
				memcpy(writeTo, entry->value, entry->size);
			}
		}
	}

	save.destroy();
}

void loadStructArray(char *structName, char *loadPath, void *arrayPtr, int arraySize, int elementSize) {
	if (fileExists(loadPath)) {
		char *str = (char *)readFile(loadPath);
		loadStructArrayString(structName, str, arrayPtr, arraySize, elementSize);
		free(str);
	}
}

StructInfo *getStructInfo(char *structName) {
	for (int i = 0; i < structInfosNum; i++) {
		StructInfo *info = &structInfos[i];
		if (streq(info->name, structName)) return info;
	}

	if (structInfosNum > STRUCT_INFOS_MAX-1) {
		printf("Too many struct infos!\n");
		Panic("Cannot continue;");
	}

	StructInfo *info = &structInfos[structInfosNum++];
	strcpy(info->name, structName);
	return info;
}
