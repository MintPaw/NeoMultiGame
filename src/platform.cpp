#if !defined(PLAYING_bulletHellGame)
# define DISABLE_JOYSTICK
#endif

#if defined(__EMSCRIPTEN__) && !defined(DISABLE_JOYSTICK)
# define DISABLE_JOYSTICK
#endif

#define RENDERER_HEADER
#include "renderer.cpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>

#if defined(__linux__) || defined(__EMSCRIPTEN__)
# include <errno.h>
#endif

#if defined(_WIN32)
extern "C" { __declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001; }
#elif defined(__EMSCRIPTEN__)
EM_BOOL pointerLockChangeCallback(int eventType, const EmscriptenPointerlockChangeEvent *event, void *userData);
EM_BOOL mouseEventCallback(int eventType, const EmscriptenMouseEvent *event, void *userData);
EM_BOOL focusEventCallback(int eventType, const EmscriptenFocusEvent *event, void *userData);
#endif

enum KeyState { KEY_RELEASED, KEY_PRESSED, KEY_JUST_RELEASED, KEY_JUST_PRESSED };
enum PlatformKey { KEY_LEFT=301, KEY_RIGHT=302, KEY_UP=303, KEY_DOWN=304, KEY_SHIFT=305, KEY_BACKSPACE=306, KEY_CTRL=307, KEY_BACKTICK=308, KEY_ESC=309 };

struct NanoTime {
	unsigned int seconds;
	unsigned int nanos;
#if defined(_WIN32)
	LARGE_INTEGER largeInt;
#else
	timespec timeSpec;
#endif
};

struct JoyPad {
	bool connected;
	SDL_Joystick *sdlJoystick;

#define JOY_PAD_BUTTONS_MAX 32
	KeyState buttons[JOY_PAD_BUTTONS_MAX];
#define JOY_PAD_AXES_MAX 8
	float axes[JOY_PAD_AXES_MAX];
};

void initPlatform(int windowWidth, int windowHeight, const char *gameName="", Vec2 initialPos=v2());
void platformUpdateLoop(void (*updateCallback)());
void platformUpdate();
void platformPreventCtrlC(int value);

bool keyPressed(int key);
bool keyJustPressed(int key);
bool keyJustReleased(int key);
void resetKeys();
// Vec2 getLeftStick(int controllerId);
// Vec2 getRightStick(int controllerId);
bool joyButtonPressed(int controllerId, int button);
bool joyButtonJustReleased(int controllerId, int button);
bool joyButtonJustPressed(int controllerId, int button);
float joyAxis(int controllerId, int axis);
Vec2 joyLeftStick(int controllerId);
Vec2 joyRightStick(int controllerId);
void maximizeWindow();
void minimizeWindow();
void setClipboard(char *str);

NanoTime getNanoTime();
void getNanoTime(NanoTime *time);
float getMsPassed(NanoTime *startTime, NanoTime *endTime);
float getMsPassed(NanoTime startTime);

void updateEvents();
void pressKey(int key);
void releaseKey(int key);
void pressMouse();
void releaseMouse();
void pressRightMouse();
void releaseRightMouse();

void platformSleep(int ms);

void navigateToUrl(const char *url);

void platformExitHook();

#define logLastOSError() logLastOSErrorCode(__FILE__, __LINE__)
void logLastOSErrorCode(const char *fileName, int lineNum);

#define getLastOSError() getLastOSErrorCode(__FILE__, __LINE__)
char *getLastOSErrorCode(const char *fileName, int lineNum);

void showErrorWindow(char *msg);

#if defined(_WIN32)
LONG WINAPI windowsCrashHandler(EXCEPTION_POINTERS *ex);
#endif

// FUNCTIONS ^

struct Platform {
	SDL_Window *sdlWindow;
	SDL_GLContext sdlContext;

#if defined(_WIN32)
	HWND hwnd;
	LARGE_INTEGER performanceFrequency;
#endif
	bool isInternalVersion;
	bool isDebugVersion;
	bool isCommandLineOnly;

	int windowWidth;
	int windowHeight;
	Vec2 initialSize;

	NanoTime frameNano;
	bool running;
	bool sleepWait;
	char *gameName;

#define KEYS_MAX 500
	KeyState keys[KEYS_MAX];

	Vec2 relativeMouse;
	Vec2 htmlNextRelativeMouse;
	Vec2 mouse;
	int mouseWheel;
	bool mouseDown;
	bool mouseJustDown;
	bool mouseJustUp;
	bool rightMouseDown;
	bool rightMouseJustDown;
	bool rightMouseJustUp;
	bool hideCursor;
	bool prevUseRelativeMouse;
	bool useRelativeMouse;
	bool waitingForPointerLock;

	bool windowHasFocus;

#define JOY_PADS_MAX 8
	JoyPad joyPads[JOY_PADS_MAX];
	int joyPadsNum;

	bool disableGui;
	bool hoveringGui;
	bool typingGui;

	bool usingSkia;

	bool useRealElapsed;
	int frameTime;
	int targetFps;
	float elapsed;
	float realElapsed;
	float time;
	int frameCount;

	float *frameTimes;
	int frameTimesMax;
	float frameTimeAvg;

	void (*updateCallback)();
	void (*atExitCallback)();
};

Platform *platform = NULL;

