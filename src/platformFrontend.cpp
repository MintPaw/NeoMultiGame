#if defined(_WIN32) && defined(FALLOW_DEBUG)
#include <dbghelp.h>
#endif

#include "guiUtils.cpp"

enum KeyState {
	KEY_STATE_RELEASED,
	KEY_STATE_PRESSED,
	KEY_STATE_JUST_PRESSED,
	KEY_STATE_JUST_RELEASED,
};

struct NanoTime {
  BackendNanoTime backendNanoTime;
};

struct Platform {
	bool isDebugVersion;
	bool isInternalVersion;

	int windowWidth;
	int windowHeight;
	float windowScaling;

	float elapsed;
	Vec2 mouse;
	int mouseWheel;

  KeyState keys[KEYS_MAX];

	float time;
	int frameCount;

	NanoTime frameNano;

#define PLATFORM_FRAME_TIMES_MAX 60
  float frameTimes[PLATFORM_FRAME_TIMES_MAX];
	float frameTimeAvg;
	float frameTimeHighest;

	int memoryUsage;

#define PLATFORM_INPUT_CHARACTERS_MAX 128
	char inputCharacters[PLATFORM_INPUT_CHARACTERS_MAX];
	int inputCharactersNum;

	void (*gameUpdateCallback)();
};

Platform *platform = NULL;

bool _imGuiHoveringGui;
bool _imGuiTypingGui;
bool _imGuiNeedToDrawThisFrame;
ImGuiMouseCursor _imGuiCurrentMouseCursor;

void initPlatform(int windowWidth, int windowHeight, char *windowTitle);

void platformUpdateLoop(void (*gameUpdateCallback)());
void platformUpdate();

void maximizeWindow() { backendPlatformMaximizeWindow(); }
void minimizeWindow() { backendPlatformMaximizeWindow(); }
void restoreWindow() { backendPlatformMaximizeWindow(); }
void hideCursor() { backendHideCursor(); }
void showCursor() { backendShowCursor(); }

bool keyPressed(int key, bool ignoreImGuiTest=false);
bool keyJustPressed(int key, bool ignoreImGuiTest=false);
bool keyJustReleased(int key, bool ignoreImGuiTest=false);
void setClipboard(char *str);
void resizeWindow(int width, int height);
void platformSleep(int ms);
NanoTime getNanoTime();
float getMsPassed(NanoTime startTime);
void navigateToUrl(char *url);
void showErrorWindow(char *msg);
void logLastOSErrorCode(const char *fileName, int lineNum);

// Internal:
void imGuiInit();
void imGuiStartFrame();
void imGuiDraw();

#if defined(_WIN32) && defined(FALLOW_DEBUG)
LONG CALLBACK win32ExceptionHandler(EXCEPTION_POINTERS *e);
#endif

/// FUNCTIONS ^

void initPlatform(int windowWidth, int windowHeight, char *windowTitle) {
	platform = (Platform *)zalloc(sizeof(Platform));
	platform->windowWidth = windowWidth;
	platform->windowHeight = windowHeight;

#if defined(FALLOW_DEBUG)
	platform->isDebugVersion = true;
#endif

#if defined(_WIN32) && defined(FALLOW_DEBUG)
	SetUnhandledExceptionFilter(win32ExceptionHandler);
#endif

#if defined(FALLOW_INTERNAL)
	platform->isInternalVersion = true;
#endif

  backendPlatformInit(platform->windowWidth, platform->windowHeight, windowTitle);
	platform->windowScaling = backendPlatformGetWindowScaling();
	pushRndSeed(time(NULL));

	imGuiInit();
}

void platformUpdateLoop(void (*gameUpdateCallback)()) {
	platform->gameUpdateCallback = gameUpdateCallback;

  backendPlatformStartUpdateLoop(platformUpdate);
}

