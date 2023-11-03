#include "platformBackend.h"

#define GLEW_STATIC
#include <glew/glew.h>
#include <GLFW/glfw3.h>

#define STB_RECT_PACK_IMPLEMENTATION
#include "imstb_rectpack.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#include "backends/imgui_impl_opengl3.cpp"

struct BackendNanoTime {
	double time;
};

#ifdef _WIN32
	HANDLE _processHandle;
#endif

GLFWwindow *_glfwWindow;
void (*_platformFrontendUpdateCallback)();

void backendPlatformUpdateLoop();
PlatformEvent *createPlatformEvent(PlatformEventType type);

void backendPlatformInit(int windowWidth, int windowHeight, char *windowTitle) {
#ifdef _WIN32
	_processHandle = GetCurrentProcess();
#endif

	if (!glfwInit()) Panic("Failed to init glfw");

	auto glfwErrorCallback = [](int error, const char *description) { logf("GLFW Error: %s\n", description); };

	glfwSetErrorCallback(glfwErrorCallback);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	_glfwWindow = glfwCreateWindow(windowWidth, windowHeight, windowTitle, NULL, NULL);
	if (!_glfwWindow) Panic("Failed to create _glfwWindow");

	glfwMakeContextCurrent(_glfwWindow);

	GLenum err = glewInit();
	if (GLEW_OK != err) Panic(frameSprintf("GLEW Error: %s\n", glewGetErrorString(err)));
	// gladLoadGL();

	glfwSwapInterval(1);

	auto keyCallback = [](GLFWwindow *window, int key, int scancode, int action, int mods) {
		if (key == GLFW_KEY_LEFT) key = KEY_LEFT;
		if (key == GLFW_KEY_RIGHT) key = KEY_RIGHT;
		if (key == GLFW_KEY_UP) key = KEY_UP;
		if (key == GLFW_KEY_DOWN) key = KEY_DOWN;
		if (key == GLFW_KEY_LEFT_SHIFT) key = KEY_SHIFT;
		if (key == GLFW_KEY_RIGHT_SHIFT) key = KEY_RIGHT_SHIFT;
		if (key == GLFW_KEY_BACKSPACE) key = KEY_BACKSPACE;
		if (key == GLFW_KEY_LEFT_CONTROL) key = KEY_CTRL;
		if (key == GLFW_KEY_RIGHT_CONTROL) key = KEY_RIGHT_CTRL;
		if (key == GLFW_KEY_LEFT_ALT) key = KEY_ALT;
		if (key == GLFW_KEY_RIGHT_ALT) key = KEY_RIGHT_ALT;
		if (key == GLFW_KEY_GRAVE_ACCENT) key = KEY_BACKTICK;
		if (key == GLFW_KEY_ESCAPE) key = KEY_ESC;
		if (key == GLFW_KEY_F1) key = KEY_F1;
		if (key == GLFW_KEY_F2) key = KEY_F2;
		if (key == GLFW_KEY_F3) key = KEY_F3;
		if (key == GLFW_KEY_F4) key = KEY_F4;
		if (key == GLFW_KEY_F5) key = KEY_F5;
		if (key == GLFW_KEY_F6) key = KEY_F6;
		if (key == GLFW_KEY_F7) key = KEY_F7;
		if (key == GLFW_KEY_F8) key = KEY_F8;
		if (key == GLFW_KEY_F9) key = KEY_F9;
		if (key == GLFW_KEY_F10) key = KEY_F10;
		if (key == GLFW_KEY_F11) key = KEY_F11;
		if (key == GLFW_KEY_F12) key = KEY_F12;
		if (key == GLFW_KEY_ENTER) key = KEY_ENTER;
		if (key == GLFW_KEY_TAB) key = KEY_TAB;

		if (action == GLFW_PRESS) {
			PlatformEvent *event = createPlatformEvent(PLATFORM_EVENT_KEY_DOWN);
			event->keyCode = key;
		} else if (action == GLFW_RELEASE) {
			PlatformEvent *event = createPlatformEvent(PLATFORM_EVENT_KEY_UP);
			event->keyCode = key;
		}
	};
	glfwSetKeyCallback(_glfwWindow, keyCallback);

	auto cursorPosCallback = [](GLFWwindow *window, double xpos, double ypos) {
		PlatformEvent *event = createPlatformEvent(PLATFORM_EVENT_MOUSE_MOVE);
		event->position = v2(xpos, ypos);
	};
	glfwSetCursorPosCallback(_glfwWindow, cursorPosCallback);

	auto mouseButtonCallback = [](GLFWwindow *window, int button, int action, int mods) {
		if (button == GLFW_MOUSE_BUTTON_LEFT) button = MOUSE_LEFT;
		if (button == GLFW_MOUSE_BUTTON_RIGHT) button = MOUSE_RIGHT;
		if (button == GLFW_MOUSE_BUTTON_MIDDLE) button = MOUSE_MIDDLE;

		if (button == MOUSE_LEFT || button == MOUSE_RIGHT || button == MOUSE_MIDDLE) {
			if (action == GLFW_PRESS) {
				PlatformEvent *event = createPlatformEvent(PLATFORM_EVENT_KEY_DOWN);
				event->keyCode = button;
			} else if (action == GLFW_RELEASE) {
				PlatformEvent *event = createPlatformEvent(PLATFORM_EVENT_KEY_UP);
				event->keyCode = button;
			}
		}
	};
	glfwSetMouseButtonCallback(_glfwWindow, mouseButtonCallback);

	auto scrollCallback = [](GLFWwindow *window, double xoffset, double yoffset) {
		PlatformEvent *event = createPlatformEvent(PLATFORM_EVENT_MOUSE_WHEEL);
		event->wheelValue = yoffset;
	};
	glfwSetScrollCallback(_glfwWindow, scrollCallback);

	auto charCallback = [](GLFWwindow *window, u32 character) {
		PlatformEvent *event = createPlatformEvent(PLATFORM_EVENT_INPUT_CHARACTER);
		event->keyCode = character;
	};
	glfwSetCharCallback(_glfwWindow, charCallback);
}