#define GUI_HEADER
#include "gui.cpp"

void initPlatform(int windowWidth, int windowHeight, const char *gameName, Vec2 initialPos) {
	printf("Platform initing\n");
	setbuf(stdout, NULL);
	srand(time(NULL));
	pushRndSeed(time(NULL));
	perlinSeed = time(NULL);

	atexit(platformExitHook);

#ifdef _WIN32
	::SetUnhandledExceptionFilter(windowsCrashHandler);
	SetProcessDPIAware();
	timeBeginPeriod(1);
#endif

	platform = (Platform *)malloc(sizeof(Platform));
	memset(platform, 0, sizeof(Platform));
	initLoggingSystem();
	logf("Starting custom engine\n");

#ifdef _WIN32
	QueryPerformanceFrequency(&platform->performanceFrequency);
#endif

#if defined(FALLOW_COMMAND_LINE_ONLY)
	platform->isCommandLineOnly = true;
#endif

#if defined(FALLOW_DEBUG)
	platform->isDebugVersion = true;
#endif

#if defined(FALLOW_INTERNAL)
	platform->isInternalVersion = true;
#endif

	platform->windowWidth = windowWidth;
	platform->windowHeight = windowHeight;
	platform->initialSize = v2(windowWidth, windowHeight);
	platform->targetFps = 60.0;

	if (!platform->isCommandLineOnly) {
		if (SDL_Init(SDL_INIT_VIDEO)) {
			logf("Failed to SDL_INIT_VIDEO\n");
			Panic("Can't continue");
		}

#ifndef DISABLE_JOYSTICK
		if (SDL_Init(SDL_INIT_JOYSTICK)) {
			logf("Failed to SDL_INIT_JOYSTICK\n");
			Panic("Can't continue");
		}
#endif

		int sdlWindowParams = SDL_WINDOW_OPENGL|SDL_WINDOW_ALLOW_HIGHDPI|SDL_WINDOWEVENT|SDL_WINDOW_RESIZABLE;

		if (initialPos.isZero()) {
			initialPos = v2(SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED);
		}

		platform->sdlWindow = SDL_CreateWindow(
			gameName,
			initialPos.x,
			initialPos.y,
			platform->windowWidth,
			platform->windowHeight,
			sdlWindowParams
		);
		if (!platform->sdlWindow) {
			logf("Failed to create window: %s\n", SDL_GetError());
			Panic("Can't continue");
		}

#if defined(__LINUX__)
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
#elif defined(__EMSCRIPTEN__)
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		// SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		// SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
#else

#if defined(PLAYING_horseGame)
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
#endif
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		// SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
#endif

		// SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 0);
		// SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 0);

		platform->sdlContext = SDL_GL_CreateContext(platform->sdlWindow);
		if (!platform->sdlContext) {
			logf("SDL_GL_CreateContext failed: %s\n", SDL_GetError());
			Panic("Can't continue");
		}

#if !defined (__EMSCRIPTEN__) // Emscripten will set the swap interval later (maybe)
		if (SDL_GL_SetSwapInterval(0)) logf("Failed to set swap interval\n"); // 0=vsync off, 1=vsync on
#endif

		glewExperimental = GL_TRUE; 
		Assert(glewInit() == GLEW_OK);
	}

#if defined(_WIN32)
	SDL_SysWMinfo wmInfo;
	SDL_VERSION(&wmInfo.version);
	SDL_GetWindowWMInfo(platform->sdlWindow, &wmInfo);
	platform->hwnd = wmInfo.info.win.window;
#elif defined(__EMSCRIPTEN__)
	if (emscripten_set_mousemove_callback("#canvas", NULL, false, mouseEventCallback)) logf("Couldn't set mouse callback\n");
	if (emscripten_set_pointerlockchange_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, NULL, false, pointerLockChangeCallback)) logf("Couldn't set pointerlock callback\n");
	if (emscripten_set_focus_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, NULL, false, focusEventCallback)) logf("Couldn't set focus callback\n");
	if (emscripten_set_blur_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, NULL, false, focusEventCallback)) logf("Couldn't set focus callback\n");
#endif

	platform->gameName = stringClone(gameName);
	guiInit();

	platform->frameTimesMax = 60;
	platform->frameTimes = (float *)malloc(sizeof(float) * platform->frameTimesMax);

	// SDL_SetRelativeMouseMode(SDL_TRUE);
	// initHotloader("assets");
}

void platformUpdateLoop(void (*updateCallback)()) {
	platform->running = true;
	platform->updateCallback = updateCallback;

#if defined(__EMSCRIPTEN__)
	emscripten_set_main_loop(platformUpdate, 0, 1);
	if (SDL_GL_SetSwapInterval(0)) logf("Failed to set swap interval\n");
#else
	while (platform->running) {
		platformUpdate();

		if (platform->sleepWait) {
			int sleepMs = ceil(1.0/platform->targetFps * 1000.0) - platform->frameTime;
			if (sleepMs > 0) platformSleep(sleepMs);
		} else {
			int msToKill = ceil(1.0/platform->targetFps * 1000.0) - platform->frameTime;

			NanoTime timeStart = getNanoTime();
			// int msStart = getTicks();
			for (;;) {
				float msPassed = getMsPassed(timeStart);
				if (msPassed >= msToKill) break;
				// int msNow = getTicks();
				// if (msNow - msStart >= msToKill) break;
			}
		}
	}
#endif
}

