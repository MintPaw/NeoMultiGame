void initFileOperations();
void initFileOperations(const char **possibleAssetPaths, int possibleAssetPathsNum);
void refreshAssetPaths();

#define ZIP_HEADER
#include "zip.cpp"

#ifdef __linux__
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#endif

enum FuzzyPathType {
	FUZZY_ALL=0,
	FUZZY_PNG,
};

char **getDirectoryList(const char *dirPath, int *numFiles, bool includingUnderscored=false, bool shallow=false, bool includeFolders=false);
char *resolveFuzzyPath(const char *fileName, const char *fileName2=NULL, FuzzyPathType type=FUZZY_ALL);
bool directoryExists(const char *dirPath);
bool fileExists(const char *fileName);
bool createDirectory(const char *dirName);
void removeDirectory(const char *dirName);
void renameFile(const char *currentName, const char *newName);
void appendFile(const char *fileName, void *data, int length);
bool writeFile(const char *fileName, void *data, int length);
void *readFile(const char *fileName, int *outSize=NULL);
void deleteFile(const char *fileName);
void loadRemoteZip(const char *url, const char *path);
void loadedRemoteFile(const char *result);
void errorLoadingRemoteFile(const char *result);

char *readTempSave();
bool writeTempSave(char *str);

int alphaSort(const void *a, const void *b) { char *str1 = *(char **)a; char *str2 = *(char **)b; return strcmp(str1, str2); }

#if defined(_WIN32)
BOOL recurseRemoveDirectory();
#endif

char **assetPaths = NULL;
int assetPathsNum = 0;

#define FILE_LIST_LIMIT 32768
char *pngAssetPaths[FILE_LIST_LIMIT];
int pngAssetPathsNum = 0;

int remoteZipsLoading = 0;
void (*remoteLoadingDoneCallback)() = NULL;

bool cantSaveTempFiles = false;

void initFileOperations() {
#if defined(_WIN32)
	HMODULE hModule = GetModuleHandleW(NULL);
	GetModuleFileNameA(hModule, exeDir, PATH_MAX_LEN);

	char *lastSlash = strrchr(exeDir, '\\');
	if (!lastSlash) Panic("No last slash found in exe path");
	*lastSlash = 0;
#elif defined(__linux__)
	strcpy(exeDir, "./");
#elif defined(__EMSCRIPTEN__)
	strcpy(exeDir, "./");
	strcpy(filePathPrefix, "");

	int canSave = EM_ASM_INT(
		if (typeof(Storage) !== "undefined") {
			return 1;
		} else {
			return 0;
		}
	);

	if (!canSave) cantSaveTempFiles = true;
#endif

	if (!projectAssetDir[0]) {
		strcpy(projectAssetDir, STRINGIFY(PROJECT_ASSET_DIR));
	}

	strcpy(filePathPrefix, projectAssetDir);
	strcat(filePathPrefix, "/");

#if defined(__EMSCRIPTEN__)
	if (fileExists("assets/preloader.zip")) {
		logf("Unzipping assets\n");
		int preloadSize;
		void *preloadData = readFile("assets/preloader.zip", &preloadSize);
		openAndExtractZip(preloadData, preloadSize);
	}
#endif
	refreshAssetPaths();
}

void refreshAssetPaths() {
	if (assetPaths) {
		for (int i = 0; i < assetPathsNum; i++) free(assetPaths[i]);
		free(assetPaths);
	}
	assetPathsNum = 0;
	pngAssetPathsNum = 0;

	assetPaths = getDirectoryList("assets", &assetPathsNum);
	qsort(assetPaths, assetPathsNum, sizeof(char *), alphaSort);
	for (int i = 0; i < assetPathsNum; i++) {
		char *path = assetPaths[i];
		if (strContains(path, ".png")) pngAssetPaths[pngAssetPathsNum++] = path;
	}
}

void loadRemoteZip(const char *url, const char *path) {
#if defined(__EMSCRIPTEN__)
	emscripten_async_wget(url, path, loadedRemoteFile, errorLoadingRemoteFile);
	remoteZipsLoading++;
#else
	Panic("You can't loadRemoteZip on this platform");
#endif
}