void platformUpdate() {
  platform->elapsed = 1/60.0;

	platform->frameNano = getNanoTime();

	platform->windowWidth = backendPlatformGetWindowWidth();
	platform->windowHeight = backendPlatformGetWindowHeight();

	for (int i = 0; i < KEYS_MAX; i++) {
		if (platform->keys[i] == KEY_STATE_JUST_PRESSED) platform->keys[i] = KEY_STATE_PRESSED;
		if (platform->keys[i] == KEY_STATE_JUST_RELEASED) platform->keys[i] = KEY_STATE_RELEASED;
	}
	platform->mouseWheel = 0;
	platform->inputCharactersNum = 0;

	for (int i = 0; i < _platformEventsNum; i++) {
		PlatformEvent *event = &_platformEvents[i];
		if (event->type == PLATFORM_EVENT_KEY_DOWN) {
			platform->keys[event->keyCode] = KEY_STATE_JUST_PRESSED;
		} else if (event->type == PLATFORM_EVENT_KEY_UP) {
			platform->keys[event->keyCode] = KEY_STATE_JUST_RELEASED;
		} else if (event->type == PLATFORM_EVENT_MOUSE_MOVE) {
			platform->mouse = event->position;
		} else if (event->type == PLATFORM_EVENT_MOUSE_WHEEL) {
			platform->mouseWheel = event->wheelValue;
		} else if (event->type == PLATFORM_EVENT_INPUT_CHARACTER) {
			if (platform->inputCharactersNum > PLATFORM_INPUT_CHARACTERS_MAX-1) platform->inputCharactersNum--;
			platform->inputCharacters[platform->inputCharactersNum++] = event->keyCode;
		}
	}
	_platformEventsNum = 0; //@incomplete backendPlatformGetEvents()

	void updateAudio(); //@headerHack
	updateAudio();

	void startRenderingFrame(); //@headerHack
	startRenderingFrame();

	imGuiStartFrame();

	void nguiStartFrame(); //@headerHack
	nguiStartFrame();

	platform->gameUpdateCallback();

  imGuiDraw();

	void endRenderingFrame(); //@headerHack
	endRenderingFrame();

	if (keyPressed(KEY_CTRL) && keyPressed('Q')) backendPlatformExit();

	{ // Calcuate frame times
		platform->frameTimes[platform->frameCount % PLATFORM_FRAME_TIMES_MAX] = getMsPassed(platform->frameNano);

		platform->frameTimeAvg = 0;
		platform->frameTimeHighest = 0;
		for (int i = 0; i < PLATFORM_FRAME_TIMES_MAX; i++) {
			platform->frameTimeAvg += platform->frameTimes[i];
			if (platform->frameTimeHighest < platform->frameTimes[i]) platform->frameTimeHighest = platform->frameTimes[i];
		}
		platform->frameTimeAvg /= (float)PLATFORM_FRAME_TIMES_MAX;
	}

  platform->memoryUsage = backendPlatformGetMemoryUsage();

	platform->frameCount++;
	platform->time += platform->elapsed;
	if (logSys) logSys->time = platform->time;

	freeFrameMemory();
}

void setClipboard(char *str) {
  backendPlatformSetClipboard(str);
}

void resizeWindow(int width, int height) {
	backendPlatformResizeWindow(width, height);
}

bool keyPressed(int key, bool ignoreImGuiTest) {
  if (!ignoreImGuiTest) {
    if (_imGuiTypingGui) return false;
    if ((key == MOUSE_LEFT || key == MOUSE_RIGHT) && _imGuiHoveringGui) return false;
  }
  return platform->keys[key] == KEY_STATE_PRESSED;
}

bool keyJustPressed(int key, bool ignoreImGuiTest) {
  if (!ignoreImGuiTest) {
    if (_imGuiTypingGui) return false;
    if ((key == MOUSE_LEFT || key == MOUSE_RIGHT) && _imGuiHoveringGui) return false;
  }
  return platform->keys[key] == KEY_STATE_JUST_PRESSED;
}

bool keyJustReleased(int key, bool ignoreImGuiTest) {
  if (!ignoreImGuiTest) {
    if (_imGuiTypingGui) return false;
    if ((key == MOUSE_LEFT || key == MOUSE_RIGHT) && _imGuiHoveringGui) return false;
  }
  return platform->keys[key] == KEY_STATE_JUST_RELEASED;
}

void platformSleep(int ms) {
  backendPlatformSleep(ms);
}

NanoTime getNanoTime() {
	NanoTime time = {};
	time.backendNanoTime = backendPlatformGetNanoTime();
	return time;
}

float getMsPassed(NanoTime startTime) {
  return backendPlatformGetMsPassed(startTime.backendNanoTime);
}

void navigateToUrl(char *url) {
  backendPlatformNavigateToUrl(url);
}

void showErrorWindow(char *msg) {
	logf("Error window: %s\n", msg);
  backendPlatformShowErrorWindow(msg);
}

