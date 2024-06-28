#include "platformBackend.h"

#include <GLES3/gl3.h>

#define IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#include "backends/imgui_impl_opengl3.cpp"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

struct BackendNanoTime {
	timespec timeSpec;
};

void (*_platformFrontendUpdateCallback)();
PlatformEvent _heldPlatformEvents[PLATFORM_EVENTS_MAX];
int _heldPlatformEventsNum;

Rect _canvasRect;
Vec2 _canvasSize;

void doCanvasElementResize();
void backendPlatformUpdateLoop();

void backendPlatformInit(int windowWidth, int windowHeight, char *windowTitle) {
	EM_ASM({
		let ctx = Module.createContext(canvas, true, true);
		if (!ctx) console.log("Failed to create opengl context!\n");

		let theStatusElement = document.getElementById("status");
		if (theStatusElement) theStatusElement.style.display = "none";

		let fullscreenButton = document.createElement("button");
		fullscreenButton.textContent = "Fullscreen";
		fullscreenButton.id = "fullscreenButton";
		document.body.insertBefore(fullscreenButton, canvas);
		// document.body.appendChild(fullscreenButton);

		// fullscreenButton.addEventListener("click", function(e) {
		// 	let result = canvas.requestFullscreen();
		// 	result.then(() => {console.log("It worked?\n");});
		// 	result.catch((err) => {console.log("It didn't work?\n"+err);});
		// });
	});

	auto fullscreenButtonCallback = [](int eventType, const EmscriptenMouseEvent *emEvent, void *userData)->EM_BOOL {
		logf("Trying fullscreen\n");
		EmscriptenFullscreenStrategy strat;
		// strat.scaleMode = EMSCRIPTEN_FULLSCREEN_SCALE_ASPECT;
		strat.scaleMode = EMSCRIPTEN_FULLSCREEN_SCALE_STRETCH;
		// strat.canvasResolutionScaleMode = EMSCRIPTEN_FULLSCREEN_CANVAS_SCALE_STDDEF;
		strat.canvasResolutionScaleMode = EMSCRIPTEN_FULLSCREEN_CANVAS_SCALE_HIDEF;
		// strat.canvasResolutionScaleMode = EMSCRIPTEN_FULLSCREEN_CANVAS_SCALE_NONE;
		// strat.filteringMode = EMSCRIPTEN_FULLSCREEN_FILTERING_BILINEAR;
		strat.filteringMode = EMSCRIPTEN_FULLSCREEN_FILTERING_DEFAULT;
		emscripten_request_fullscreen_strategy("#canvas", true, &strat);

		return EM_TRUE;
	};

	emscripten_set_click_callback("#fullscreenButton", NULL, true, fullscreenButtonCallback);

	auto fullscreenChangeCallback = [](int eventType, const EmscriptenFullscreenChangeEvent *emEvent, void *userData)->EM_BOOL {
		// doCanvasElementResize();
		return EM_TRUE;
	};
	emscripten_set_fullscreenchange_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, NULL, true, fullscreenChangeCallback);

	auto keyCallback = [](int eventType, const EmscriptenKeyboardEvent *emEvent, void *userData)->EM_BOOL {
		if (_heldPlatformEventsNum > PLATFORM_EVENTS_MAX-1) {
			logf("Too many platform events!\n");
			return EM_TRUE;
		}

		if (eventType == EMSCRIPTEN_EVENT_KEYDOWN) {
			PlatformEvent *event = &_heldPlatformEvents[_heldPlatformEventsNum++];
			memset(event, 0, sizeof(PlatformEvent));
			event->type = PLATFORM_EVENT_KEY_DOWN;
			event->keyCode = emEvent->key[0];
			if (isalpha(event->keyCode)) event->keyCode = toUpperCase(event->keyCode);
		} else if (eventType == EMSCRIPTEN_EVENT_KEYUP) {
			PlatformEvent *event = &_heldPlatformEvents[_heldPlatformEventsNum++];
			memset(event, 0, sizeof(PlatformEvent));
			event->type = PLATFORM_EVENT_KEY_UP;
			event->keyCode = emEvent->key[0];
			if (isalpha(event->keyCode)) event->keyCode = toUpperCase(event->keyCode);
		}
		return EM_TRUE;
	};
	emscripten_set_keydown_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, NULL, true, keyCallback);
	emscripten_set_keyup_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, NULL, true, keyCallback);

	auto resizeCallback = [](int eventType, const EmscriptenUiEvent *emEvent, void *userData)->EM_BOOL {
		doCanvasElementResize();
		return EM_TRUE;
	};
	emscripten_set_resize_callback("#canvas", NULL, true, resizeCallback);

	auto mouseCallback = [](int eventType, const EmscriptenMouseEvent *emEvent, void *userData)->EM_BOOL {
		if (eventType == EMSCRIPTEN_EVENT_MOUSEMOVE) {
			PlatformEvent *event = &_heldPlatformEvents[_heldPlatformEventsNum++];
			memset(event, 0, sizeof(PlatformEvent));
			event->type = PLATFORM_EVENT_MOUSE_MOVE;
			event->position.x = (emEvent->clientX - _canvasRect.x) * (_canvasSize.x / _canvasRect.width);
			event->position.y = (emEvent->clientY - _canvasRect.y) * (_canvasSize.y / _canvasRect.height);
			return true;
		} else if (eventType == EMSCRIPTEN_EVENT_MOUSEDOWN) {
			PlatformEvent *event = &_heldPlatformEvents[_heldPlatformEventsNum++];
			memset(event, 0, sizeof(PlatformEvent));
			event->type = PLATFORM_EVENT_KEY_DOWN;
			event->keyCode = MOUSE_LEFT;
			return true;
		} else if (eventType == EMSCRIPTEN_EVENT_MOUSEUP) {
			PlatformEvent *event = &_heldPlatformEvents[_heldPlatformEventsNum++];
			memset(event, 0, sizeof(PlatformEvent));
			event->type = PLATFORM_EVENT_KEY_UP;
			event->keyCode = MOUSE_LEFT;
			return true;
		}
		return false;
	};
	emscripten_set_mousemove_callback("#canvas", NULL, true, mouseCallback);
	emscripten_set_mousedown_callback("#canvas", NULL, true, mouseCallback);
	emscripten_set_mouseup_callback("#canvas", NULL, true, mouseCallback);

	{
		auto touchMoveCallback = [](int eventType, const EmscriptenTouchEvent *emEvent, void *userData)->EM_BOOL {
			for (int i = 0; i < emEvent->numTouches; i++) {
				const EmscriptenTouchPoint *touchPoint = &emEvent->touches[i];
				PlatformEvent *event = &_heldPlatformEvents[_heldPlatformEventsNum++];
				memset(event, 0, sizeof(PlatformEvent));
				event->type = PLATFORM_EVENT_MOUSE_MOVE;
				event->position.x = (touchPoint->clientX - _canvasRect.x) * (_canvasSize.x / _canvasRect.width);
				event->position.y = (touchPoint->clientY - _canvasRect.y) * (_canvasSize.y / _canvasRect.height);
			}
			return true;
		};
		emscripten_set_touchmove_callback("#canvas", NULL, true, touchMoveCallback);

		auto touchStartCallback = [](int eventType, const EmscriptenTouchEvent *emEvent, void *userData)->EM_BOOL {
			for (int i = 0; i < emEvent->numTouches; i++) {
				const EmscriptenTouchPoint *touchPoint = &emEvent->touches[i];
				PlatformEvent *event = &_heldPlatformEvents[_heldPlatformEventsNum++];
				memset(event, 0, sizeof(PlatformEvent));
				event->type = PLATFORM_EVENT_MOUSE_MOVE;
				event->position.x = (touchPoint->clientX - _canvasRect.x) * (_canvasSize.x / _canvasRect.width);
				event->position.y = (touchPoint->clientY - _canvasRect.y) * (_canvasSize.y / _canvasRect.height);

				event = &_heldPlatformEvents[_heldPlatformEventsNum++];
				memset(event, 0, sizeof(PlatformEvent));
				event->type = PLATFORM_EVENT_KEY_DOWN;
				event->keyCode = MOUSE_LEFT;
			}
			return true;
		};
		emscripten_set_touchstart_callback("#canvas", NULL, true, touchStartCallback);

		auto touchEndCallback = [](int eventType, const EmscriptenTouchEvent *emEvent, void *userData)->EM_BOOL {
			for (int i = 0; i < emEvent->numTouches; i++) {
				PlatformEvent *event = &_heldPlatformEvents[_heldPlatformEventsNum++];
				memset(event, 0, sizeof(PlatformEvent));
				event->type = PLATFORM_EVENT_KEY_UP;
				event->keyCode = MOUSE_LEFT;
			}
			return true;
		};
		emscripten_set_touchend_callback("#canvas", NULL, true, touchEndCallback);
	}

	auto wheelCallback = [](int eventType, const EmscriptenWheelEvent *emEvent, void *userData)->EM_BOOL {
		PlatformEvent *event = &_heldPlatformEvents[_heldPlatformEventsNum++];
		memset(event, 0, sizeof(PlatformEvent));
		event->type = PLATFORM_EVENT_MOUSE_WHEEL;
		event->wheelValue = emEvent->deltaY * -0.05;
		return true;
	};
	emscripten_set_wheel_callback("#canvas", NULL, true, wheelCallback);

	float windowScaling = backendPlatformGetWindowScaling();
	emscripten_set_canvas_element_size("#canvas", windowWidth, windowHeight);
	emscripten_set_element_css_size("#canvas", windowWidth/windowScaling, windowHeight/windowScaling);
}

