char *platformRun(const char *cmd, bool blocks);
char *saveFileForUser(char *fileName, void *data, int dataNum);

char *platformRun(const char *cmd, bool blocks) {
#if defined(_WIN32)
	SECURITY_ATTRIBUTES saAttr; 
	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES); 
	saAttr.lpSecurityDescriptor = NULL; 
	saAttr.bInheritHandle = TRUE; 
	STARTUPINFO startupInfo = {};
	startupInfo.cb = sizeof(startupInfo);

	PROCESS_INFORMATION processInfo = {};

	// HANDLE outFile = NULL;
	// char *outFilePath = frameSprintf("%s/assets/subProcessStdOut.txt", filePathPrefix);
	// if (blocks) {
	// 	outFile = CreateFile(
	// 		outFilePath,
	// 		GENERIC_WRITE,
	// 		FILE_SHARE_WRITE | FILE_SHARE_READ,
	// 		&saAttr,
	// 		CREATE_ALWAYS,
	// 		FILE_ATTRIBUTE_NORMAL,  // normal file
	// 		NULL                    // no attr. template
	// 	);

	// 	if (!outFile || outFile == INVALID_HANDLE_VALUE) { 
	// 		logf("Could not create subProcessStdOut.txt\n");
	// 		logLastOSError();
	// 		return NULL;
	// 	}

	// 	startupInfo.hStdError = outFile;
	// 	startupInfo.hStdOutput = outFile;
	// 	startupInfo.hStdInput = NULL;
	// 	startupInfo.dwFlags |= STARTF_USESTDHANDLES;
	// }

	// char *realCmd = frameSprintf("C:\\Windows\\System32\\cmd.exe /c %s", cmd);
	char *realCmd = (char *)cmd;
	// logf("real: %s\n", realCmd);

	BOOL good = CreateProcess(
		NULL,                    // the command
		realCmd,                 // Command line
		NULL,                    // Process handle not inheritable
		NULL,                    // Thread handle not inheritable
		TRUE,                    // Set handle inheritance to FALSE(?)
		CREATE_NO_WINDOW,
		NULL,                    // Use parent's environment block
		NULL,                    // Use parent's starting directory 
		&startupInfo,            // Pointer to STARTUPINFO structure
		&processInfo             // Pointer to PROCESS_INFORMATION structure (removed extra parentheses)
	);
	if (!good) {
		logf("Failed to get run command: %s\n", cmd);
		logLastOSError();
	}

	if (blocks) {
		WaitForSingleObject(processInfo.hProcess, INFINITE);

		CloseHandle(processInfo.hProcess);
		CloseHandle(processInfo.hThread);
		// CloseHandle(outFile);
		// if (fileExists("assets/subProcessStdOut.txt")) {
		// 	void *data = readFile("assets/subProcessStdOut.txt");
		// 	return (char *)data;
		// } else {
		// 	logf("Failed to get return data from command: %s\n", cmd);
		// 	logLastOSError();
		// 	return NULL;
		// }
	}
	return NULL;
#else
	logf("Can't platformRun on this platform\n");
	return NULL;
#endif
}

char *saveFileForUser(char *fileName, void *data, int dataNum) {
#ifdef __EMSCRIPTEN__
	EM_ASM({
		let fileNamePtr = $0;
		let dataPtr = $1;
		let dataSize = $2;

		let fileName = UTF8ToString(fileNamePtr);

		let dataArray = new Uint8Array(dataSize);
		for (let i = 0; i < dataSize; i++) {
			dataArray[i] = getValue(dataPtr + i, "i8");
		}
		let blob = new Blob([dataArray], {type: "octet/stream"});

		let url = window.URL.createObjectURL(blob);

		var link = document.createElement('a');
		link.href = url;
		link.download = fileName;
		document.body.appendChild(link);
		link.click();
		document.body.removeChild(link);
	}, fileName, data, dataNum);
	return NULL;
#else
	char *path = frameSprintf("%s/%s", exeDir, fileName);
	if (writeFile(path, data, dataNum)) {
		for (char *p = path; *p; p++) if (*p == '\\') *p = '/';
		return path;
	} else {
		return NULL;
	}
#endif
}