void platformUpdate() {
	if (platform->frameCount == 0) {
		platform->realElapsed = 1.0/60.0;
	} else {
		platform->realElapsed = getMsPassed(platform->frameNano) / 1000.0;
	}

	platform->frameNano = getNanoTime();

	if (platform->useRealElapsed) {
		platform->elapsed = platform->realElapsed;
		if (platform->elapsed < 0.0001) platform->elapsed = 0.0001;
		if (platform->elapsed > 1/15.0) platform->elapsed = 1/15.0;
	} else {
		platform->elapsed = 1.0/platform->targetFps;
	}

	SDL_ShowCursor(platform->hideCursor ? SDL_DISABLE : SDL_ENABLE);

	platform->time += platform->elapsed;
	logSys->time = platform->time;
	platform->frameCount++;

	updateEvents(); // Should this really happen if you're on the command line?

	if (platform->isCommandLineOnly) {
		platform->updateCallback();
	} else {
		guiStartFrame();

		platform->updateCallback();
		endRenderingFrame();

		void updateAudio(); //@headerHack
		updateAudio();
		guiDraw();

		SDL_GL_SwapWindow(platform->sdlWindow);
	}

	freeFrameMemory();
	// freeFrameTextures();

	{ /// Calcuate frame times
		float frameMs = getMsPassed(platform->frameNano);
		platform->frameTime = frameMs;
		platform->frameTimes[platform->frameCount % platform->frameTimesMax] = frameMs;

		platform->frameTimeAvg = 0;
		for (int i = 0; i < platform->frameTimesMax; i++) platform->frameTimeAvg += platform->frameTimes[i];
		platform->frameTimeAvg /= platform->frameTimesMax;
	}
}