float backendPlatformGetWindowScaling() {
	return emscripten_get_device_pixel_ratio();
}

void backendPlatformStartUpdateLoop(void (*callback)()) {
	_platformFrontendUpdateCallback = callback;
  emscripten_set_main_loop(backendPlatformUpdateLoop, 60, 1);
}

void backendPlatformExit() { }

void doCanvasElementResize() {
	double canvasWidth, canvasHeight;
	float windowScaling = backendPlatformGetWindowScaling();
	emscripten_get_element_css_size("#canvas", &canvasWidth, &canvasHeight);
	emscripten_set_canvas_element_size("#canvas", canvasWidth*windowScaling, canvasHeight*windowScaling);
}

float _lastWindowScaling = 1;
void backendPlatformUpdateLoop() {
	float windowScaling = backendPlatformGetWindowScaling();
	if (_lastWindowScaling != windowScaling) {
		_lastWindowScaling = windowScaling;
		doCanvasElementResize();
	}

	_canvasRect.x = EM_ASM_DOUBLE({return document.getElementById("canvas").getBoundingClientRect().left});
	_canvasRect.y = EM_ASM_DOUBLE({return document.getElementById("canvas").getBoundingClientRect().top});
	_canvasRect.width = EM_ASM_DOUBLE({return document.getElementById("canvas").getBoundingClientRect().width});
	_canvasRect.height = EM_ASM_DOUBLE({return document.getElementById("canvas").getBoundingClientRect().height});
	_canvasSize.x = EM_ASM_DOUBLE({return document.getElementById("canvas").width});
	_canvasSize.y = EM_ASM_DOUBLE({return document.getElementById("canvas").height});

	memcpy(_platformEvents, _heldPlatformEvents, _heldPlatformEventsNum * sizeof(PlatformEvent));
	_platformEventsNum = _heldPlatformEventsNum;
	_heldPlatformEventsNum = 0;

	_platformFrontendUpdateCallback();
}