void loadedRemoteFile(const char *result) {
	int size;
	void *data = readFile(result, &size);
	openAndExtractZip(data, size);
	refreshAssetPaths();
	remoteZipsLoading--;
	if (remoteLoadingDoneCallback) remoteLoadingDoneCallback();
	logf("%s was loaded\n", result);
}

void errorLoadingRemoteFile(const char *result) {
	logf("error loading %s\n", result);
	remoteZipsLoading--;
}

#define DIRECTORY_LIST_LIMIT 4096
char **getDirectoryList(const char *dirPath, int *numFiles, bool includingUnderscored, bool shallow, bool includeFolders) {
	char realRootDir[PATH_MAX_LEN];
	bool usedPrefix;
	int pathOffset;
	if (dirPath[1] != ':') {
		strcpy(realRootDir, filePathPrefix); //@robustness Do this everywhere
		pathOffset = strlen(filePathPrefix);
		usedPrefix = true;
	} else {
		realRootDir[0] = 0;
		usedPrefix = false;
		pathOffset = 0;
	}
	strcat(realRootDir, dirPath);
	// logf("Root dir is %s\n", realRootDir);

	char **fileNames = (char **)malloc(FILE_LIST_LIMIT * sizeof(char *));
	int fileNamesNum = 0;

#if defined(_WIN32)
	char *dirNames[DIRECTORY_LIST_LIMIT];
	int dirNamesNum = 0;
	dirNames[dirNamesNum++] = stringClone(realRootDir);

	for (int i = 0; i < dirNamesNum; i++) {
		char realDirPath[PATH_MAX_LEN];
		strcpy(realDirPath, dirNames[i]);
		strcat(realDirPath, "/*");
		// logf("Scanning dir %s\n", realDirPath);

		WIN32_FIND_DATA file;
		HANDLE hFind = FindFirstFile(realDirPath, &file);

		if (hFind == INVALID_HANDLE_VALUE) {
			logf("Cannot access dir %s\n", realDirPath);
			Assert(0);
		}

		for (;;) {
			if (!streq(file.cFileName, ".") && !streq(file.cFileName, "..")) {
				if (file.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
					char *newDir = (char *)malloc(PATH_MAX_LEN);
					strcpy(newDir, realDirPath);
					newDir[strlen(newDir)-1] = 0;
					strcat(newDir, file.cFileName);
					if (strContains(newDir, "__") && !includingUnderscored) {
						// logf("Skipping dir %s\n", newDir);
						free(newDir);
					} else {
						// logf("Adding dir %s (%d)\n", newDir, strstr(newDir, "assets/raw"));
						if (!shallow) dirNames[dirNamesNum++] = newDir;
						if (includeFolders) fileNames[fileNamesNum++] = newDir;
					}
				} else {
					if (fileNamesNum == FILE_LIST_LIMIT-1) {
						logf("Exceeded file limit\n");
						Assert(0);
					}
					char *newFile = (char *)malloc(PATH_MAX_LEN);
					strcpy(newFile, realDirPath+pathOffset);
					newFile[strlen(newFile)-1] = 0;
					strcat(newFile, file.cFileName);
					fileNames[fileNamesNum++] = newFile;
					// logf("Adding file %s\n", newFile);
				}
			}

			BOOL found = FindNextFile(hFind, &file);
			if (!found) break;
		}

		FindClose(hFind);
	}
#elif defined(__linux__) || defined(__EMSCRIPTEN__)
	char *dirNames[DIRECTORY_LIST_LIMIT];
	int dirNamesNum = 0;
	dirNames[dirNamesNum++] = stringClone(realRootDir);

	for (int i = 0; i < dirNamesNum; i++) {
		char realDirName[PATH_MAX] = {};
		if (!stringStartsWith(dirNames[i], filePathPrefix)) strcpy(realDirName, filePathPrefix);
		strcat(realDirName, dirNames[i]);
		DIR *dir = opendir(realDirName);
		if (!dir) {
			logf("Failed to open dir %s\n", realDirName);
		}
		dirent *ent;

		for (;;) {
			ent = readdir(dir); 

			if (!ent) break;
			if (ent->d_name[0] == '.') continue;

			if (ent->d_type == DT_DIR) {
				char newDirName[PATH_MAX];
				strcpy(newDirName, realDirName+strlen(filePathPrefix));
				strcat(newDirName, "/");
				strcat(newDirName, ent->d_name);
				if (strContains(newDirName, "__") && !includingUnderscored) {
					// logf("Skipping dir %s\n", newDirName);
				} else {
					if (!shallow) dirNames[dirNamesNum++] = stringClone(newDirName);
					if (includeFolders) fileNames[fileNamesNum++] = newDirName;
				}
			} else {
				char newFileName[PATH_MAX];
				if (stringStartsWith(dirNames[i], filePathPrefix)) strcpy(newFileName, dirNames[i]+strlen(filePathPrefix));
				else strcpy(newFileName, dirNames[i]);
				strcat(newFileName, "/");
				strcat(newFileName, ent->d_name);

				fileNames[fileNamesNum++] = stringClone(newFileName);
			}
		}
	}
#else
	logf("Line: %d\n", __LINE__);
	Panic("Can't list directory on this OS\n");
#endif

	*numFiles = fileNamesNum;
	return fileNames;
}