void updateEvents() {
#ifdef __EMSCRIPTEN__
	if (platform->useRelativeMouse) {
		logf("relative mouse doesn't work on emscripten right now\n");
	}
#endif

	platform->mouseJustDown = false;
	platform->mouseJustUp = false;
	platform->rightMouseJustDown = false;
	platform->rightMouseJustUp = false;

	int flags = SDL_GetWindowFlags(platform->sdlWindow);

#if defined(_WIN32)
	platform->windowHasFocus = false;
	if (GetFocus()) platform->windowHasFocus = true;

	if (platform->windowHasFocus) {
		CURSORINFO cursorInfo = {};
		cursorInfo.cbSize = sizeof(CURSORINFO);
		if (GetCursorInfo(&cursorInfo)) {
			bool cursorIsVisible = cursorInfo.flags & CURSOR_SHOWING;
			if (cursorIsVisible && platform->useRelativeMouse) ShowCursor(FALSE);
			if (!cursorIsVisible && !platform->useRelativeMouse) ShowCursor(TRUE);
		} else {
			logf("Failed to get cursor info\n");
			logLastOSError();
		}
	}

	platform->relativeMouse = v2();
#elif defined(__EMSCRIPTEN__)
	EmscriptenPointerlockChangeEvent pointerlockStatus = {};
	emscripten_get_pointerlock_status(&pointerlockStatus);
	platform->waitingForPointerLock = false;
	if (!pointerlockStatus.isActive && platform->useRelativeMouse) {
		platform->waitingForPointerLock = true;
		if (emscripten_request_pointerlock("#canvas", true) != EMSCRIPTEN_RESULT_SUCCESS) {
			// Nothing...
		}
	}

	if (pointerlockStatus.isActive && !platform->useRelativeMouse) {
		if (emscripten_exit_pointerlock() != EMSCRIPTEN_RESULT_SUCCESS) {
			// Nothing...
		}
	}

	platform->relativeMouse = platform->htmlNextRelativeMouse;
	platform->htmlNextRelativeMouse = v2();
#endif

	for (int i = 0; i < KEYS_MAX; i++) {
		if (platform->keys[i] == KEY_JUST_PRESSED) platform->keys[i] = KEY_PRESSED;
		else if (platform->keys[i] == KEY_JUST_RELEASED) platform->keys[i] = KEY_RELEASED;
	}

	for (int i = 0; i < JOY_PADS_MAX; i++) {
		JoyPad *pad = &platform->joyPads[i];
		if (!pad->connected) continue;

		for (int i = 0; i < JOY_PAD_BUTTONS_MAX; i++) {
			if (pad->buttons[i] == KEY_JUST_PRESSED) pad->buttons[i] = KEY_PRESSED;
			else if (pad->buttons[i] == KEY_JUST_RELEASED) pad->buttons[i] = KEY_RELEASED;
		}
	}

	platform->mouseWheel = 0;

	SDL_Event e;
	while (SDL_PollEvent(&e) != 0) {
		guiProcessSdlEvent(&e);
		if (e.type == SDL_QUIT) {
			platform->running = false;
		} else if (e.type == SDL_WINDOWEVENT) {
			if (e.window.event == SDL_WINDOWEVENT_RESIZED) {
				platform->windowWidth = e.window.data1;
				platform->windowHeight = e.window.data2;
			}
		} else if (e.type == SDL_KEYDOWN) {
			int key = e.key.keysym.sym;
			if (key >= 'a' && key <= 'z') key -= 'a'-'A';
			if (key == SDLK_KP_0) key = '0';
			if (key == SDLK_KP_1) key = '1';
			if (key == SDLK_KP_2) key = '2';
			if (key == SDLK_KP_3) key = '3';
			if (key == SDLK_KP_4) key = '4';
			if (key == SDLK_KP_5) key = '5';
			if (key == SDLK_KP_6) key = '6';
			if (key == SDLK_KP_7) key = '7';
			if (key == SDLK_KP_8) key = '8';
			if (key == SDLK_KP_9) key = '9';
			if (key == SDLK_LSHIFT) key = KEY_SHIFT;
			if (key == SDLK_RSHIFT) key = KEY_SHIFT;
			if (key == SDLK_BACKSPACE) key = KEY_BACKSPACE;
			if (key == SDLK_LCTRL) key = KEY_CTRL;
			if (key == SDLK_RCTRL) key = KEY_CTRL;
			if (key == SDLK_UP) key = KEY_UP;
			if (key == SDLK_DOWN) key = KEY_DOWN;
			if (key == SDLK_LEFT) key = KEY_LEFT;
			if (key == SDLK_RIGHT) key = KEY_RIGHT;
			if (key == SDLK_BACKQUOTE) key = KEY_BACKTICK;
			if (key == SDLK_ESCAPE) key = KEY_ESC;
			if (key > KEYS_MAX-1) return;

			pressKey(key);
		} else if (e.type == SDL_KEYUP) {
			int key = e.key.keysym.sym;
			if (key >= 'a' && key <= 'z') key -= 'a'-'A';
			if (key == SDLK_KP_0) key = '0';
			if (key == SDLK_KP_1) key = '1';
			if (key == SDLK_KP_2) key = '2';
			if (key == SDLK_KP_3) key = '3';
			if (key == SDLK_KP_4) key = '4';
			if (key == SDLK_KP_5) key = '5';
			if (key == SDLK_KP_6) key = '6';
			if (key == SDLK_KP_7) key = '7';
			if (key == SDLK_KP_8) key = '8';
			if (key == SDLK_KP_9) key = '9';
			if (key == SDLK_LSHIFT) key = KEY_SHIFT;
			if (key == SDLK_RSHIFT) key = KEY_SHIFT;
			if (key == SDLK_BACKSPACE) key = KEY_BACKSPACE;
			if (key == SDLK_LCTRL) key = KEY_CTRL;
			if (key == SDLK_RCTRL) key = KEY_CTRL;
			if (key == SDLK_UP) key = KEY_UP;
			if (key == SDLK_DOWN) key = KEY_DOWN;
			if (key == SDLK_LEFT) key = KEY_LEFT;
			if (key == SDLK_RIGHT) key = KEY_RIGHT;
			if (key == SDLK_BACKQUOTE) key = KEY_BACKTICK;
			if (key == SDLK_ESCAPE) key = KEY_ESC;
			if (key > KEYS_MAX-1) return;

			releaseKey(key);
		}	else if (e.type == SDL_MOUSEMOTION) {
			// platform->mouse.x = e.motion.x;
			// platform->mouse.y = e.motion.y;
		} else if (e.type == SDL_MOUSEBUTTONDOWN) {
			if (e.button.button == SDL_BUTTON_LEFT) pressMouse();
			if (e.button.button == SDL_BUTTON_RIGHT) pressRightMouse();
		} else if (e.type == SDL_MOUSEBUTTONUP) {
			if (e.button.button == SDL_BUTTON_LEFT) releaseMouse();
			if (e.button.button == SDL_BUTTON_RIGHT) releaseRightMouse();
		} else if (e.type == SDL_MOUSEWHEEL) {
			platform->mouseWheel = e.wheel.y;
#if defined (__EMSCRIPTEN__)
			platform->mouseWheel /= 100;
#endif
		} else if (e.type == SDL_JOYDEVICEADDED) {
			int index = e.jdevice.which;
			if (index > JOY_PADS_MAX-1) {
				logf("Joypad number too high %d\n", index);
				continue;
			}

			JoyPad *pad = &platform->joyPads[index];
			pad->connected = true;
			pad->sdlJoystick = SDL_JoystickOpen(index);
			if (!pad->sdlJoystick) {
				logf("Failed to open pad %d\n", index);
				pad->connected = false;
			}
		} else if (e.type == SDL_JOYDEVICEREMOVED) {
			int index = e.jdevice.which;
			if (index > JOY_PADS_MAX-1) {
				logf("Joypad number too high %d\n", index);
				continue;
			}

			JoyPad *pad = &platform->joyPads[index];
			pad->connected = false;
			SDL_JoystickClose(pad->sdlJoystick);
			pad->sdlJoystick = NULL;
		} else if (e.type == SDL_JOYHATMOTION) {
			// logf("here\n");
			// int index = e.jdevice.which;
			// if (index > JOY_PADS_MAX-1) {
			// 	logf("Joypad number too high %d\n", index);
			// 	continue;
			// }
			// if (e.jhat.hat != 0) logf("Non 0 hat??\n");

			// JoyPad *pad = &platform->joyPads[index];
			// if (e.jhat.value == SDL_HAT_LEFTUP) pad->hat = HAT_UP_LEFT;
			// if (e.jhat.value == SDL_HAT_UP) pad->hat = HAT_UP;
			// if (e.jhat.value == SDL_HAT_RIGHTUP) pad->hat = HAT_UP_RIGHT;
			// if (e.jhat.value == SDL_HAT_LEFT) pad->hat = HAT_LEFT;
			// if (e.jhat.value == SDL_HAT_CENTERED) pad->hat = HAT_CENTER;
			// if (e.jhat.value == SDL_HAT_RIGHT) pad->hat = HAT_RIGHT;
			// if (e.jhat.value == SDL_HAT_LEFTDOWN) pad->hat = HAT_DOWN_LEFT;
			// if (e.jhat.value == SDL_HAT_DOWN) pad->hat = HAT_DOWN;
			// if (e.jhat.value == SDL_HAT_RIGHTDOWN) pad->hat = HAT_DOWN_RIGHT;
		} else if (e.type == SDL_JOYAXISMOTION) {
			int index = e.jaxis.which;
			int axisIndex = e.jaxis.axis;
			if (index > JOY_PADS_MAX-1) {
				logf("Joypad number too high %d\n", index);
				continue;
			}

			if (axisIndex > JOY_PAD_AXES_MAX-1) {
				logf("Joypad axis too high %d\n", axisIndex);
				continue;
			}

			JoyPad *pad = &platform->joyPads[index];
			pad->axes[axisIndex] = (e.jaxis.value + 32768.0) / SHRT_MAX - 1;
		} else if (e.type == SDL_JOYBUTTONUP || e.type == SDL_JOYBUTTONDOWN) {
			int index = e.jbutton.which;
			int buttonIndex = e.jbutton.button;
			if (index > JOY_PADS_MAX-1) {
				logf("Joypad number too high %d\n", index);
				continue;
			}

			if (buttonIndex > JOY_PAD_BUTTONS_MAX-1) {
				logf("Joypad button too high %d\n", buttonIndex);
				continue;
			}

			JoyPad *pad = &platform->joyPads[index];
			if (e.type == SDL_JOYBUTTONDOWN) {
				pad->buttons[buttonIndex] = KEY_JUST_PRESSED;
			} else {
				pad->buttons[buttonIndex] = KEY_JUST_RELEASED;
			}
		}
	}

#if defined(__EMSCRIPTEN__)
	int width, height;
	EMSCRIPTEN_RESULT result = emscripten_get_canvas_element_size("#canvas", &width, &height);
	if (result == EMSCRIPTEN_RESULT_SUCCESS) {
		platform->windowWidth = width;
		platform->windowHeight = height;
	}
#endif

#if defined(_WIN32)
	if (platform->windowHasFocus) {
		POINT winMouse;
		GetCursorPos(&winMouse);
		ScreenToClient(platform->hwnd, &winMouse);

		if (platform->useRelativeMouse) {
			Vec2 windowCenter = v2(platform->windowWidth/2, platform->windowHeight/2);

			platform->relativeMouse.x = winMouse.x - windowCenter.x;
			platform->relativeMouse.y = winMouse.y - windowCenter.y;

			POINT winWindowCenter;
			winWindowCenter.x = windowCenter.x;
			winWindowCenter.y = windowCenter.y;
			ClientToScreen(platform->hwnd, &winWindowCenter);
			SetCursorPos(winWindowCenter.x, winWindowCenter.y);
		} else {
			platform->mouse.x = winMouse.x;
			platform->mouse.y = winMouse.y;
		}
	}
#endif

	if (platform->useRelativeMouse && !platform->prevUseRelativeMouse) {
		platform->prevUseRelativeMouse = platform->useRelativeMouse;
		platform->relativeMouse = v2();
	}

	guiUpdateEvents();

	if (platform->hoveringGui) {
		platform->mouseJustUp = false;
		platform->mouseJustDown = false;
		platform->mouseDown = false;
		platform->mouseWheel = 0;
	}

	if (keyPressed(KEY_CTRL) && keyPressed('Q')) platform->running = false;
}