void logLastOSErrorCode(const char *fileName, int lineNum) {
  char *msg = backendPlatformGetLastErrorMessage();
	logf("Error (%s:%d): %s\n", fileName, lineNum, msg);
}

void imGuiInit() {
#if defined(FALLOW_COMMAND_LINE_ONLY)
  return;
#endif

	_imGuiCurrentMouseCursor = ImGuiMouseCursor_COUNT;

	ImGui::CreateContext(NULL);

	ImGui::StyleColorsDark();

	ImGuiIO& io = ImGui::GetIO();
	io.BackendPlatformName = "imGui_impl_custom_mix";
	io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;

	io.KeyMap[ImGuiKey_Tab] = KEY_TAB;
	io.KeyMap[ImGuiKey_LeftArrow] = KEY_LEFT;
	io.KeyMap[ImGuiKey_RightArrow] = KEY_RIGHT;
	io.KeyMap[ImGuiKey_UpArrow] = KEY_UP;
	io.KeyMap[ImGuiKey_DownArrow] = KEY_DOWN;
	io.KeyMap[ImGuiKey_PageUp] = KEY_PAGE_UP;
	io.KeyMap[ImGuiKey_PageDown] = KEY_PAGE_DOWN;
	io.KeyMap[ImGuiKey_Home] = KEY_HOME;
	io.KeyMap[ImGuiKey_End] = KEY_END;
	io.KeyMap[ImGuiKey_Insert] = KEY_INSERT;
	io.KeyMap[ImGuiKey_Delete] = KEY_DELETE;
	io.KeyMap[ImGuiKey_Backspace] = KEY_BACKSPACE;
	io.KeyMap[ImGuiKey_Space] = ' ';
	io.KeyMap[ImGuiKey_Enter] = KEY_ENTER;
	io.KeyMap[ImGuiKey_Escape] = KEY_ESC;
	// io.KeyMap[ImGuiKey_KeyPadEnter] = KEY_KP_ENTER;
	io.KeyMap[ImGuiKey_A] = 'A';
	io.KeyMap[ImGuiKey_C] = 'C';
	io.KeyMap[ImGuiKey_V] = 'V';
	io.KeyMap[ImGuiKey_X] = 'X';
	io.KeyMap[ImGuiKey_Y] = 'Y';
	io.KeyMap[ImGuiKey_Z] = 'Z';

	io.MousePos = ImVec2(0, 0);

	ImGui::GetStyle().ScaleAllSizes(platform->windowScaling);
	io.FontGlobalScale *= platform->windowScaling;

	ImFont *imFont;
	{
		int ttfDataSize;
		void *ttfData = readFile("assets/common/arial.ttf", &ttfDataSize);
		ImGuiIO &io = ImGui::GetIO();
		imFont = io.Fonts->AddFontFromMemoryTTF(ttfData, ttfDataSize, 13);
		io.Fonts->Build();
	}

	backendPlatformImGuiInit();
}