int backendPlatformGetWindowWidth() {
	double canvasWidth, canvasHeight;
	emscripten_get_element_css_size("#canvas", &canvasWidth, &canvasHeight);
  return canvasWidth * backendPlatformGetWindowScaling();
}

int backendPlatformGetWindowHeight() {
	double canvasWidth, canvasHeight;
	emscripten_get_element_css_size("#canvas", &canvasWidth, &canvasHeight);
	float windowScaling = backendPlatformGetWindowScaling();
  return canvasHeight * backendPlatformGetWindowScaling();
}

void backendPlatformMaximizeWindow() { }
void backendPlatformMinimizeWindow() { }
void backendPlatformRestoreWindow() { }

void backendPlatformResizeWindow(int width, int height) {
	float windowScaling = backendPlatformGetWindowScaling();
	emscripten_set_canvas_element_size("#canvas", width, height);
	emscripten_set_element_css_size("#canvas", width/windowScaling, height/windowScaling);
	// doCanvasElementResize();
}

void backendHideCursor() {
	//@incomplete
}
void backendShowCursor() {
	//@incomplete
}

void backendPlatformSleep(int ms) {
	timespec ts;
	ts.tv_sec = ms / 1000;
	ts.tv_nsec = (ms % 1000) * 1000000;
	nanosleep(&ts, NULL);
}

BackendNanoTime backendPlatformGetNanoTime() {
	BackendNanoTime time = {};
	clock_gettime(CLOCK_REALTIME, &time.timeSpec);
	return time;
}

float backendPlatformGetMsPassed(BackendNanoTime startTime) {
	BackendNanoTime endTime = backendPlatformGetNanoTime();

	float seconds = ((float)endTime.timeSpec.tv_sec - (float)startTime.timeSpec.tv_sec) + ((float)endTime.timeSpec.tv_nsec - (float)startTime.timeSpec.tv_nsec) / 1000000000.0;
	float ms = seconds * 1000.0;
	if (ms < 0) ms = 0;
	return ms;
}

void backendPlatformSetClipboard(char *str) {
	//@incomplete
}

void backendPlatformNavigateToUrl(char *url) {
	emscripten_run_script(frameSprintf("window.open('%s', '_blank')", url));
}

void backendPlatformShowErrorWindow(char *msg) {
	EM_ASM({
		var str = UTF8ToString($0);
		str = str.split("\n").join("<br />");
		crashlogElement.innerHTML += str;
	}, msg);
}

char *backendPlatformGetLastErrorMessage() {
	int myErrno = errno;
	if (!myErrno) return "No error";
	return frameStringClone(strerror(myErrno));
}

u64 backendPlatformGetMemoryUsage() {
	return EM_ASM_INT({ return HEAP8.length; });
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