void pressKey(int key) {	
	if (platform->typingGui) return;
	if (platform->keys[key] == KEY_RELEASED || platform->keys[key] == KEY_JUST_RELEASED) { //@cleanup Why do we do this check?
		platform->keys[key] = KEY_JUST_PRESSED;
	}	
}

void releaseKey(int key) {
	if (platform->keys[key] == KEY_PRESSED || platform->keys[key] == KEY_JUST_PRESSED) {
		platform->keys[key] = KEY_JUST_RELEASED;
	}	
}

void pressMouse() {
	platform->mouseDown = true;
	platform->mouseJustDown = true;
}
void releaseMouse() {
	platform->mouseDown = false;
	platform->mouseJustUp = true;
}
void pressRightMouse() {
	platform->rightMouseDown = true;
	platform->rightMouseJustDown = true;
}
void releaseRightMouse() {
	platform->rightMouseDown = false;
	platform->rightMouseJustUp = true;
}

bool keyPressed(int key) {
	if (platform->keys[key] == KEY_JUST_PRESSED || platform->keys[key] == KEY_PRESSED) return true;
	return false;
}

bool keyJustPressed(int key) {
	if (platform->keys[key] == KEY_JUST_PRESSED) return true;
	return false;
}

bool keyJustReleased(int key) {
	if (platform->keys[key] == KEY_JUST_RELEASED) return true;
	return false;
}