void imGuiStartFrame() {
#if defined(FALLOW_COMMAND_LINE_ONLY)
	return;
#endif

	_imGuiNeedToDrawThisFrame = true;
	_imGuiHoveringGui = false;
	_imGuiTypingGui = false;

	{ /// rlImGuiNewFrame();
		ImGuiIO& io = ImGui::GetIO();

		io.DisplaySize.x = platform->windowWidth;
		io.DisplaySize.y = platform->windowHeight;

		io.DeltaTime = platform->elapsed;

		io.KeyCtrl = keyPressed(KEY_CTRL) || keyPressed(KEY_RIGHT_CTRL);
		io.KeyShift = keyPressed(KEY_SHIFT) || keyPressed(KEY_RIGHT_SHIFT);
		io.KeyAlt = keyPressed(KEY_ALT) || keyPressed(KEY_RIGHT_ALT);

		if (io.WantSetMousePos) {
			// Raylib::SetMousePosition((int)io.MousePos.x, (int)io.MousePos.y); //@todo Why?
		} else {
			io.MousePos.x = platform->mouse.x;
			io.MousePos.y = platform->mouse.y;
		}

		io.MouseDown[0] = keyPressed(MOUSE_LEFT);
		io.MouseDown[1] = keyPressed(MOUSE_RIGHT);
		io.MouseDown[2] = keyPressed(MOUSE_MIDDLE);

		io.MouseWheel = platform->mouseWheel;

		if ((io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange) == 0) {
			ImGuiMouseCursor imgui_cursor = ImGui::GetMouseCursor();
			if (imgui_cursor != _imGuiCurrentMouseCursor || io.MouseDrawCursor) {
				_imGuiCurrentMouseCursor = imgui_cursor;
				if (io.MouseDrawCursor || imgui_cursor == ImGuiMouseCursor_None) {
					hideCursor();
				} else {
					showCursor();

					if (!(io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange)) {
						// if (imgui_cursor == ImGuiMouseCursor_Arrow) Raylib::SetMouseCursor(Raylib::MOUSE_CURSOR_ARROW); //@todo how?
						// else if (imgui_cursor == ImGuiMouseCursor_TextInput) Raylib::SetMouseCursor(Raylib::MOUSE_CURSOR_IBEAM);
						// else if (imgui_cursor == ImGuiMouseCursor_Hand) Raylib::SetMouseCursor(Raylib::MOUSE_CURSOR_POINTING_HAND);
						// else if (imgui_cursor == ImGuiMouseCursor_ResizeAll) Raylib::SetMouseCursor(Raylib::MOUSE_CURSOR_RESIZE_ALL);
						// else if (imgui_cursor == ImGuiMouseCursor_ResizeEW) Raylib::SetMouseCursor(Raylib::MOUSE_CURSOR_RESIZE_EW);
						// else if (imgui_cursor == ImGuiMouseCursor_ResizeNESW) Raylib::SetMouseCursor(Raylib::MOUSE_CURSOR_RESIZE_NESW);
						// else if (imgui_cursor == ImGuiMouseCursor_ResizeNS) Raylib::SetMouseCursor(Raylib::MOUSE_CURSOR_RESIZE_NS);
						// else if (imgui_cursor == ImGuiMouseCursor_ResizeNWSE) Raylib::SetMouseCursor(Raylib::MOUSE_CURSOR_RESIZE_NWSE);
						// else if (imgui_cursor == ImGuiMouseCursor_NotAllowed) Raylib::SetMouseCursor(Raylib::MOUSE_CURSOR_NOT_ALLOWED);
						// else Raylib::SetMouseCursor(Raylib::MOUSE_CURSOR_DEFAULT);
					}
				}
			}
		}

		{ /// rlImGuiEvents();
			ImGuiIO& io = ImGui::GetIO();
			for (int i = 0; i < ArrayLength(_keysThatImGuiCaresAbout); i++) {
				int key = _keysThatImGuiCaresAbout[i];
				io.KeysDown[key] = keyPressed(key);
			}

			for (int i = 0; i < platform->inputCharactersNum; i++) {
				io.AddInputCharacter(platform->inputCharacters[i]);
			}

			_imGuiHoveringGui = io.WantCaptureMouse;
			_imGuiTypingGui = io.WantTextInput;
		}
	}

	backendPlatformImGuiStartFrame(platform->windowWidth, platform->windowHeight);
	ImGui::NewFrame();
}

void imGuiDraw() {
#if defined(FALLOW_COMMAND_LINE_ONLY)
	return;
#endif

	if (!_imGuiNeedToDrawThisFrame) return;
	_imGuiNeedToDrawThisFrame = false;

	ImGui::Render();
  backendPlatformImGuiDraw();
}

#if defined(_WIN32) && defined(FALLOW_DEBUG)
LONG CALLBACK win32ExceptionHandler(EXCEPTION_POINTERS* e) {
	auto hDbgHelp = LoadLibraryA("dbghelp");
	if (!hDbgHelp) return EXCEPTION_CONTINUE_SEARCH;

	auto pMiniDumpWriteDump = (decltype(&MiniDumpWriteDump))GetProcAddress(hDbgHelp, "MiniDumpWriteDump");
	if (!pMiniDumpWriteDump) return EXCEPTION_CONTINUE_SEARCH;

	char name[MAX_PATH];
	{
		auto nameEnd = name + GetModuleFileNameA(GetModuleHandleA(0), name, MAX_PATH);
		SYSTEMTIME t;
		GetSystemTime(&t);
		wsprintfA(nameEnd - strlen(".exe"), "_%4d%02d%02d_%02d%02d%02d.dmp", t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond);
	}

	auto hFile = CreateFileA(name, GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile == INVALID_HANDLE_VALUE) return EXCEPTION_CONTINUE_SEARCH;

	MINIDUMP_EXCEPTION_INFORMATION exceptionInfo;
	exceptionInfo.ThreadId = GetCurrentThreadId();
	exceptionInfo.ExceptionPointers = e;
	exceptionInfo.ClientPointers = FALSE;

	auto dumped = pMiniDumpWriteDump(
		GetCurrentProcess(),
		GetCurrentProcessId(),
		hFile,
		MINIDUMP_TYPE(MiniDumpWithIndirectlyReferencedMemory | MiniDumpScanMemory),
		e ? &exceptionInfo : nullptr,
		nullptr,
		nullptr);

	CloseHandle(hFile);

	return EXCEPTION_CONTINUE_SEARCH;
}
#endif