float backendPlatformGetWindowScaling() {
	float scaleX, scaleY;
	glfwGetWindowContentScale(_glfwWindow, &scaleX, &scaleY);
	return scaleX;
}

void backendPlatformStartUpdateLoop(void (*callback)()) {
	_platformFrontendUpdateCallback = callback;

	for (;;) {
    backendPlatformUpdateLoop();
		if (glfwWindowShouldClose(_glfwWindow)) break;
  }
}

void backendPlatformExit() {
	glfwSetWindowShouldClose(_glfwWindow, GLFW_TRUE);
}

void backendPlatformUpdateLoop() {
	glfwPollEvents();
	_platformFrontendUpdateCallback();
	glfwSwapBuffers(_glfwWindow);
}

int backendPlatformGetWindowWidth() {
	int width, height;
	glfwGetWindowSize(_glfwWindow, &width, &height);
  return width;
}

int backendPlatformGetWindowHeight() {
	int width, height;
	glfwGetWindowSize(_glfwWindow, &width, &height);
  return height;
}

void backendPlatformMaximizeWindow() { glfwMaximizeWindow(_glfwWindow); }
void backendPlatformMinimizeWindow() { glfwIconifyWindow(_glfwWindow); }
void backendPlatformRestoreWindow() { glfwRestoreWindow(_glfwWindow); }
void backendPlatformResizeWindow(int width, int height) { glfwSetWindowSize(_glfwWindow, width, height); }
void backendHideCursor() { glfwSetInputMode(_glfwWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN); }
void backendShowCursor() { glfwSetInputMode(_glfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL); }

void backendPlatformSleep(int ms) { Sleep(ms); }

BackendNanoTime backendPlatformGetNanoTime() {
	BackendNanoTime time = {};
	time.time = glfwGetTime();
	return time;
}

float backendPlatformGetMsPassed(BackendNanoTime startTime) {
	BackendNanoTime endTime = backendPlatformGetNanoTime();
	return (endTime.time - startTime.time) * 1000;
}

void backendPlatformSetClipboard(char *str) { glfwSetClipboardString(_glfwWindow, str); }
void backendPlatformNavigateToUrl(char *url) { system(frameSprintf("explorer %s", url)); }
void backendPlatformShowErrorWindow(char *msg) { MessageBoxA(NULL, msg, "Error", MB_OK); }

char *backendPlatformGetLastErrorMessage() {
	DWORD errorMessageID = ::GetLastError();
	if (errorMessageID) {
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

		char *ret = frameStringClone(messageBuffer);

		LocalFree(messageBuffer);

		return ret;
	}

	return "No error";
}

int backendPlatformGetMemoryUsage() {
#if defined(_WIN32) && defined(FALLOW_DEBUG)
	PROCESS_MEMORY_COUNTERS pmc;
	if (GetProcessMemoryInfo(_processHandle, &pmc, sizeof(pmc))) return pmc.WorkingSetSize;
	return 0;
#endif
}

void backendPlatformImGuiInit() {
	ImGui_ImplOpenGL3_Init("#version 300 es");
}

void backendPlatformImGuiStartFrame(int windowWidth, int windowHeight) {
	ImGui_ImplOpenGL3_NewFrame();
}

void backendPlatformImGuiDraw() {
	ImDrawData *data = ImGui::GetDrawData();
	if (data->CmdListsCount == 0) return;

	ImGui_ImplOpenGL3_RenderDrawData(data);
}

PlatformEvent *createPlatformEvent(PlatformEventType type) {
	if (_platformEventsNum > PLATFORM_EVENTS_MAX-1) {
		logf("Too many platform events!\n");
		_platformEventsNum--;
	}

	PlatformEvent *event = &_platformEvents[_platformEventsNum++];
	memset(event, 0, sizeof(PlatformEvent));
	event->type = type;
	return event;
}