void resetKeys() {
	for (int i = 0; i < KEYS_MAX; i++) {
		platform->keys[i] = KEY_RELEASED;
	}
}

bool joyButtonPressed(int controllerId, int button) {
	JoyPad *pad = &platform->joyPads[controllerId];
	if (!pad->connected) return false;

	if (pad->buttons[button] == KEY_PRESSED || pad->buttons[button] == KEY_JUST_PRESSED) return true;
	return false;
}

bool joyButtonJustPressed(int controllerId, int button) {
	JoyPad *pad = &platform->joyPads[controllerId];
	if (!pad->connected) return false;

	if (pad->buttons[button] == KEY_JUST_PRESSED) return true;
	return false;
}

float joyAxis(int controllerId, int axis) {
	JoyPad *pad = &platform->joyPads[controllerId];
	if (!pad->connected) return false;

	return pad->axes[axis];
}

Vec2 joyLeftStick(int controllerId) {
	Vec2 ret = v2(joyAxis(controllerId, 0), joyAxis(controllerId, 1));
	return ret;
}

Vec2 joyRightStick(int controllerId) {
	Vec2 ret = v2(joyAxis(controllerId, 2), joyAxis(controllerId, 3));
	return ret;
}

bool joyButtonJustReleased(int controllerId, int button) {
	JoyPad *pad = &platform->joyPads[controllerId];
	if (!pad->connected) return false;

	if (pad->buttons[button] == KEY_JUST_RELEASED) return true;
	return false;
}

void maximizeWindow() {
#if defined(_WIN32)
	ShowWindow(platform->hwnd, SW_MAXIMIZE);
#endif
}

void minimizeWindow() {
#if defined(_WIN32)
	ShowWindow(platform->hwnd, SW_MINIMIZE);
#endif
}

void setClipboard(char *str) {
	SDL_SetClipboardText(str);
}

void resizeWindow(int width, int height) {
	SDL_SetWindowSize(platform->sdlWindow, width, height);
	platform->windowWidth = width;
	platform->windowHeight = height;
}

void platformSleep(int ms) {
#ifdef _WIN32
	Sleep(ms);
#else
	timespec ts;
	ts.tv_sec = ms / 1000;
	ts.tv_nsec = (ms % 1000) * 1000000;
	nanosleep(&ts, NULL);
#endif
}

NanoTime getNanoTime();
NanoTime getNanoTime() {
	NanoTime time = {};

#ifdef _WIN32
	QueryPerformanceCounter(&time.largeInt);
#endif

#ifdef __linux__
	clock_gettime(CLOCK_REALTIME, &time.timeSpec);
#endif

#ifdef __EMSCRIPTEN__
	clock_gettime(CLOCK_REALTIME, &time.timeSpec);
#endif

	return time;
}

float getMsPassed(NanoTime startTime) {
	NanoTime endTime = getNanoTime();

	NanoTime time;

#ifdef _WIN32
	LONGLONG llTimeDiff = endTime.largeInt.QuadPart - startTime.largeInt.QuadPart;

	float ftDuration = (float)llTimeDiff * 1000.0 / (double)platform->performanceFrequency.QuadPart;
	return ftDuration;
#else
	float seconds = (float)((float)endTime.timeSpec.tv_sec - (float)startTime.timeSpec.tv_sec) + (float)((float)endTime.timeSpec.tv_nsec - (float)startTime.timeSpec.tv_nsec) / 1000000000.0;
	float ms = seconds * 1000.0;
	if (ms < 0) ms = 0;
	return ms;
#endif
}

void navigateToUrl(const char *url) {
#if defined(_WIN32) 
	char *command = frameMalloc(PATH_MAX_LEN);
	strcpy(command, "start ");
	strcat(command, url);
	system(command);
#elif defined(__EMSCRIPTEN__)
	char *command = frameMalloc(2048);
	snprintf(command, 2048, "window.open(\"%s\", \"_blank\");", url);
	emscripten_run_script(command);
	// ES_ASM({
	// 	"window.open(URL, '_blank');"
	// });
#else
	logf("Can't open URLs on this platform\n");
#endif
}

void platformExitHook() {
	if (platform->atExitCallback) platform->atExitCallback();
	SDL_DestroyWindow(platform->sdlWindow);
	SDL_Quit();

#if defined(_WIN32)
	// createMiniDump();
#endif
};

void platformPreventCtrlC(int value) {
	logf("Ctrl-C was prevented\n");
}

char *getLastOSErrorCode(const char *fileName, int lineNum) {
#if defined(_WIN32)
	DWORD errorMessageID = ::GetLastError();
	if(errorMessageID == 0) return NULL;

	LPSTR messageBuffer = NULL;
	size_t size = FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		errorMessageID,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPSTR)&messageBuffer,
		0,
		NULL
	);

	char *str = mallocSprintf("Error (%s:%d): %s\n", fileName, lineNum, messageBuffer);
	LocalFree(messageBuffer);
	return str;
#else
	int myErrno = errno;
	if (myErrno == 0) return NULL;
	char *str = mallocSprintf("Linux errorno (%s:%d): %s (%d)\n", fileName, lineNum, strerror(myErrno), myErrno);
	return str;
#endif
}