char *resolveFuzzyPath(const char *fileName, const char *fileName2, FuzzyPathType type) {
	char *possiblePaths[FILE_LIST_LIMIT];
	int possiblePathsNum = 0;

	char **toSearch;
	int toSearchNum;

	if (type == FUZZY_ALL) {
		toSearch = assetPaths;
		toSearchNum = assetPathsNum;
	} else if (type == FUZZY_PNG) {
		toSearch = pngAssetPaths;
		toSearchNum = pngAssetPathsNum;
	}

	for (int i = 0; i < toSearchNum; i++) {
		char *path = toSearch[i];

		if (fileName2) {
			if (strContains(path, fileName) && strContains(path, fileName2)) possiblePaths[possiblePathsNum++] = path;
		} else {
			if (strContains(path, fileName)) possiblePaths[possiblePathsNum++] = path;
		}
	}

	if (possiblePathsNum > 0) {
		if (possiblePathsNum == 1) return possiblePaths[0];
		int choice = rndInt(0, possiblePathsNum-1);
		char *realPath = possiblePaths[choice];
		return realPath;
	} else {
		return NULL;
	}
}

bool directoryExists(const char *dirPath) {
	char realName[PATH_MAX_LEN] = {};
	if (dirPath[1] != ':' && dirPath[0] != '/') strcpy(realName, filePathPrefix);
	strcat(realName, dirPath);

#if defined(_WIN32)
	DWORD fileHandle = GetFileAttributesA(realName);
	if (fileHandle == INVALID_FILE_ATTRIBUTES) return false;
	if (fileHandle & FILE_ATTRIBUTE_DIRECTORY) return true;
	return false;
#elif defined(__linux__)
	DIR *pDir = opendir(realName);

	if (pDir) {
		closedir(pDir);
		return true;
	}

	return false;
#elif defined(__EMSCRIPTEN__)
	struct stat info;

	if (stat(realName, &info) != 0) {
		return false;
	} else if (info.st_mode & S_IFDIR) {
		return true;
	} else {
		return false;
	}
#else
	logf("Line: %d\n", __LINE__);
	Panic("Can't do this on this platform");
	return false;
#endif
}

bool fileExists(const char *fileName) {
	if (!fileName || !fileName[0]) return false;

	char realName[PATH_MAX_LEN] = {};
	if (fileName[1] != ':' && fileName[0] != '/') strcpy(realName, filePathPrefix);
	strcat(realName, fileName);

	if (strContains(fileName, "assets/raw/")) return false;

#if defined(_WIN32)
	HANDLE hFile = CreateFile(
		realName,
		GENERIC_READ,          // open for reading
		FILE_SHARE_READ,       // share for reading
		NULL,                  // default security
		OPEN_EXISTING,         // existing file only
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, // normal file
		NULL                   // no attr. template
	);

	if (hFile == INVALID_HANDLE_VALUE) return false;

	CloseHandle(hFile);
	return true;
#else
	FILE *filePtr = fopen(realName, "rb");
	if (!filePtr) return false;

	fclose(filePtr);
	return true;
#endif
}