///- Audio

// #define DO_CUSTOM_AUDIO_STREAMS 1

// struct Channel;

// struct Sound {
// 	Channel *channel;

// 	u8 *oggData;
// 	int oggDataLen;
// 	Raylib::Music raylibMusic;
// 	float length;

// 	bool exists;
// 	char *path;

// 	float tweakVolume;

// 	stb_vorbis *vorbis;
// 	int frameCount;
// 	Raylib::AudioStream raylibStream;
// };

// struct Channel {
// 	Sound *sound;

// 	bool exists;
// 	int id;

// 	float secondPosition;

// 	float userVolume;
// 	float userVolume2;

// 	bool looping;
// };

// struct Audio {
// 	bool doInstantVolumeChanges; // Does nothing?

// 	float masterVolume;

// #define SOUNDS_MAX 4096
// 	char soundStoreNames[SOUNDS_MAX][PATH_MAX_LEN];
// 	int soundStoreNamesNum;
// 	Sound *soundStore[SOUNDS_MAX];
// 	int soundStoreNum;

// 	Sound sounds[SOUNDS_MAX];

// #define CHANNELS_MAX 512
// 	Channel channels[CHANNELS_MAX];
// 	int nextChannelId;
// 	int channelsCount;

// 	int memoryUsed;
// };

// Audio *audio = NULL;

// void initAudio();
// void updateAudio();
// Sound *getSound(const char *path, bool onlyLoadRaw=false);
// void initSound(Sound *sound);
// Channel *getChannel(int id);
// Channel *playSound(Sound *sound, bool looping=false);
// void stopChannel(int channelId);
// void stopChannel(Channel *channel);
// void seekChannelPerc(Channel *channel, float perc);

// void initAudio() {
// 	Raylib::InitAudioDevice();

// 	audio = (Audio *)zalloc(sizeof(Audio));
// 	audio->masterVolume = 1;
// }

// void updateAudio() {
// 	for (int i = 0; i < CHANNELS_MAX; i++) {
// 		Channel *channel = &audio->channels[i];
// 		if (!channel->exists) {
// 			continue;
// 		}
// 		if (!Raylib::IsMusicStreamPlaying(channel->sound->raylibMusic)) {
// 			stopChannel(channel);
// 			continue;
// 		}

// 		channel->secondPosition = Raylib::GetMusicTimePlayed(channel->sound->raylibMusic);
// 		channel->sound->raylibMusic.looping = channel->looping;

// 		float vol;
// 		{ // computeChannelVolume
// 			float tweak = 1;
// 			if (channel->sound) tweak = channel->sound->tweakVolume;
// 			vol = tweak * channel->userVolume * channel->userVolume2 * audio->masterVolume;
// 			vol = vol*vol;
// 		}

// 		Raylib::SetMusicVolume(channel->sound->raylibMusic, vol);

// 		Raylib::UpdateMusicStream(channel->sound->raylibMusic);
// 	}
// }

// Sound *getSound(const char *path, bool onlyLoadRaw) {
// 	/// Look in the store for a sound
// 	for (int i = 0; i < audio->soundStoreNamesNum; i++) {
// 		char *soundName = audio->soundStoreNames[i];
// 		if (streq(soundName, path)) {
// 			Sound *sound = audio->soundStore[i];
// 			if (!onlyLoadRaw && sound->raylibMusic.frameCount == 0) initSound(sound);
// 			return sound;
// 		}
// 	}