void logLastOSErrorCode(const char *fileName, int lineNum) {
#if defined(_WIN32)
	DWORD errorMessageID = ::GetLastError();
	if(errorMessageID == 0) return;

	LPSTR messageBuffer = NULL;
	size_t size = FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		errorMessageID,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPSTR)&messageBuffer,
		0,
		NULL
	);

	printf("Error (%s:%d): %s\n", fileName, lineNum, messageBuffer);

	LocalFree(messageBuffer);
#else
	int myErrno = errno;
	if (myErrno != 0) {
		logf("Linux errorno (%s:%d): %s (%d)\n", fileName, lineNum, strerror(myErrno), myErrno);
	}
#endif
}

void showErrorWindow(char *msg) {
#if defined(_WIN32)
	int result = MessageBoxA(NULL, msg, "Error", MB_OK);
#elif defined(__EMSCRIPTEN__)
	EM_ASM({
		var str = UTF8ToString($0);
		str = str.split("\n").join("<br />");
		crashlogElement.innerHTML += str;
	}, msg);
#else
	logf("Can't show error window on this platform\n");
#endif
}

void pngQuantImage(const char *path);
void pngQuantImage(const char *path) {
#if defined(_WIN32)
	char *pngAbsPath = frameSprintf("%s/%s", projectAssetDir, path);

	char *command = frameSprintf(
		"\"%s/assets/__raw/tools/pngquant.exe\" \"%s\" -o \"%s\" --strip --speed 1 --quality 100 --force",
		projectAssetDir,
		pngAbsPath,
		pngAbsPath
	);

	// 	"%s/buildSystem/tools/pngcrush.exe -rem alla -rem text -rem gAMA -rem cHRM -rem iCCP -rem sRGB -force %s %s",

	STARTUPINFO startupInfo = {};
	startupInfo.cb = sizeof(startupInfo);

	PROCESS_INFORMATION processInfo = {};

	BOOL good = CreateProcess(
		NULL,                    // the command
		command,                 // Command line
		NULL,                    // Process handle not inheritable
		NULL,                    // Thread handle not inheritable
		FALSE,                   // Set handle inheritance to FALSE
		CREATE_NO_WINDOW,
		NULL,                    // Use parent's environment block
		NULL,                    // Use parent's starting directory 
		&startupInfo,            // Pointer to STARTUPINFO structure
		&processInfo             // Pointer to PROCESS_INFORMATION structure (removed extra parentheses)
	);
	if (!good) {
		logf("No good\n");
		logLastOSError();
	}

	WaitForSingleObject(processInfo.hProcess, INFINITE);
#else
	logf("Can't pngQuantImage on this platform\n");
#endif
}


#if defined(_WIN32)
#if defined(FALLOW_INTERNAL)
#include <tchar.h>
#include <dbghelp.h>
#include <crtdbg.h>

void createMiniDump(EXCEPTION_POINTERS *pep);
BOOL CALLBACK MyMiniDumpCallback(PVOID pParam, const PMINIDUMP_CALLBACK_INPUT pInput, PMINIDUMP_CALLBACK_OUTPUT pOutput); 
bool IsDataSectionNeeded(const WCHAR* pModuleName); 

void createMiniDump(EXCEPTION_POINTERS *pep) {
	char *dumpPath = frameSprintf("%s/minidump.dmp", exeDir);
	printf("Creating minidump... at %s\n", dumpPath);
	HANDLE hFile = CreateFile(dumpPath, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL); 

	if(!hFile || hFile == INVALID_HANDLE_VALUE) {
		_tprintf( _T("CreateFile failed. Error: %lu \n"), GetLastError() ); 
		return;
	}

	MINIDUMP_EXCEPTION_INFORMATION mdei; 

	mdei.ThreadId = GetCurrentThreadId(); 
	mdei.ExceptionPointers = pep; 
	mdei.ClientPointers = false; 

	MINIDUMP_CALLBACK_INFORMATION mci; 

	mci.CallbackRoutine = (MINIDUMP_CALLBACK_ROUTINE)MyMiniDumpCallback; 
	mci.CallbackParam = 0; 

	MINIDUMP_TYPE mdt = (MINIDUMP_TYPE)(
		MiniDumpWithPrivateReadWriteMemory | 
		MiniDumpWithDataSegs | 
		MiniDumpWithHandleData |
		MiniDumpWithFullMemoryInfo | 
		MiniDumpWithThreadInfo | 
		MiniDumpWithUnloadedModules
	); 

	BOOL rv = MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, mdt, (pep != 0) ? &mdei : 0, 0, &mci);
	CloseHandle(hFile); 

	if (!rv) {
		_tprintf( _T("MiniDumpWriteDump failed. Error: %lu \n"), GetLastError()); 
		logLastOSError();
	} else {
		_tprintf( _T("Minidump created.\n") ); 
	}
}