bool isFirstPathNewer(const char *firstPath, const char *secondPath);
bool isFirstPathNewer(const char *firstPath, const char *secondPath) {
	char realFirstPath[PATH_MAX_LEN];
	if (firstPath[1] != ':' && firstPath[0] != '/') strcpy(realFirstPath, filePathPrefix);
	strcat(realFirstPath, firstPath);

	char realSecondPath[PATH_MAX_LEN];
	if (secondPath[1] != ':' && secondPath[0] != '/') strcpy(realSecondPath, filePathPrefix);
	strcat(realSecondPath, secondPath);

#if defined(_WIN32)
		HANDLE inHandle = CreateFile(realFirstPath, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		HANDLE outHandle = CreateFile(realSecondPath, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

		FILETIME creationTime;
		FILETIME lastAccessTime;
		FILETIME firstLastWriteTime;
		FILETIME secondLastWriteTime;
		BOOL good = GetFileTime(inHandle, &creationTime, &lastAccessTime, &firstLastWriteTime);
		if (!good) {
			logf("Failed to get in file time %s\n", realFirstPath);
			logLastOSError();
		}

		good = GetFileTime(outHandle, &creationTime, &lastAccessTime, &secondLastWriteTime);
		if (!good) {
			logf("Failed to get out file time %s\n", realSecondPath);
			logLastOSError();
		}

		CloseHandle(inHandle);
		CloseHandle(outHandle);
		LONG comparison = CompareFileTime(&firstLastWriteTime, &secondLastWriteTime);
		if (comparison == 1) return true;
		return false;
#else
	Panic("Can't run isFirstPathNewer outside windows\n");
#endif

	return false;
}

bool copyFile(const char *srcPath, const char *destPath);
bool copyFile(const char *srcPath, const char *destPath) {
	char realSrcPath[PATH_MAX_LEN] = {};
	if (srcPath[1] != ':' && srcPath[0] != '/') strcpy(realSrcPath, filePathPrefix);
	strcat(realSrcPath, srcPath);

	char realDestPath[PATH_MAX_LEN] = {};
	if (destPath[1] != ':' && destPath[0] != '/') strcpy(realDestPath, filePathPrefix);
	strcat(realDestPath, destPath);

#if defined(_WIN32)
	BOOL good = CopyFile(realSrcPath, realDestPath, false);
	if (!good) {
		logf("Failed to copy file %s -> %s\n", realSrcPath, realDestPath);
		logLastOSError();
		return false;
	}
#else
	Panic("Can't run copyFile outside windows\n");
#endif

	return true;
}

bool createDirectory(const char *dirName) {
	if (directoryExists(dirName)) return true;
	char realName[PATH_MAX_LEN] = {};
	// strcpy(realName, exeDir);
	// strcat(realName, "/");
	if (dirName[1] != ':' && dirName[0] != '/') strcpy(realName, filePathPrefix);
	strcat(realName, dirName);

#if defined(_WIN32)
	int good = CreateDirectoryA(realName, NULL);
	if (!good) return false;
	return true;
#else
	int good = mkdir(realName, 0777);
	if (!good) return false;
	return true;
#endif
}

void removeDirectory(const char *dirName) {
	if (!directoryExists(dirName)) return;

	char realDir[PATH_MAX_LEN] = {};
	if (dirName[1] != ':') strcpy(realDir, filePathPrefix);
	strcat(realDir, dirName);

#if defined(_WIN32)
	char *command = frameSprintf("rmdir /s /q \"%s\"", realDir);
	// logf("Command: %s\n", command);
	system(command);
	// logf("Line: %d\n", __LINE__);
	// Panic("Can't do this on this platform");
#else
	logf("Line: %d\n", __LINE__);
	logf("Can't do this on this platform");
#endif
}

#if defined(_WIN32)
BOOL IsDots(const TCHAR* str) {
   if(_tcscmp(str,".") && _tcscmp(str,"..")) return FALSE;
   return TRUE;
}

BOOL recurseRemoveDirectory(const TCHAR* sPath) {
	HANDLE hFind;    // file handle
	WIN32_FIND_DATA FindFileData;

	TCHAR DirPath[MAX_PATH];
	TCHAR FileName[MAX_PATH];

	_tcscpy(DirPath,sPath);
	_tcscat(DirPath,"\\*");    // searching all files
	_tcscpy(FileName,sPath);
	_tcscat(FileName,"\\");

	// find the first file
	hFind = FindFirstFile(DirPath,&FindFileData);
	if(hFind == INVALID_HANDLE_VALUE) return FALSE;
	_tcscpy(DirPath,FileName);

	bool bSearch = true;
	while(bSearch) {    // until we find an entry
		if(FindNextFile(hFind,&FindFileData)) {
			if(IsDots(FindFileData.cFileName)) continue;
			_tcscat(FileName,FindFileData.cFileName);
			if((FindFileData.dwFileAttributes &
				 FILE_ATTRIBUTE_DIRECTORY)) {

				// we have found a directory, recurse
				if(!recurseRemoveDirectory(FileName)) {
					FindClose(hFind);
					return FALSE;    // directory couldn't be deleted
				}
				// remove the empty directory
				RemoveDirectory(FileName);
				_tcscpy(FileName,DirPath);
			}
			else {
				if(FindFileData.dwFileAttributes &
				 FILE_ATTRIBUTE_READONLY)
					// change read-only file mode
					chmod(FileName, _S_IWRITE);
				if(!DeleteFile(FileName)) {    // delete the file
					FindClose(hFind);
					return FALSE;
				}
				_tcscpy(FileName,DirPath);
			}
		}
		else {
			// no more files there
			if(GetLastError() == ERROR_NO_MORE_FILES)
				bSearch = false;
			else {
				// some error occurred; close the handle and return FALSE
				FindClose(hFind);
				return FALSE;
			}

		}

	}
	FindClose(hFind);                  // close the file handle

	return RemoveDirectory(sPath);     // remove the empty directory
}
#endif

void renameFile(const char *currentName, const char *newName) {
	char realCurrentName[PATH_MAX_LEN];
	strcpy(realCurrentName, filePathPrefix);
	strcat(realCurrentName, currentName);

	char realNewName[PATH_MAX_LEN];
	strcpy(realNewName, filePathPrefix);
	strcat(realNewName, newName);

#if defined(_WIN32)
	if (!directoryExists(realCurrentName)) logf("%s doesn't even exist\n", realCurrentName);
	BOOL good = MoveFile(realCurrentName, realNewName);
	if (!good) logf("Failed to rename file %s -> %s\n", realCurrentName, realNewName);

#else
	logf("Line: %d\n", __LINE__);
	Panic("Can't do this on this platform");
#endif
}

bool writeFile(const char *fileName, void *data, int length) {
	char realName[PATH_MAX_LEN] = {};
	if (fileName[1] != ':' && fileName[0] != '/') strcpy(realName, filePathPrefix);
	strcat(realName, fileName);

#if defined(_WIN32)
	HANDLE hFile = CreateFile(
		realName,
		GENERIC_WRITE,          // open for writing
		0,                      // do not share
		NULL,                   // default security
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,  // normal file
		NULL                    // no attr. template
	);

	if (!hFile || hFile == INVALID_HANDLE_VALUE) { 
#if !defined(FALLOW_DEBUG)
		void showErrorWindow(char *msg); //@headerHack
		showErrorWindow(frameSprintf("Cannot write file %s\n", realName));
#endif
		logf("Cannot find file %s to write. (Maybe because you're saving too fast)\n", realName);
		logLastOSError();
		return false;
	}

	DWORD written;
	bool good = WriteFile( 
		hFile,
		data,
		length,
		&written,
		NULL
	);

	if (!good || written != length) {
#if !defined(FALLOW_DEBUG)
		void showErrorWindow(char *msg); //@headerHack
		showErrorWindow(frameSprintf("Failed to write file %s\n", realName));
		return false;
#endif
	}

	CloseHandle(hFile);
	return true;
#else
	FILE *filePtr = fopen(realName, "wb");
	if (!filePtr) {
		logf("Cannot find file %s\n", realName);
		printf("Why: %s\n", strerror(errno));
		Assert(0);
		return false;
	}

	fwrite(data, length, 1, filePtr);
	fclose(filePtr);
	return true;
#endif
}

void appendFile(const char *fileName, void *data, int length) {
	char realName[PATH_MAX_LEN];
	if (fileName[1] != ':' && fileName[0] != '/') strcpy(realName, filePathPrefix);
	strcat(realName, fileName);

	FILE *filePtr = fopen(realName, "a");
	if (!filePtr) {
		logf("Cannot find file %s\n", realName);
		Assert(0);
	}

	fwrite(data, length, 1, filePtr);
	fclose(filePtr);
}

bool readFileDirect(const char *fileName, u8 *outData, int outDataSize, int *outSize);
bool readFileDirect(const char *fileName, u8 *outData, int outDataSize, int *outSize) {
	char realName[PATH_MAX_LEN] = {};
	if (fileName[1] != ':' && fileName[0] != '/') strcpy(realName, filePathPrefix);
	strcat(realName, fileName);

	FILE *filePtr = fopen(realName, "rb");
	if (!filePtr) {
		logf("Cannot find file %s\n", realName);
		Assert(0);
	}

	fseek(filePtr, 0, SEEK_END);
	long fileSize = ftell(filePtr);
	fseek(filePtr, 0, SEEK_SET);

	if (fileSize+1 > outDataSize) {
		logf("readFileDirect failed, need %d bytes, but only have a max of %d", fileSize, outDataSize);
		return false;
	}

	fread(outData, fileSize, 1, filePtr);
	fclose(filePtr);

	outData[fileSize] = 0; 
	if (outSize) *outSize = fileSize;
	return true;
}

void *readFile(const char *fileName, int *outSize) {
	char realName[PATH_MAX_LEN] = {};
	if (fileName[1] != ':' && fileName[0] != '/') strcpy(realName, filePathPrefix);
	strcat(realName, fileName);

// #if defined(_WIN32)
// 	HANDLE hFile = CreateFile(
// 		realName,
// 		GENERIC_READ,          // open for reading
// 		FILE_SHARE_READ,       // share for reading
// 		NULL,                  // default security
// 		OPEN_EXISTING,         // existing file only
// 		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, // normal file
// 		NULL                   // no attr. template
// 	);

// 	if (hFile == INVALID_HANDLE_VALUE) { 
// 		logf("Cannot find file %s\n", realName);
// 		Assert(0);
// 	}

// 	int fileSize = GetFileSize(hFile, NULL);

// 	char *str = (char *)malloc(fileSize + 1);

// 	OVERLAPPED ol = {0};
// 	bool good = ReadFileEx(hFile, str, fileSize, &ol, NULL);

// 	CloseHandle(hFile);
// #else
	FILE *filePtr = fopen(realName, "rb");
	if (!filePtr) {
		logf("Cannot find file %s\n", realName);
		Assert(0);
	}

	fseek(filePtr, 0, SEEK_END);
	long fileSize = ftell(filePtr);
	fseek(filePtr, 0, SEEK_SET);

	char *str = (char *)malloc(fileSize + 1);
	fread(str, fileSize, 1, filePtr);
	fclose(filePtr);
// #endif

	str[fileSize] = 0; 
	if (outSize) *outSize = fileSize;
	return str;
}

void deleteFile(const char *fileName) {
	char realName[PATH_MAX_LEN];
	strcpy(realName, filePathPrefix);
	strcat(realName, fileName);

#if defined(_WIN32)
	BOOL good = DeleteFileA(realName);

	if (!good) {
		logf("Failed to delete %s\n", realName);
	}
#else
	if(remove(realName) != 0 ) {
		logf("Failed to delete %s\n", realName);
	}
#endif
}

char *readTempSave() {
	if (cantSaveTempFiles) return NULL;

#if defined(__EMSCRIPTEN__)
	int bytesNeeded = EM_ASM_INT(
		if (!localStorage.saveData) return 0;
		return localStorage.saveData.length*4+1;
	);
	if (bytesNeeded == 0) return NULL;

	char *str = (char *)zalloc(bytesNeeded);
	EM_ASM(stringToUTF8(localStorage.saveData, $0, localStorage.saveData.length*4+1) , str);
	return str;
#else
	char *path = frameSprintf("%s/save.dat", exeDir);
	if (!fileExists(path)) return NULL;
	char *str = (char *)readFile(path);
	return str;
#endif
}

bool writeTempSave(char *str) {
	if (cantSaveTempFiles) return false;

#if defined(__EMSCRIPTEN__)
	EM_ASM(localStorage.saveData = UTF8ToString($0), str);
	return true;
#else
	writeFile(frameSprintf("%s/save.dat", exeDir), str, strlen(str));
	return true;
#endif
}