// 	/// Not in store, look for file
// 	if (fileExists(path)) {
// 		Sound *sound = NULL;
// 		for (int i = 0; i < SOUNDS_MAX; i++) {
// 			if (!audio->sounds[i].exists) {
// 				sound = &audio->sounds[i];
// 				break;
// 			}
// 		}

// 		if (!sound) {
// 			logf("There are no more sound slots\n");
// 			Assert(0);
// 		}

// 		memset(sound, 0, sizeof(Sound));
// 		sound->exists = true;
// 		sound->path = stringClone(path);
// 		sound->tweakVolume = 1;

// 		sound->oggData = (u8 *)readFile(sound->path, &sound->oggDataLen);
// 		audio->memoryUsed += sound->oggDataLen;
// 		if (!onlyLoadRaw) initSound(sound);

// 		if (audio->soundStoreNum < SOUNDS_MAX) {
// 			strcpy(audio->soundStoreNames[audio->soundStoreNamesNum++], path);
// 			audio->soundStore[audio->soundStoreNum++] = sound;
// 		} else {
// 			logf("Sound store full! Sound will leak\n");
// 		}

// 		return sound;
// 	}

// 	return NULL;
// }

// void initSound(Sound *sound) {
// #if DO_CUSTOM_AUDIO_STREAMS
// 	// sound->vorbis = stb_vorbis_open_memory(sound->oggData, sound->oggDataLen, NULL, NULL);

// 	// if (sound->vorbis != NULL) {
// 	// 	stb_vorbis_info info = stb_vorbis_get_info((stb_vorbis *)sound->vorbis);  // Get Ogg file info
// 	// 	sound->raylibStream = Raylib::LoadAudioStream(info.sample_rate, 16, info.channels);
// 	// 	sound->frameCount = (unsigned int)stb_vorbis_stream_length_in_samples((stb_vorbis *)sound->vorbis);
// 	// } else {
// 	// 	logf("Failed to initSound\n");
// 	// 	stb_vorbis_close((stb_vorbis *)sound->vorbis);
// 	// 	sound->vorbis = NULL;
// 	// }
// #else
// 	sound->raylibMusic = Raylib::LoadMusicStreamFromMemory(".ogg", sound->oggData, sound->oggDataLen);
// 	sound->length = Raylib::GetMusicTimeLength(sound->raylibMusic);
// #endif
// }

// Channel *getChannel(int id) {
// 	if (id == 0) return NULL;

// 	for (int i = 0; i < CHANNELS_MAX; i++) {
// 		Channel *channel = &audio->channels[i];
// 		if (channel->exists && channel->id == id) return channel;
// 	}

// 	return NULL;
// }

// Channel *playSound(Sound *sound, bool looping) {
// 	if (!sound) {
// 		logf("Called playSound with NULL sound\n");
// 		return NULL;
// 	}

// 	if (sound->channel) {
// 		logf("Raylib can't play the same sound multiple times at the same time\n");
// 		return NULL;
// 	}

// 	Channel *channel = NULL;
// 	for (int i = 0; i < CHANNELS_MAX; i++) {
// 		if (!audio->channels[i].exists) {
// 			channel = &audio->channels[i];
// 			break;
// 		}
// 	}

// 	if (!channel) {
// 		logf("There are no more sound channels!\n");
// 		return NULL;
// 	}

// 	memset(channel, 0, sizeof(Channel));

// 	channel->id = ++audio->nextChannelId;
// 	channel->userVolume = 1;
// 	channel->userVolume2 = 1;
// 	channel->exists = true;
// 	channel->looping = looping;

// 	channel->sound = sound;
// 	sound->channel = channel;
// 	audio->channelsCount++;

// #if DO_CUSTOM_AUDIO_STREAMS
// #else
// 	Raylib::PlayMusicStream(channel->sound->raylibMusic);
// #endif

// 	return channel;
// }

// void stopChannel(Channel *channel) {
// 	channel->exists = false;
// 	if (channel->sound) {
// 		Raylib::StopMusicStream(channel->sound->raylibMusic);
// 		channel->sound->channel = NULL;
// 		channel->sound = NULL;
// 	}
// }

// void stopChannel(int channelId) {
// 	Channel *channel = getChannel(channelId);
// 	if (channel) stopChannel(channel);
// }

// void seekChannelPerc(Channel *channel, float perc) { //@untested
// 	Raylib::SeekMusicStream(channel->sound->raylibMusic, perc*channel->sound->length);
// }