BOOL CALLBACK MyMiniDumpCallback(PVOID pParam, const PMINIDUMP_CALLBACK_INPUT pInput, PMINIDUMP_CALLBACK_OUTPUT pOutput) {
	BOOL bRet = false; 
	if (!pInput) return false; 
	if (!pOutput) return false; 

	// Process the callbacks 
	switch(pInput->CallbackType) {
		case IncludeModuleCallback: {
			// Include the module into the dump 
			bRet = true; 
		} break; 

		case IncludeThreadCallback: {
			// Include the thread into the dump 
			bRet = true; 
		} break; 

		case ModuleCallback: {
			// Are data sections available for this module ? 

			if(pOutput->ModuleWriteFlags & ModuleWriteDataSeg) {
				// Yes, they are, but do we need them? 

				if(!IsDataSectionNeeded(pInput->Module.FullPath)) {
					// wprintf( L"Excluding module data sections: %s \n", pInput->Module.FullPath ); 
					pOutput->ModuleWriteFlags &= (~ModuleWriteDataSeg); 
				}
			}

			bRet = true; 
		} break; 

		case ThreadCallback: {
			// Include all thread information into the minidump 
			bRet = true;  
		} break; 

		case ThreadExCallback: {
			// Include this information 
			bRet = true;  
		} break; 

		case MemoryCallback: {
			// We do not include any information here -> return false 
			bRet = false; 
		} break; 

		case CancelCallback: 
			break; 
	}

	return bRet; 
}

bool IsDataSectionNeeded(const WCHAR* pModuleName) {
	if (!pModuleName) Panic("Parameter is null."); 

	// Extract the module name 
	WCHAR szFileName[_MAX_FNAME] = L""; 
	_wsplitpath( pModuleName, NULL, NULL, szFileName, NULL ); 


	// Compare the name with the list of known names and decide 
	// Note: For this to work, the executable name must be "mididump.exe"
	if (wcsicmp( szFileName, L"interrogationGame.exe" ) == 0) return true; 
	else if (wcsicmp( szFileName, L"ntdll" ) == 0) return true; 

	return false; 
}
#endif

LONG WINAPI windowsCrashHandler(EXCEPTION_POINTERS *ex) {
	printf("Caught\n");

#ifdef FALLOW_INTERNAL
	const int TRACE_MAX_STACK_FRAMES = 1024;
	const int TRACE_MAX_FUNCTION_NAME_LENGTH = 1024;
	void *stack[TRACE_MAX_STACK_FRAMES];
	HANDLE process = GetCurrentProcess();
	SymInitialize(process, NULL, TRUE);
	WORD numberOfFrames = CaptureStackBackTrace(0, TRACE_MAX_STACK_FRAMES, stack, NULL);
	char buf[sizeof(SYMBOL_INFO)+(TRACE_MAX_FUNCTION_NAME_LENGTH - 1) * sizeof(TCHAR)];
	SYMBOL_INFO* symbol = (SYMBOL_INFO*)buf;
	symbol->MaxNameLen = TRACE_MAX_FUNCTION_NAME_LENGTH;
	symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
	DWORD displacement;
	IMAGEHLP_LINE64 line;
	line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
	for (int i = 0; i < numberOfFrames; i++) {
		DWORD64 address = (DWORD64)(stack[i]);
		SymFromAddr(process, address, NULL, symbol);
		if (SymGetLineFromAddr64(process, address, &displacement, &line)) {
			logf("\tat %s in %s: line: %lu: address: 0x%0X\n", symbol->Name, line.FileName, line.LineNumber, symbol->Address);
		} else {
			logf("\tSymGetLineFromAddr64 returned error code %lu.\n", GetLastError());
			logf("\tat %s, address 0x%0X.\n", symbol->Name, symbol->Address);
		}
	}

	// createMiniDump(ex);
#else
	logf("Non-internal build, no stack frames.\n");
#endif

	writeCrashLog();
	showLogfBufferErrorWindow();

	return EXCEPTION_CONTINUE_SEARCH;
}

#endif

#if defined(__EMSCRIPTEN__)
EM_BOOL pointerLockChangeCallback(int eventType, const EmscriptenPointerlockChangeEvent *event, void *userData) {
	logf("isActive: %d\n", event->isActive);
	return true;
}

EM_BOOL mouseEventCallback(int eventType, const EmscriptenMouseEvent *event, void *userData) {
	float top = EM_ASM_DOUBLE({
		return Module.canvas.getBoundingClientRect().top;
	});

	float left = EM_ASM_DOUBLE({
		return Module.canvas.getBoundingClientRect().left;
	});

	float scaleX = EM_ASM_DOUBLE({
		return Module.canvas.width / Module.canvas.getBoundingClientRect().width;
	});

	float scaleY = EM_ASM_DOUBLE({
		return Module.canvas.height / Module.canvas.getBoundingClientRect().height;
	});

	// logf("%f %f\n", scaleX, scaleY);

	// logf("Got event type %d\n", eventType);
	if (eventType == EMSCRIPTEN_EVENT_MOUSEMOVE) {
		if (platform->useRelativeMouse) {
			platform->htmlNextRelativeMouse.x = event->movementX;
			platform->htmlNextRelativeMouse.y = event->movementY;
		} else {
			platform->mouse.x = (event->clientX - left) * scaleX;
			platform->mouse.y = (event->clientY - top) * scaleY;
			// platform->mouse.x = event->targetX;
			// platform->mouse.y = event->targetY;
		}
	}
	return true;
}

EM_BOOL focusEventCallback(int eventType, const EmscriptenFocusEvent *event, void *userData) {
	if (eventType == EMSCRIPTEN_EVENT_FOCUS) {
		platform->windowHasFocus = true;
	}

	if (eventType == EMSCRIPTEN_EVENT_BLUR) {
		platform->windowHasFocus = false;
	}

	return true;
}
#endif
