#if defined(_WIN32)
#include "gl.h"
#elif defined(__EMSCRIPTEN__)
#include <GLES3/gl3.h>
#else
#include "gl.h"
#endif

///- Gui Header
void guiInit();
void guiStartFrame();
void guiDraw();

///- Platform

struct NanoTime {
	double time;
};

struct Platform {
#ifdef _WIN32
	HANDLE processHandle;
#endif

	bool sleepWait; // Does nothing?
	bool isCommandLineOnly; // Does nothing?
	bool usingSkia; // Does nothing?

	bool isDebugVersion;
	bool isInternalVersion;
	bool disableGui;
	bool useRealElapsed;
  bool scissorDoesntRespectCamera;

	bool running;

	Vec2i originalWindowSize;
	int windowWidth;
	int windowHeight;
	Vec2i windowSize;
	float windowScaling;

	float realElapsed;
	float elapsed;
	Vec2 mouse;
	bool mouseDown;
	bool mouseJustDown;
	bool mouseJustUp;
	int mouseWheel;
	bool rightMouseDown;
	bool rightMouseJustDown;
	bool rightMouseJustUp;
	bool justTapped;
	bool useRelativeMouse; //@todo Actually make this work
	Vec2 relativeMouse; //@todo Actually make this work
	bool windowHasFocus; //@todo Actually make this work

	bool hoveringGui;
	bool typingGui;

	float time;
	int frameCount;

	NanoTime frameNano;

	float *frameTimes;
	int frameTimesMax;
	float frameTimeAvg;
	float frameTimeHighest;

	int memoryUsage;

#if defined(_WIN32)
	LARGE_INTEGER performanceFrequency;
#endif

	void (*updateCallback)();
};

enum PlatformKey {
	KEY_LEFT=Raylib::KEY_LEFT,
	KEY_RIGHT=Raylib::KEY_RIGHT,
	KEY_UP=Raylib::KEY_UP,
	KEY_DOWN=Raylib::KEY_DOWN,
	KEY_SHIFT=Raylib::KEY_LEFT_SHIFT,
	KEY_RIGHT_SHIFT=Raylib::KEY_RIGHT_SHIFT,
	KEY_BACKSPACE=Raylib::KEY_BACKSPACE,
	KEY_CTRL=Raylib::KEY_LEFT_CONTROL,
	KEY_RIGHT_CTRL=Raylib::KEY_RIGHT_CONTROL,
	KEY_ALT=Raylib::KEY_LEFT_ALT,
	KEY_RIGHT_ALT=Raylib::KEY_RIGHT_ALT,
	KEY_BACKTICK=Raylib::KEY_GRAVE,
	KEY_ESC=Raylib::KEY_ESCAPE,
	KEY_F1=Raylib::KEY_F1,
	KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6, KEY_F7, KEY_F8, KEY_F9, KEY_F10, KEY_F11, KEY_F12,
	KEY_ENTER=Raylib::KEY_ENTER,
};

enum JoyButtons {
	JOY_X = Raylib::GAMEPAD_BUTTON_RIGHT_FACE_DOWN,
	JOY_CIRCLE = Raylib::GAMEPAD_BUTTON_RIGHT_FACE_RIGHT,
	JOY_SQUARE = Raylib::GAMEPAD_BUTTON_RIGHT_FACE_LEFT,
	JOY_TRIANGLE = Raylib::GAMEPAD_BUTTON_RIGHT_FACE_UP,
	JOY_PAD_LEFT = Raylib::GAMEPAD_BUTTON_LEFT_FACE_LEFT,
	JOY_PAD_RIGHT = Raylib::GAMEPAD_BUTTON_LEFT_FACE_RIGHT,
	JOY_PAD_UP = Raylib::GAMEPAD_BUTTON_LEFT_FACE_UP,
	JOY_PAD_DOWN = Raylib::GAMEPAD_BUTTON_LEFT_FACE_DOWN,
	JOY_L1 = Raylib::GAMEPAD_BUTTON_LEFT_TRIGGER_1,
	JOY_L2 = Raylib::GAMEPAD_BUTTON_LEFT_TRIGGER_2,
	JOY_L3 = Raylib:: GAMEPAD_BUTTON_LEFT_THUMB,
	JOY_R1 = Raylib::GAMEPAD_BUTTON_RIGHT_TRIGGER_1,
	JOY_R2 = Raylib::GAMEPAD_BUTTON_RIGHT_TRIGGER_2,
	JOY_R3 = Raylib:: GAMEPAD_BUTTON_RIGHT_THUMB,
	JOY_START = Raylib::GAMEPAD_BUTTON_MIDDLE_RIGHT,
	JOY_SELECT = Raylib::GAMEPAD_BUTTON_MIDDLE_LEFT,
};

Platform *platform = NULL;
bool defeatWindowsScalingInHtml5 = false;

void initPlatform(int windowWidth, int windowHeight, char *windowTitle);

void platformUpdateLoop(void (*updateCallback)());
void platformUpdate();

void maximizeWindow() { Raylib::MaximizeWindow(); }
void minimizeWindow() { Raylib::MinimizeWindow(); }
void restoreWindow() { Raylib::RestoreWindow(); }
bool keyPressed(int key);
bool keyJustPressed(int key);
bool keyJustReleased(int key);
bool joyButtonPressed(int controllerId, int button);
bool joyButtonJustReleased(int controllerId, int button);
bool joyButtonJustPressed(int controllerId, int button);
Vec2 joyLeftStick(int controllerId);
Vec2 joyRightStick(int controllerId);
void setClipboard(char *str);
void resizeWindow(int width, int height);
void platformSleep(int ms);
NanoTime getNanoTime();
float getMsPassed(NanoTime startTime);
void navigateToUrl(const char *url);
void showErrorWindow(char *msg);

void pngQuantImage(const char *path);

/// FUNCTIONS ^

void initPlatform(int windowWidth, int windowHeight, char *windowTitle) {
	Raylib::SetTraceLogLevel(Raylib::LOG_WARNING);

	platform = (Platform *)zalloc(sizeof(Platform));
	platform->windowWidth = windowWidth;
	platform->windowHeight = windowHeight;

#ifdef __EMSCRIPTEN__
	if (defeatWindowsScalingInHtml5) {
		float html5ExtraScaling = emscripten_get_device_pixel_ratio();
		platform->windowWidth *= html5ExtraScaling;
		platform->windowHeight *= html5ExtraScaling;
	}
#endif

	platform->windowSize = v2i(platform->windowWidth, platform->windowHeight);
	platform->originalWindowSize = platform->windowSize;

#ifdef _WIN32
	platform->processHandle = GetCurrentProcess();
	QueryPerformanceFrequency(&platform->performanceFrequency);
#endif

	pushRndSeed(time(NULL));

#if defined(FALLOW_COMMAND_LINE_ONLY)
	platform->isCommandLineOnly = true;
#endif

#if defined(FALLOW_DEBUG)
	platform->isDebugVersion = true;
#endif

#if defined(FALLOW_INTERNAL)
	platform->isInternalVersion = true;
#endif

	Raylib::SetWindowState(Raylib::FLAG_WINDOW_RESIZABLE);
	Raylib::InitWindow(platform->windowWidth, platform->windowHeight, windowTitle);

#ifdef __EMSCRIPTEN__
	if (defeatWindowsScalingInHtml5) {
		EM_ASM({
			let newWidth = $0;
			let newHeight = $1;
			let canvasElement = document.getElementById("canvas");
			canvasElement.style.width = newWidth+"px";
			canvasElement.style.height = newHeight+"px";
		}, windowWidth, windowHeight);
	}
#endif

	platform->windowScaling = Raylib::GetWindowScaleDPI().x;

	guiInit();

	platform->frameTimesMax = 60;
	platform->frameTimes = (float *)malloc(sizeof(float) * platform->frameTimesMax);
}

void platformUpdateLoop(void (*updateCallback)()) {
	platform->updateCallback = updateCallback;
	platform->running = true;

	Raylib::SetExitKey(0);

#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop(platformUpdate, 0, 1);
#else
	Raylib::SetTargetFPS(60); //@todo Does targetFps matter?

	while (platform->running) {
		platformUpdate();
		if (Raylib::WindowShouldClose()) platform->running = false;
	}
#endif
}

void platformUpdate() {
	if (platform->frameCount == 0) {
		platform->realElapsed = 1.0/60.0;
	} else {
		platform->realElapsed = getMsPassed(platform->frameNano) / 1000.0;
	}

	if (platform->useRealElapsed) {
		platform->elapsed = platform->realElapsed;
		if (platform->elapsed < 0.0001) platform->elapsed = 0.0001;
		if (platform->elapsed > 1/15.0) platform->elapsed = 1/15.0;
	} else {
		platform->elapsed = 1/60.0;
	}

	platform->frameNano = getNanoTime();

	platform->windowWidth = Raylib::GetScreenWidth();
	platform->windowHeight = Raylib::GetScreenHeight();

#ifdef __EMSCRIPTEN__
		// float windowScaling = emscripten_get_device_pixel_ratio();
		// platform->windowWidth *= windowScaling;
		// platform->windowHeight *= windowScaling;
		// if (platform->windowSize.x != platform->windowWidth || platform->windowSize.y != platform->windowHeight) {
		// 	EM_ASM({
		// 		let newWidth = $0;
		// 		let newHeight = $1;
		// 		let canvasElement = document.getElementById("canvas");
		// 		canvasElement.width = newWidth;
		// 		canvasElement.height = newHeight;
		// 	}, platform->windowWidth, platform->windowHeight);
		// }
#endif

	platform->windowSize = v2i(platform->windowWidth, platform->windowHeight);

	{ /// Events
		platform->mouseJustDown = Raylib::IsMouseButtonPressed(Raylib::MOUSE_BUTTON_LEFT);
		platform->mouseJustUp = Raylib::IsMouseButtonReleased(Raylib::MOUSE_BUTTON_LEFT);
		platform->mouseDown = Raylib::IsMouseButtonDown(Raylib::MOUSE_BUTTON_LEFT);
		platform->rightMouseJustDown = Raylib::IsMouseButtonPressed(Raylib::MOUSE_BUTTON_RIGHT);
		platform->rightMouseJustUp = Raylib::IsMouseButtonReleased(Raylib::MOUSE_BUTTON_RIGHT);
		platform->rightMouseDown = Raylib::IsMouseButtonDown(Raylib::MOUSE_BUTTON_RIGHT);
		platform->mouseWheel = Raylib::GetMouseWheelMove();

#ifdef __EMSCRIPTEN__
		platform->mouseWheel *= -1;
		// platform->mouseWheel *= -1/60.0;
#endif

		Raylib::Vector2 mouse = Raylib::GetMousePosition();
		platform->mouse.x = mouse.x;
		platform->mouse.y = mouse.y;

		int gesture = Raylib::GetGestureDetected();
		platform->justTapped = false;
		if (gesture == Raylib::GESTURE_TAP) {
			Raylib::Vector2 point = Raylib::GetTouchPosition(0);
			platform->mouse.x = point.x;
			platform->mouse.y = point.y;
			platform->justTapped = true;
		}

		if (platform->hoveringGui) {
			platform->mouseJustUp = false;
			platform->mouseJustDown = false;
			platform->mouseDown = false;
			platform->rightMouseJustUp = false;
			platform->rightMouseJustDown = false;
			platform->rightMouseDown = false;
			platform->mouseWheel = 0;
		}
	} ///

	void updateAudio(float elapsed); //@headerHack
	updateAudio(platform->realElapsed);

	void startRenderingFrame(); //@headerHack
	startRenderingFrame();
	guiStartFrame();

	void nguiStartFrame(); //@headerHack
	nguiStartFrame();

	platform->updateCallback();

	guiDraw();
	Raylib::rlDrawRenderBatchActive();

	void endRenderingFrame(); //@headerHack
	endRenderingFrame();

	if (keyPressed(KEY_CTRL) && keyPressed('Q')) platform->running = false;

	freeFrameMemory();

	void freeFrameTextures(); //@headerHack
	freeFrameTextures();

	{ // Calcuate frame times
		platform->frameTimes[platform->frameCount % platform->frameTimesMax] = getMsPassed(platform->frameNano);

		platform->frameTimeAvg = 0;
		platform->frameTimeHighest = 0;
		for (int i = 0; i < platform->frameTimesMax; i++) {
			platform->frameTimeAvg += platform->frameTimes[i];
			if (platform->frameTimeHighest < platform->frameTimes[i]) platform->frameTimeHighest = platform->frameTimes[i];
		}
		platform->frameTimeAvg /= platform->frameTimesMax;
	}

	{ // Calculate memory usage
#if defined(_WIN32) && defined(FALLOW_DEBUG)
    PROCESS_MEMORY_COUNTERS pmc;
		if (GetProcessMemoryInfo(platform->processHandle, &pmc, sizeof(pmc))) {
			platform->memoryUsage = pmc.WorkingSetSize;
		}
#endif
	}

	Raylib::EndDrawing();

	platform->frameCount++;
	platform->time += platform->elapsed;
	logSys->time = platform->time;
}

void setClipboard(char *str) {
	Raylib::SetClipboardText(str);
}

void resizeWindow(int width, int height) {
	Raylib::SetWindowSize(width, height);
}

bool keyPressed(int key) {
	if (platform->typingGui) return false;
	return Raylib::IsKeyDown(key);
}

bool keyJustPressed(int key) {
	if (platform->typingGui) return false;
	return Raylib::IsKeyPressed(key);
}

bool keyJustReleased(int key) {
	if (platform->typingGui) return false;
	return Raylib::IsKeyReleased(key);
}

bool joyButtonPressed(int controllerId, int button) {
	if (!Raylib::IsGamepadAvailable(controllerId)) return false;
	bool ret = Raylib::IsGamepadButtonDown(controllerId, button);
	return ret;
}

bool joyButtonJustReleased(int controllerId, int button) {
	if (!Raylib::IsGamepadAvailable(controllerId)) return false;
	bool ret = Raylib::IsGamepadButtonReleased(controllerId, button);
	return ret;
}

bool joyButtonJustPressed(int controllerId, int button) {
	if (!Raylib::IsGamepadAvailable(controllerId)) return false;
	bool ret = Raylib::IsGamepadButtonPressed(controllerId, button);
	return ret;
}

Vec2 joyLeftStick(int controllerId) {
	if (!Raylib::IsGamepadAvailable(controllerId)) return v2();

	Vec2 ret;
	ret.x = Raylib::GetGamepadAxisMovement(controllerId, Raylib::GAMEPAD_AXIS_LEFT_X);
	ret.y = Raylib::GetGamepadAxisMovement(controllerId, Raylib::GAMEPAD_AXIS_LEFT_Y);
	return ret;
}

Vec2 joyRightStick(int controllerId) {
	if (!Raylib::IsGamepadAvailable(controllerId)) return v2();

	Vec2 ret;
	ret.x = Raylib::GetGamepadAxisMovement(controllerId, Raylib::GAMEPAD_AXIS_RIGHT_X);
	ret.y = Raylib::GetGamepadAxisMovement(controllerId, Raylib::GAMEPAD_AXIS_RIGHT_Y);
	return ret;
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

NanoTime getNanoTime() {
	NanoTime time = {};

	time.time = Raylib::GetTime();

	return time;
}

float getMsPassed(NanoTime startTime) {
	NanoTime endTime = getNanoTime();

	return (endTime.time - startTime.time) * 1000;
}

void navigateToUrl(const char *url) {
	Raylib::OpenURL(url);
}

void showErrorWindow(char *msg) {
	logf("Error window: %s\n", msg);
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

void pngQuantImage(const char *path) {
	logf("No png quant image on raylib\n");
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

///- Renderer
#include "rendererBackend.cpp"

enum BlendMode {
	BLEND_NORMAL,
	BLEND_MULTIPLY,
	BLEND_SCREEN,
	BLEND_ADD,
	BLEND_INVERT,
	BLEND_RECOMMENDED,
	BLEND_SKIA,
};

struct Texture {
	BackendTexture backendTexture;
	// Raylib::Texture2D raylibTexture;
	int width;
	int height;

	bool clampedX;
	bool clampedY;
	char *path;
};

struct RenderProps {
	bool disabled;

	Matrix3 matrix;
	float alpha;
	int tint;

	int srcWidth;
	int srcHeight;

#define _F_CIRCLE (1 << 6)
	u32 flags;

	Vec2 uv0;
	Vec2 uv1;
	Matrix3 uvMatrix;
};

struct Camera {
	Vec3 position;
	Vec3 target;
	Vec3 up;
	float fovy;
	bool isOrtho;

	Vec2 size;
	float nearCull;
	float farCull;
};

struct Shader {
	BackendShader backendShader;
	// int shaderId;
	// int locs[RL_MAX_SHADER_LOCATIONS];
};

#define MAX_LIGHTS 4

struct Light {
	int type;
	Raylib::Vector3 position;
	Raylib::Vector3 target;
	Raylib::Color color;
	bool enabled;

	int enabledLoc;
	int typeLoc;
	int posLoc;
	int targetLoc;
	int colorLoc;
};

enum LightType {
	LIGHT_DIRECTIONAL,
	LIGHT_POINT
};

static int lightsCount = 0;


struct Renderer {
	bool in3dPass;
	bool disabled;
	int maxTextureUnits; // Does nothing

	bool lastBackfaceCull;
	BlendMode lastBlendMode;

	Shader *lightingAnimatedShader;
	int lightingAnimatedShaderBoneTransformsLoc;

	Shader *alphaDiscardShader;

	Shader *outlineShader;
	int outlineShaderResolutionLoc;
	int outlineShaderOutlineSizeLoc;
	int outlineShaderOutlineColorLoc;
	int outlineShaderOutlineFadeOuterLoc;
	int outlineShaderOutlineFadeInnerLoc;

#define TARGET_TEXTURE_LIMIT 16
	Texture *targetTextureStack[TARGET_TEXTURE_LIMIT];
	int targetTextureStackNum;

#define CAMERA_2D_STACK_MAX 128
	Matrix3 camera2dStack[CAMERA_2D_STACK_MAX];
	int camera2dStackNum;

#define ALPHA_STACK_MAX 128
	float alphaStack[ALPHA_STACK_MAX];
	int alphaStackNum;

	Matrix3 baseMatrix2d;

	Texture *whiteTexture;
	Texture *circleTexture1024;
	Texture *circleTexture32;
	Texture *linearGrad256;

	Light lights[MAX_LIGHTS];

	void *tempTextureBuffer;
	int tempTextureBufferSize;
	void *tempTextureRowBuffer;
	int tempTextureRowBufferSize;

  bool defaultSetSrcWidthAndSrcHeightTo1;
};

Renderer *renderer = NULL;
bool skip3dShaders = false;

// #define PASS_HEADER
// #include "pass.cpp"
// #include "pass.cpp"

Raylib::Vector3 toRaylib(Vec3 vec) { return {vec.x, vec.y, vec.z}; }
Raylib::Vector3 toRaylib(Vec2 vec) { return {vec.x, vec.y}; }
Vec3 v3(Raylib::Vector3 vec) { return v3(vec.x, vec.y, vec.z); }
Vec2 v2(Raylib::Vector2 vec) { return v2(vec.x, vec.y); }
Raylib::Matrix toRaylib(Matrix4 matrix) {
	Raylib::Matrix raylibMatrix = {};
	memcpy(&raylibMatrix.m0, matrix.transpose().data, sizeof(float) * 16);
	return raylibMatrix;
}

void initRenderer(int width, int height);
void clearRenderer(int color=0);

Shader *loadShader(char *vsPath, char *fsPath, char *vs100Path=NULL, char *fs100Path=NULL);
int getUniformLocation(Shader *shader, char *uniformName);
int getVertextAttribLocation(Shader *shader, char *attribName);
bool setShaderUniform(Shader *shader, int loc, void *ptr, ShaderUniformType type, int count);

Texture *createTexture(const char *path, int flags=0);
Texture *createTexture(int width, int height, void *data=NULL, int flags=0);

void setTextureSmooth(Texture *texture, bool smooth);
void setTextureClamped(Texture *texture, bool clamped);
void setTextureClampedX(Texture *texture, bool clamped);
void setTextureClampedY(Texture *texture, bool clamped);

void setTextureData(Texture *texture, void *data, int width, int height, int flags=0);
u8 *getTextureData(Texture *texture, int flags=0);
bool writeTextureToFile(Texture *texture, char *path);

void destroyTexture(Texture *texture);

void drawTexture(Texture *texture, RenderProps props);
void drawSimpleTexture(Texture *texture);
void drawSimpleTexture(Texture *texture, Matrix3 matrix, Vec2 uv0=v2(0, 0), Vec2 uv1=v2(1, 1), float alpha=1);
void drawRect(Rect rect, int color, int flags=0);
void drawCircle(Vec2 position, float radius, int color);
// void drawBillboard(Camera camera, Texture *texture, Vec3 position, Vec2 size=v2(), int tint=0xFFFFFFFF, Rect source=makeRect());

void pushTargetTexture(Texture *renderTexture);
void popTargetTexture();

void pushCamera2d(Matrix3 mat);
void popCamera2d();
void refreshGlobalMatrices();

void setScissor(Rect rect);
void clearScissor();

void pushAlpha(float alpha);
void popAlpha();

void setRendererBlendMode(BlendMode blendMode);
void setDepthTest(bool enabled);
void setDepthMask(bool enabled);
void setBlending(bool enabled);
void setBackfaceCulling(bool enabled);

Light createLight(int number, int type, Raylib::Vector3 position, Raylib::Vector3 target, Raylib::Color color, Shader *shader);
void updateLightValues(Shader *shader, Light light);
void updateLightingShader(Camera camera);

void processBatchDraws();
void resetRenderContext();

void startRenderingFrame();
void endRenderingFrame();
void start3d(Camera camera);
void end3d();
// void startShader(Raylib::Shader shader);
void startShader(Shader *shader);
void endShader();
void getMouseRay(Camera camera, Vec2 mouse, Vec3 *outPos, Vec3 *outDir);
Vec2 worldSpaceTo2dNDC01(Camera camera, Vec3 worldPosition);

#include "rendererUtils.cpp"
bool usesAlphaDiscard = false;

void initRenderer(int width, int height) {
	renderer = (Renderer *)zalloc(sizeof(Renderer));

	backendInit();

	pushCamera2d(mat3());
	pushAlpha(1);
	setRendererBlendMode(BLEND_NORMAL);
	setBackfaceCulling(false);

	{ /// Setup shaders
		if (!skip3dShaders) for (int i = 0; i < 30; i++) logf("There's no 3d shaders to not skip\n");
		// if (!skip3dShaders) {
		// 	// renderer->alphaDiscardShader = loadShader(
		// 	// 	NULL, "assets/common/shaders/raylib/glsl330/alphaDiscard.fs",
		// 	// 	NULL, "assets/common/shaders/raylib/glsl100/alphaDiscard.fs"
		// 	// );

		// 	{
		// 		renderer->lightingAnimatedShader = loadShader(
		// 			"assets/common/shaders/raylib/glsl330/lightingAnimated.vs",
		// 			"assets/common/shaders/raylib/glsl330/lightingAnimated.fs",
		// 			"assets/common/shaders/raylib/glsl100/lightingAnimated.vs",
		// 			"assets/common/shaders/raylib/glsl100/lightingAnimated.fs"
		// 		);

		// 		renderer->lightingAnimatedShaderBoneTransformsLoc = getUniformLocation(renderer->lightingAnimatedShader, "boneTransforms");

		// 		renderer->lightingAnimatedShader->raylibShader.locs[Raylib::SHADER_LOC_VECTOR_VIEW] = getUniformLocation(renderer->lightingAnimatedShader, "viewPos");
		// 		renderer->lightingAnimatedShader->raylibShader.locs[Raylib::SHADER_LOC_COLOR_AMBIENT] = getUniformLocation(renderer->lightingAnimatedShader, "ambient");
		// 		renderer->lightingAnimatedShader->raylibShader.locs[Raylib::SHADER_LOC_COLOR_SPECULAR] = getUniformLocation(renderer->lightingAnimatedShader, "colSpecular");
		// 		float ambientLightValue[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
		// 		int loc = renderer->lightingAnimatedShader->raylibShader.locs[Raylib::SHADER_LOC_COLOR_AMBIENT];
		// 		setShaderUniform(renderer->lightingAnimatedShader, loc, ambientLightValue, SHADER_UNIFORM_VEC4, 1);

		// 		renderer->lights[0] = createLight(0, LIGHT_DIRECTIONAL, { 1, -1, 1 }, {0, 0, 0}, Raylib::WHITE, renderer->lightingAnimatedShader);
		// 	}
		// }

		renderer->outlineShader = loadShader(
			NULL, "assets/common/shaders/raylib/glsl330/outline.fs",
			NULL, "assets/common/shaders/raylib/glsl100/outline.fs"
		);
		renderer->outlineShaderResolutionLoc = getUniformLocation(renderer->outlineShader, "resolution");
		renderer->outlineShaderOutlineSizeLoc = getUniformLocation(renderer->outlineShader, "outlineSize");
		renderer->outlineShaderOutlineColorLoc = getUniformLocation(renderer->outlineShader, "outlineColor");
		renderer->outlineShaderOutlineFadeOuterLoc = getUniformLocation(renderer->outlineShader, "outlineFadeOuter");
		renderer->outlineShaderOutlineFadeInnerLoc = getUniformLocation(renderer->outlineShader, "outlineFadeInner");
	} ///

	u64 whiteData = 0xFFFFFFFF;
	renderer->whiteTexture = createTexture(1, 1, &whiteData);
	renderer->circleTexture1024 = createTexture("assets/common/images/circle1024.png");
	renderer->circleTexture32 = createTexture("assets/common/images/circle32.png");

	initRendererUtils();
}

void clearRenderer(int color) {
	backendClearRenderer(color);
}

Shader *loadShader(char *vsPath, char *fsPath, char *vs100Path, char *fs100Path) {
#ifdef __EMSCRIPTEN__
	if ((vsPath && !vs100Path) || (fsPath && !fs100Path)) logf("Using glsl3.3 shader on incompatible platform (%s)\n", fsPath);
	vsPath = vs100Path;
	fsPath = fs100Path;
#endif

#ifdef __EMSCRIPTEN__
	if (vsPath == NULL) vsPath = "assets/common/shaders/raylib/glsl100/base.vs";
	if (fsPath == NULL) fsPath = "assets/common/shaders/raylib/glsl100/base.fs";
#else
	if (vsPath == NULL) vsPath = "assets/common/shaders/raylib/glsl330/base.vs";
	if (fsPath == NULL) fsPath = "assets/common/shaders/raylib/glsl330/base.fs";
#endif

	char *vs = (char *)readFile(vsPath);
	char *fs = (char *)readFile(fsPath);

	Shader *shader = (Shader *)zalloc(sizeof(Shader));
	backendLoadShader(&shader->backendShader, vs, fs);

	free(vs);
	free(fs);
	return shader;
}

int getUniformLocation(Shader *shader, char *uniformName) {
	return backendGetUniformLocation(&shader->backendShader, uniformName);
}

int getVertextAttribLocation(Shader *shader, char *attribName) {
	return backendGetVertextAttribLocation(&shader->backendShader, attribName);
}

bool setShaderUniform(Shader *shader, int loc, void *ptr, ShaderUniformType type, int count) {
	return backendSetShaderUniform(&shader->backendShader, loc, ptr, type, count);
}

Texture *createTexture(const char *path, int flags) {
	int pngSize;
	void *pngData = readFile(path, &pngSize);

	int width, height, channels;
	stbi_set_flip_vertically_on_load(true);
	stbi_uc *img = stbi_load_from_memory((unsigned char *)pngData, pngSize, &width, &height, &channels, 4);

	if (!img) return NULL;

	Texture *texture = createTexture(width, height, img, flags);
	if (!texture) Panic(frameSprintf("Failed to load image %s\n", path));

	if (path) texture->path = stringClone(path);
	free(img);
	free(pngData);

	return texture;
}

Texture *createTexture(int width, int height, void *data, int flags) {
	Texture *texture = (Texture *)zalloc(sizeof(Texture));
	texture->width = width;
	texture->height = height;

	texture->backendTexture = backendCreateTexture(width, height, flags);

	if (data) setTextureData(texture, data, width, height, flags);
	setTextureSmooth(texture, true);
	setTextureClamped(texture, true);

	return texture;
}

void setTextureSmooth(Texture *texture, bool smooth) {
	backendSetTextureSmooth(&texture->backendTexture, smooth);
}

void setTextureClamped(Texture *texture, bool clamped) {
  setTextureClampedX(texture, clamped);
  setTextureClampedY(texture, clamped);
}

void setTextureClampedX(Texture *texture, bool clamped) {
	backendSetTextureClampedX(&texture->backendTexture, clamped);
}
void setTextureClampedY(Texture *texture, bool clamped) {
	backendSetTextureClampedY(&texture->backendTexture, clamped);
}

void setTextureData(Texture *texture, void *data, int width, int height, int flags) {
	backendSetTextureData(&texture->backendTexture, data, width, height, flags);
}

u8 *getTextureData(Texture *texture, int flags) {
	return backendGetTextureData(&texture->backendTexture);
}

bool writeTextureToFile(Texture *texture, char *path) {
	u8 *bitmapData = getTextureData(texture);

	stbi_flip_vertically_on_write(true);
	if (!stbi_write_png(frameSprintf("%s%s", filePathPrefix, path), texture->width, texture->height, 4, bitmapData, texture->width*4)) {
		logf("Failed to write sprite sheet: %s\n", path);
		free(bitmapData);
		return false;
	}
	free(bitmapData);
	return true;
}

void destroyTexture(Texture *texture) {
	if (texture->path) free(texture->path);
	backendDestroyTexture(&texture->backendTexture);
	// free(texture); //@incomplete Why can't I free this???
}

void drawTexture(Texture *texture, RenderProps props) {
	if (props.alpha == 0) return;
	if (props.disabled) return;
	if (renderer->disabled) return;
	if (!texture) Panic("drawTexture called with null texture!");

	if (props.srcWidth == 0) props.srcWidth = texture->width;
	if (props.srcHeight == 0) props.srcHeight = texture->height;
	props.matrix.SCALE(props.srcWidth, props.srcHeight);

	Vec4i tints = v4i(props.tint, props.tint, props.tint, props.tint);
	float alpha = props.alpha;
	int flags = props.flags;
	Vec2 uv0 = props.uvMatrix * props.uv0;
	Vec2 uv1 = props.uvMatrix * props.uv1;
	backendDrawTexture(&texture->backendTexture, props.matrix, uv0, uv1, tints, alpha, flags);
}

void drawSimpleTexture(Texture *texture) {
	Matrix3 matrix = mat3();
	matrix.SCALE(getSize(texture));
	Vec2 uv0 = v2(0, 0);
	Vec2 uv1 = v2(1, 1);
	Vec4i tints = v4i(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF);
	int alpha = 1;
	int flags = 0;
	backendDrawTexture(&texture->backendTexture, matrix, uv0, uv1, tints, alpha, flags);
}
void drawSimpleTexture(Texture *texture, Matrix3 matrix, Vec2 uv0, Vec2 uv1, float alpha) {
	Vec4i tints = v4i(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF);
	int flags = 0;
	backendDrawTexture(&texture->backendTexture, matrix, uv0, uv1, tints, alpha, flags);
}

void drawRect(Rect rect, int color, int flags) {
	Matrix3 matrix = mat3();
	matrix.TRANSLATE(rect.x, rect.y);
	matrix.SCALE(rect.width, rect.height);

	float alpha = 1;
	Vec4i tints = v4i(color, color, color, color);
	backendDrawTexture(&renderer->whiteTexture->backendTexture, matrix, v2(0, 0), v2(1, 1), tints, alpha, flags);
}

void drawCircle(Vec2 position, float radius, int color) {
	Matrix3 matrix = mat3();
	matrix.TRANSLATE(position - radius);
	matrix.SCALE(radius*2);

	Matrix3 uvMatrix = mat3();
	float alpha = 1;
	int flags = 0;
	Vec4i tints = v4i(color, color, color, color);

	Texture *texture = renderer->circleTexture1024;
	if (radius < 45) texture = renderer->circleTexture32;

	backendDrawTexture(&texture->backendTexture, matrix, v2(0, 0), v2(1, 1), tints, alpha, flags);
}

// void drawBillboard(Camera camera, Texture *texture, Vec3 position, Vec2 size, int tint, Rect source) {
// 	if (renderer->disabled) return;
// 	if (isZero(source)) source = makeRect(0, 0, texture->width, texture->height);
// 	if (isZero(size)) size = v2(texture->width, texture->height);

// 	// I flipped uv.y
// 	Vec2 uv0; // bl For some reason
// 	uv0.x = source.x/texture->width;
// 	uv0.y = (source.y + source.height)/texture->height;

// 	Vec2 uv1; // tr For some reason
// 	uv1.x = (source.x + source.width)/texture->width;
// 	uv1.y = source.y/texture->height;

// 	if (size.x < 0) {
// 		float temp = uv0.x;
// 		uv0.x = uv1.x;
// 		uv1.x = temp;
// 		size.x *= -1;
// 	}

// 	if (size.y < 0) logf("No y flip\n");

// 	Raylib::Matrix matView = Raylib::MatrixLookAt(toRaylib(camera.position), toRaylib(camera.target), toRaylib(camera.up));

// 	Vec3 right = v3(matView.m0, matView.m4, matView.m8);
// 	Vec3 left = v3(-matView.m0, -matView.m4, -matView.m8);
// 	Vec3 up = v3(matView.m1, matView.m5, matView.m9);
// 	// Vec3 up = v3(0, 0, 1);

// 	Vec3 rightScaled = right * (size.x/2);
// 	Vec3 upScaled = up * (size.y/2);

// 	Vec3 p1 = rightScaled + upScaled;
// 	Vec3 p2 = rightScaled - upScaled;

// 	Vec3 topLeft = -p2;
// 	Vec3 topRight = p1;
// 	Vec3 bottomRight = p2;
// 	Vec3 bottomLeft = -p1;

// 	// Rotation would happen here!

// 	topLeft += position;
// 	topRight += position;
// 	bottomRight += position;
// 	bottomLeft += position;

// 	Vec3 verts[4] = { topLeft, bottomLeft, bottomRight, topRight };
// 	Vec2 uvs[4] = { v2(uv0.x, uv0.y), v2(uv0.x, uv1.y), v2(uv1.x, uv1.y), v2(uv1.x, uv0.y) };
// 	int colors[4] = { tint, tint, tint, tint };
// 	drawTexturedQuad(texture->raylibTexture.id, verts, uvs, colors);
// }

void pushTargetTexture(Texture *texture) {
	if (renderer->targetTextureStackNum >= TARGET_TEXTURE_LIMIT-1) Panic("Target texture overflow");
	renderer->targetTextureStack[renderer->targetTextureStackNum++] = texture;
  backendSetTargetTexture(&texture->backendTexture);
}

void popTargetTexture() {
	renderer->targetTextureStackNum--;
	if (renderer->targetTextureStackNum > 0) {
		Texture *texture = renderer->targetTextureStack[renderer->targetTextureStackNum-1];
		backendSetTargetTexture(&texture->backendTexture);
	} else {
		backendSetTargetTexture(NULL);
	}
}

void pushCamera2d(Matrix3 mat) {
	if (renderer->camera2dStackNum > CAMERA_2D_STACK_MAX-1) Panic("camera2d overflow");
	renderer->camera2dStack[renderer->camera2dStackNum++] = mat;
	refreshGlobalMatrices();
}

void popCamera2d() {
	if (renderer->camera2dStackNum <= 1) Panic("camera2d underflow");
	renderer->camera2dStackNum--;
	refreshGlobalMatrices();
}

void refreshGlobalMatrices() {
	renderer->baseMatrix2d = mat3();
	for (int i = 0; i < renderer->camera2dStackNum; i++) renderer->baseMatrix2d *= renderer->camera2dStack[i];
	backendSetCamera2d(renderer->baseMatrix2d);
}

void setScissor(Rect rect) {
  if (!platform->scissorDoesntRespectCamera) rect = renderer->baseMatrix2d * rect;
  backendSetScissor(rect);
}

void clearScissor() {
  backendEndScissor();
}

void pushAlpha(float value) {
	if (renderer->alphaStackNum > ALPHA_STACK_MAX-1) Panic("alpha overflow");
	renderer->alphaStack[renderer->alphaStackNum++] = value;
	backendSetAlpha(renderer->alphaStack[renderer->alphaStackNum-1]);
}

void popAlpha() {
	if (renderer->alphaStackNum <= 1) Panic("alpha underflow");
	renderer->alphaStackNum--;
	backendSetAlpha(renderer->alphaStack[renderer->alphaStackNum-1]);
}

void setRendererBlendMode(BlendMode blendMode) {
	processBatchDraws();
	renderer->lastBlendMode = blendMode;
	Raylib::rlSetBlendMode(Raylib::BLEND_ALPHA);
	Raylib::rlSetBlendMode(Raylib::BLEND_MULTIPLIED);

	if (blendMode == BLEND_NORMAL) {
		// void rlSetBlendFactors(int glSrcFactor, int glDstFactor, int glEquation);
		Raylib::rlSetBlendFactors(GL_ONE, GL_ONE_MINUS_SRC_ALPHA, GL_FUNC_ADD);
		Raylib::rlSetBlendMode(Raylib::BLEND_CUSTOM);
	} else if (blendMode == BLEND_MULTIPLY) {
		Raylib::rlSetBlendMode(Raylib::BLEND_MULTIPLIED);
	} else if (blendMode == BLEND_SCREEN) {
		logf("@todo no blendmodes in raylib\n");
	} else if (blendMode == BLEND_ADD) {
		Raylib::rlSetBlendMode(Raylib::BLEND_ADDITIVE);
	} else if (blendMode == BLEND_INVERT) {
		logf("@todo no blendmodes in raylib\n");
	} else if (blendMode == BLEND_SKIA) {
		Raylib::rlSetBlendFactors(GL_ONE, GL_ONE_MINUS_SRC_ALPHA, GL_FUNC_ADD);
		Raylib::rlSetBlendMode(Raylib::BLEND_CUSTOM);
	} else if (blendMode == BLEND_RECOMMENDED) {
		Raylib::rlSetBlendMode(Raylib::BLEND_ALPHA);
	}
}

void setDepthTest(bool enabled) {
	processBatchDraws();
	if (enabled) Raylib::rlEnableDepthTest();
	else Raylib::rlDisableDepthTest();
}

void setDepthMask(bool enabled) {
	processBatchDraws();
	if (enabled) Raylib::rlEnableDepthMask();
	else Raylib::rlDisableDepthMask();
}

void setBlending(bool enabled) {
	processBatchDraws();
	if (enabled) Raylib::rlEnableColorBlend();
	else Raylib::rlDisableColorBlend();
}

void setBackfaceCulling(bool enabled) {
	processBatchDraws();
	renderer->lastBackfaceCull = enabled;
	if (enabled) Raylib::rlEnableBackfaceCulling();
	else Raylib::rlDisableBackfaceCulling();
}

Light createLight(int number, int type, Raylib::Vector3 position, Raylib::Vector3 target, Raylib::Color color, Shader *shader) {
	Light light = {};

	light.enabled = true;
	light.type = type;
	light.position = position;
	light.target = target;
	light.color = color;

	light.enabledLoc = getUniformLocation(shader, frameSprintf("lights[%d].enabled", number));
	light.typeLoc = getUniformLocation(shader, frameSprintf("lights[%d].type", number));
	light.posLoc = getUniformLocation(shader, frameSprintf("lights[%d].position", number));
	light.targetLoc = getUniformLocation(shader, frameSprintf("lights[%d].target", number));
	light.colorLoc = getUniformLocation(shader, frameSprintf("lights[%d].color", number));

	updateLightValues(shader, light);

	return light;
}

void updateLightValues(Shader *shader, Light light) {
	setShaderUniform(shader, light.enabledLoc, &light.enabled, SHADER_UNIFORM_INT, 1);
	setShaderUniform(shader, light.typeLoc, &light.type, SHADER_UNIFORM_INT, 1);

	setShaderUniform(shader, light.posLoc, &light.position, SHADER_UNIFORM_VEC3, 1);

	setShaderUniform(shader, light.targetLoc, &light.target, SHADER_UNIFORM_VEC3, 1);

	float color[4] = { (float)light.color.r/(float)255, (float)light.color.g/(float)255, (float)light.color.b/(float)255, (float)light.color.a/(float)255 };
	setShaderUniform(shader, light.colorLoc, color, SHADER_UNIFORM_VEC4, 1);
}

void updateLightingShader(Camera camera) {
	// updateLightValues(renderer->lightingAnimatedShader, renderer->lights[0]);
	// updateLightValues(renderer->lightingAnimatedShader, renderer->lights[1]);
	// updateLightValues(renderer->lightingAnimatedShader, renderer->lights[2]);
	// updateLightValues(renderer->lightingAnimatedShader, renderer->lights[3]);

	// // This happens automatically because of drawMesh
	// // Raylib::SetShaderValue(renderer->lightingAnimatedShader, renderer->lightingAnimatedShader->locs[Raylib::SHADER_LOC_VECTOR_VIEW], &camera.position.x, Raylib::SHADER_UNIFORM_VEC3);
	// setShaderUniform(
	// 	renderer->lightingAnimatedShader,
	// 	renderer->lightingAnimatedShader->raylibShader.locs[Raylib::SHADER_LOC_VECTOR_VIEW],
	// 	&camera.position.x,
	// 	SHADER_UNIFORM_VEC3,
	// 	1
	// );
}

void processBatchDraws() {
	backendFlush();
}

void resetRenderContext() {
	processBatchDraws();
	setRendererBlendMode(BLEND_NORMAL);
#ifndef __EMSCRIPTEN__ // 100% needed...
	glBindSampler(0, 0);
#endif
}

void startRenderingFrame() {
	Raylib::BeginDrawing();
}

void endRenderingFrame() {
}

// void start3d(Camera camera) {
// 	renderer->in3dPass = true;

// 	processBatchDraws();

// 	Raylib::rlMatrixMode(RL_PROJECTION);
// 	Raylib::rlPushMatrix();
// 	Raylib::rlLoadIdentity();

// 	if (isZero(camera.size)) logf("camera.size is 0,0\n");
// 	if (camera.nearCull == 0) logf("camera.nearCull is 0\n");
// 	if (camera.farCull == 0) logf("camera.far is 0\n");
// 	if (!camera.isOrtho) {
// 		if (camera.fovy == 0) logf("fovy is 0\n");
// 		float aspect = camera.size.x/camera.size.y;
// 		Raylib::Matrix matProj = Raylib::MatrixPerspective(toRad(camera.fovy), aspect, camera.nearCull, camera.farCull);
// 		Raylib::rlMultMatrixf(MatrixToFloat(matProj));
// 	} else {
// 		if (camera.fovy != 0) logf("fovy does nothing currently\n");
// 		double top = camera.size.y/2;
// 		double right = camera.size.x/2;
// 		// Raylib::Matrix matOrtho = Raylib::MatrixOrtho(-right, right, -top, top, camera.nearCull, camera.farCull);
// 		Raylib::rlOrtho(-right, right, -top, top, camera.nearCull, camera.farCull);
// 	}

// 	Raylib::rlMatrixMode(RL_MODELVIEW);
// 	Raylib::rlLoadIdentity();

// 	Raylib::Matrix raylibLookAt = Raylib::MatrixLookAt(toRaylib(camera.position), toRaylib(camera.target), toRaylib(camera.up));
// 	Raylib::rlMultMatrixf(MatrixToFloat(raylibLookAt));

// 	setDepthTest(true);
// }

// void end3d() {
// 	renderer->in3dPass = false;
// 	processBatchDraws();
// 	Raylib::EndMode3D();
// }

void startShader(Shader *shader) {
	backendStartShader(&shader->backendShader);
}

void endShader() {
	backendEndShader();
}

// void getMouseRay(Camera camera, Vec2 mouse, Vec3 *outPos, Vec3 *outDir) {
// 	float x = (2.0f*mouse.x)/platform->windowWidth - 1.0f;
// 	float y = 1.0f - (2.0f*mouse.y)/platform->windowHeight;
// 	float z = 1.0f;

// 	Raylib::Vector3 deviceCoords = { x, y, z };

// 	Raylib::Matrix matProj;
// 	if (!camera.isOrtho) {
// 		float aspect = camera.size.x / camera.size.y;
// 		matProj = Raylib::MatrixPerspective(toRad(camera.fovy), aspect, camera.nearCull, camera.farCull);
// 	} else {
// 		matProj = Raylib::rlGetMatrixProjection();
// 	}
// 	Raylib::Matrix matView = Raylib::MatrixLookAt(toRaylib(camera.position), toRaylib(camera.target), toRaylib(camera.up));

// 	Raylib::Vector3 nearPoint = Raylib::Vector3Unproject({ deviceCoords.x, deviceCoords.y, 0.0f }, matProj, matView);
// 	Raylib::Vector3 farPoint = Raylib::Vector3Unproject({ deviceCoords.x, deviceCoords.y, 1.0f }, matProj, matView);

// 	Raylib::Vector3 cameraPlanePointerPos = Raylib::Vector3Unproject({ deviceCoords.x, deviceCoords.y, -1.0f }, matProj, matView);

// 	Raylib::Vector3 direction = Raylib::Vector3Normalize(Raylib::Vector3Subtract(farPoint, nearPoint));

// 	Raylib::Ray raylibScreenRay = {};
// 	Vec3 rayPos;
// 	if (!camera.isOrtho) rayPos = camera.position;
// 	else rayPos = v3(cameraPlanePointerPos);
// 	raylibScreenRay.direction = direction;

// 	*outPos = rayPos;
// 	*outDir = v3(raylibScreenRay.direction.x, raylibScreenRay.direction.y, raylibScreenRay.direction.z);
// }

// Vec2 worldSpaceTo2dNDC01(Camera camera, Vec3 worldPosition) {
// 	Raylib::Matrix proj;
// 	if (!camera.isOrtho) {
// 		float aspect = camera.size.x / camera.size.y;
// 		proj = Raylib::MatrixPerspective(toRad(camera.fovy), aspect, camera.nearCull, camera.farCull);
// 	} else {
// 		proj = Raylib::rlGetMatrixProjection();
// 	}
// 	Raylib::Matrix view = Raylib::MatrixLookAt(toRaylib(camera.position), toRaylib(camera.target), toRaylib(camera.up));
// #if 1
// 	Raylib::Quaternion worldPos = { worldPosition.x, worldPosition.y, worldPosition.z, 1.0f };

// 	worldPos = Raylib::QuaternionTransform(worldPos, view);
// 	worldPos = Raylib::QuaternionTransform(worldPos, proj);

// 	Raylib::Vector3 ndcPos = { worldPos.x/worldPos.w, -worldPos.y/worldPos.w, worldPos.z/worldPos.w };
// 	Vec2 ndc = v2(ndcPos.x, ndcPos.y);
// 	ndc.x++;
// 	ndc.y++;
// 	ndc /= 2;
// #else // This one used to work, with orthographic camera
// 	Raylib::Matrix viewProj = Raylib::MatrixMultiply(view, proj);

// 	Raylib::Matrix invViewProj = Raylib::MatrixInvert(viewProj);

// 	Raylib::Vector3 raylibPosition = {worldPosition.x, worldPosition.y, worldPosition.z};
// 	raylibPosition = Raylib::Vector3Transform(raylibPosition, viewProj);

// 	Vec2 ndc = v2(raylibPosition.x, raylibPosition.y);
// 	ndc /= 2;
// 	ndc.x++;
// #endif

// 	return ndc;
// }


///- Gui

void guiTexture(Texture *texture);
bool guiImageButton(Texture *texture);
// bool guiInputRgb(const char *name, int *argb, bool showInputs=false);
// bool guiInputArgb(const char *name, int *argb, bool showInputs=false);
// void guiPushStyleColor(ImGuiCol style, int color);
// void guiPopStyleColor(int amount=1);

static const char *raylibImGuiGetClipText(void *) { return Raylib::GetClipboardText(); }
static void raylibImGuiSetClipText(void *, const char *text) { Raylib::SetClipboardText(text); } 

struct Gui {
	Raylib::Texture2D FontTexture;

	ImGuiMouseCursor CurrentMouseCursor;

	bool needToDrawThisFrame;
};

Raylib::KeyboardKey raylibKeysThatImGuiCaresAbout[] = { Raylib::KEY_APOSTROPHE, Raylib::KEY_COMMA, Raylib::KEY_MINUS, Raylib::KEY_PERIOD, Raylib::KEY_SLASH, Raylib::KEY_ZERO, Raylib::KEY_ONE, Raylib::KEY_TWO, Raylib::KEY_THREE, Raylib::KEY_FOUR, Raylib::KEY_FIVE, Raylib::KEY_SIX, Raylib::KEY_SEVEN, Raylib::KEY_EIGHT, Raylib::KEY_NINE, Raylib::KEY_SEMICOLON, Raylib::KEY_EQUAL, Raylib::KEY_A, Raylib::KEY_B, Raylib::KEY_C, Raylib::KEY_D, Raylib::KEY_E, Raylib::KEY_F, Raylib::KEY_G, Raylib::KEY_H, Raylib::KEY_I, Raylib::KEY_J, Raylib::KEY_K, Raylib::KEY_L, Raylib::KEY_M, Raylib::KEY_N, Raylib::KEY_O, Raylib::KEY_P, Raylib::KEY_Q, Raylib::KEY_R, Raylib::KEY_S, Raylib::KEY_T, Raylib::KEY_U, Raylib::KEY_V, Raylib::KEY_W, Raylib::KEY_X, Raylib::KEY_Y, Raylib::KEY_Z, Raylib::KEY_SPACE, Raylib::KEY_ESCAPE, Raylib::KEY_ENTER, Raylib::KEY_TAB, Raylib::KEY_BACKSPACE, Raylib::KEY_INSERT, Raylib::KEY_DELETE, Raylib::KEY_RIGHT, Raylib::KEY_LEFT, Raylib::KEY_DOWN, Raylib::KEY_UP, Raylib::KEY_PAGE_UP, Raylib::KEY_PAGE_DOWN, Raylib::KEY_HOME, Raylib::KEY_END, Raylib::KEY_CAPS_LOCK, Raylib::KEY_SCROLL_LOCK, Raylib::KEY_NUM_LOCK, Raylib::KEY_PRINT_SCREEN, Raylib::KEY_PAUSE, Raylib::KEY_F1, Raylib::KEY_F2, Raylib::KEY_F3, Raylib::KEY_F4, Raylib::KEY_F5, Raylib::KEY_F6, Raylib::KEY_F7, Raylib::KEY_F8, Raylib::KEY_F9, Raylib::KEY_F10, Raylib::KEY_F11, Raylib::KEY_F12, Raylib::KEY_LEFT_SHIFT, Raylib::KEY_LEFT_CONTROL, Raylib::KEY_LEFT_ALT, Raylib::KEY_LEFT_SUPER, Raylib::KEY_RIGHT_SHIFT, Raylib::KEY_RIGHT_CONTROL, Raylib::KEY_RIGHT_ALT, Raylib::KEY_RIGHT_SUPER, Raylib::KEY_KB_MENU, Raylib::KEY_LEFT_BRACKET, Raylib::KEY_BACKSLASH, Raylib::KEY_RIGHT_BRACKET, Raylib::KEY_GRAVE, Raylib::KEY_KP_0, Raylib::KEY_KP_1, Raylib::KEY_KP_2, Raylib::KEY_KP_3, Raylib::KEY_KP_4, Raylib::KEY_KP_5, Raylib::KEY_KP_6, Raylib::KEY_KP_7, Raylib::KEY_KP_8, Raylib::KEY_KP_9, Raylib::KEY_KP_DECIMAL, Raylib::KEY_KP_DIVIDE, Raylib::KEY_KP_MULTIPLY, Raylib::KEY_KP_SUBTRACT, Raylib::KEY_KP_ADD, Raylib::KEY_KP_ENTER, Raylib::KEY_KP_EQUAL };


Gui *gui = NULL;

#include "guiUtils.cpp"

void guiInit() {
	gui = (Gui *)zalloc(sizeof(Gui));

	if (platform->isCommandLineOnly) platform->disableGui = true;
	if (platform->disableGui) return;

	gui->CurrentMouseCursor = ImGuiMouseCursor_COUNT;

	ImGui::CreateContext(NULL);

	ImGui::StyleColorsDark();

	ImGuiIO& io = ImGui::GetIO();
	io.BackendPlatformName = "imgui_impl_raylib";

	io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;

	io.KeyMap[ImGuiKey_Tab] = Raylib::KEY_TAB;
	io.KeyMap[ImGuiKey_LeftArrow] = Raylib::KEY_LEFT;
	io.KeyMap[ImGuiKey_RightArrow] = Raylib::KEY_RIGHT;
	io.KeyMap[ImGuiKey_UpArrow] = Raylib::KEY_UP;
	io.KeyMap[ImGuiKey_DownArrow] = Raylib::KEY_DOWN;
	io.KeyMap[ImGuiKey_PageUp] = Raylib::KEY_PAGE_DOWN;
	io.KeyMap[ImGuiKey_PageDown] = Raylib::KEY_PAGE_UP;
	io.KeyMap[ImGuiKey_Home] = Raylib::KEY_HOME;
	io.KeyMap[ImGuiKey_End] = Raylib::KEY_END;
	io.KeyMap[ImGuiKey_Insert] = Raylib::KEY_INSERT;
	io.KeyMap[ImGuiKey_Delete] = Raylib::KEY_DELETE;
	io.KeyMap[ImGuiKey_Backspace] = Raylib::KEY_BACKSPACE;
	io.KeyMap[ImGuiKey_Space] = Raylib::KEY_SPACE;
	io.KeyMap[ImGuiKey_Enter] = Raylib::KEY_ENTER;
	io.KeyMap[ImGuiKey_Escape] = Raylib::KEY_ESCAPE;
	io.KeyMap[ImGuiKey_KeyPadEnter] = Raylib::KEY_KP_ENTER;
	io.KeyMap[ImGuiKey_A] = Raylib::KEY_A;
	io.KeyMap[ImGuiKey_C] = Raylib::KEY_C;
	io.KeyMap[ImGuiKey_V] = Raylib::KEY_V;
	io.KeyMap[ImGuiKey_X] = Raylib::KEY_X;
	io.KeyMap[ImGuiKey_Y] = Raylib::KEY_Y;
	io.KeyMap[ImGuiKey_Z] = Raylib::KEY_Z;

	io.MousePos = ImVec2(0, 0);

	io.SetClipboardTextFn = raylibImGuiSetClipText;
	io.GetClipboardTextFn = raylibImGuiGetClipText;

	io.ClipboardUserData = NULL;

	ImGui::GetStyle().ScaleAllSizes(platform->windowScaling);
	ImGui::GetIO().FontGlobalScale *= platform->windowScaling;

	{ /// Reload fonts
		ImGuiIO& io = ImGui::GetIO();
		unsigned char* pixels = NULL;

		int width;
		int height;
		io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height, NULL);
		Raylib::Image image = Raylib::GenImageColor(width, height, Raylib::BLANK);
		memcpy(image.data, pixels, width * height * 4);

		if (gui->FontTexture.id != 0) Raylib::UnloadTexture(gui->FontTexture);

		gui->FontTexture = LoadTextureFromImage(image);
		Raylib::UnloadImage(image);
		io.Fonts->TexID = &gui->FontTexture;
	}
}

void guiStartFrame() {
	gui->needToDrawThisFrame = true;
	platform->hoveringGui = false;
	platform->typingGui = false;
	if (platform->disableGui) return;

	{ /// rlImGuiNewFrame();
		ImGuiIO& io = ImGui::GetIO();

		io.DisplaySize.x = platform->windowWidth;
		io.DisplaySize.y = platform->windowHeight;

		io.DeltaTime = Raylib::GetFrameTime();

		io.KeyCtrl = Raylib::IsKeyDown(Raylib::KEY_RIGHT_CONTROL) || Raylib::IsKeyDown(Raylib::KEY_LEFT_CONTROL);
		io.KeyShift = Raylib::IsKeyDown(Raylib::KEY_RIGHT_SHIFT) || Raylib::IsKeyDown(Raylib::KEY_LEFT_SHIFT);
		io.KeyAlt = Raylib::IsKeyDown(Raylib::KEY_RIGHT_ALT) || Raylib::IsKeyDown(Raylib::KEY_LEFT_ALT);
		io.KeySuper = Raylib::IsKeyDown(Raylib::KEY_RIGHT_SUPER) || Raylib::IsKeyDown(Raylib::KEY_LEFT_SUPER);

		if (io.WantSetMousePos)
		{
			Raylib::SetMousePosition((int)io.MousePos.x, (int)io.MousePos.y);
		}
		else
		{
			io.MousePos.x = Raylib::GetMouseX();
			io.MousePos.y = Raylib::GetMouseY();
		}

		io.MouseDown[0] = Raylib::IsMouseButtonDown(Raylib::MOUSE_LEFT_BUTTON);
		io.MouseDown[1] = Raylib::IsMouseButtonDown(Raylib::MOUSE_RIGHT_BUTTON);
		io.MouseDown[2] = Raylib::IsMouseButtonDown(Raylib::MOUSE_MIDDLE_BUTTON);


#ifdef __EMSCRIPTEN__
		if (Raylib::GetMouseWheelMove() > 0) io.MouseWheel -= 1;
		else if (Raylib::GetMouseWheelMove() < 0) io.MouseWheel += 1;
#else
		if (Raylib::GetMouseWheelMove() > 0) io.MouseWheel += 1;
		else if (Raylib::GetMouseWheelMove() < 0) io.MouseWheel -= 1;
#endif

		if ((io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange) == 0)
		{
			ImGuiMouseCursor imgui_cursor = ImGui::GetMouseCursor();
			if (imgui_cursor != gui->CurrentMouseCursor || io.MouseDrawCursor)
			{
				gui->CurrentMouseCursor = imgui_cursor;
				if (io.MouseDrawCursor || imgui_cursor == ImGuiMouseCursor_None)
				{
					Raylib::HideCursor();
				}
				else
				{
					Raylib::RaylibShowCursor();

					if (!(io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange))
					{
						if (imgui_cursor == ImGuiMouseCursor_Arrow) Raylib::SetMouseCursor(Raylib::MOUSE_CURSOR_ARROW);
						else if (imgui_cursor == ImGuiMouseCursor_TextInput) Raylib::SetMouseCursor(Raylib::MOUSE_CURSOR_IBEAM);
						else if (imgui_cursor == ImGuiMouseCursor_Hand) Raylib::SetMouseCursor(Raylib::MOUSE_CURSOR_POINTING_HAND);
						else if (imgui_cursor == ImGuiMouseCursor_ResizeAll) Raylib::SetMouseCursor(Raylib::MOUSE_CURSOR_RESIZE_ALL);
						else if (imgui_cursor == ImGuiMouseCursor_ResizeEW) Raylib::SetMouseCursor(Raylib::MOUSE_CURSOR_RESIZE_EW);
						else if (imgui_cursor == ImGuiMouseCursor_ResizeNESW) Raylib::SetMouseCursor(Raylib::MOUSE_CURSOR_RESIZE_NESW);
						else if (imgui_cursor == ImGuiMouseCursor_ResizeNS) Raylib::SetMouseCursor(Raylib::MOUSE_CURSOR_RESIZE_NS);
						else if (imgui_cursor == ImGuiMouseCursor_ResizeNWSE) Raylib::SetMouseCursor(Raylib::MOUSE_CURSOR_RESIZE_NWSE);
						else if (imgui_cursor == ImGuiMouseCursor_NotAllowed) Raylib::SetMouseCursor(Raylib::MOUSE_CURSOR_NOT_ALLOWED);
						else Raylib::SetMouseCursor(Raylib::MOUSE_CURSOR_DEFAULT);
					}
				}
			}
		}
	}

	{ /// rlImGuiEvents();
    ImGuiIO& io = ImGui::GetIO();
		for (int i = 0; i < ArrayLength(raylibKeysThatImGuiCaresAbout); i++) {
			Raylib::KeyboardKey raylibKey = raylibKeysThatImGuiCaresAbout[i];
			io.KeysDown[raylibKey] = Raylib::IsKeyDown(raylibKey);
		}

    unsigned int pressed = Raylib::GetCharPressed();
    if (pressed != 0) io.AddInputCharacter(pressed);

		platform->hoveringGui = io.WantCaptureMouse;
		platform->typingGui = io.WantTextInput;
	}

	ImGui::NewFrame();
}

void guiDraw() {
	if (!gui->needToDrawThisFrame) return;
	gui->needToDrawThisFrame = false;
	if (platform->disableGui) return;

	{ /// rlImGuiEnd();
		ImGui::Render();

		{ /// rlRenderData(ImGui::GetDrawData());
			ImDrawData *data = ImGui::GetDrawData();

			Raylib::rlDrawRenderBatchActive();
			Raylib::rlDisableBackfaceCulling();
			Raylib::rlSetBlendMode(Raylib::BLEND_ALPHA);

			for (int l = 0; l < data->CmdListsCount; ++l) {
				int idxOffset = 0;

				const ImDrawList* commandList = data->CmdLists[l];

				for (const auto& cmd : commandList->CmdBuffer) {
					{ /// EnableScissor(...)
						Raylib::rlEnableScissorTest();
						float x = cmd.ClipRect.x - data->DisplayPos.x;
						float y = cmd.ClipRect.y - data->DisplayPos.y;
						float width = cmd.ClipRect.z - (cmd.ClipRect.x - data->DisplayPos.x);
						float height = cmd.ClipRect.w - (cmd.ClipRect.y - data->DisplayPos.y);
						Raylib::rlScissor((int)x, Raylib::GetScreenHeight() - (int)(y + height), (int)width, (int)height);
					}
					if (cmd.UserCallback != nullptr) {
						cmd.UserCallback(commandList, &cmd);
						idxOffset += cmd.ElemCount;
						continue;
					}

					{ /// rlImGuiRenderTriangles(cmd.ElemCount, idxOffset, commandList->IdxBuffer, commandList->VtxBuffer, cmd.TextureId);
						unsigned int count = cmd.ElemCount;
						int indexStart = idxOffset;
						const ImVector<ImDrawIdx>& indexBuffer = commandList->IdxBuffer;
						const ImVector<ImDrawVert>& vertBuffer = commandList->VtxBuffer;
						Raylib::Texture* texture = (Raylib::Texture*)cmd.TextureId;

						unsigned int textureId = (texture == nullptr) ? 0 : texture->id;

						Raylib::rlBegin(RL_TRIANGLES);
						Raylib::rlSetTexture(textureId);

						for (unsigned int i = 0; i <= (count - 3); i += 3) {
							if(Raylib::rlCheckRenderBatchLimit(3)) {
								Raylib::rlBegin(RL_TRIANGLES);
								Raylib::rlSetTexture(textureId);
							}

							ImDrawIdx indexA = indexBuffer[indexStart + i];
							ImDrawIdx indexB = indexBuffer[indexStart + i + 1];
							ImDrawIdx indexC = indexBuffer[indexStart + i + 2];

							ImDrawVert verts[3] = {
								vertBuffer[indexA],
								vertBuffer[indexB],
								vertBuffer[indexC],
							};

							for (int i = 0; i < 3; i++) {
								ImDrawVert idx_vert = verts[i];
								Raylib::Color *c = (Raylib::Color*)&idx_vert.col;
								Raylib::rlColor4ub(c->r, c->g, c->b, c->a);
								Raylib::rlTexCoord2f(idx_vert.uv.x, idx_vert.uv.y);
								Raylib::rlVertex2f(idx_vert.pos.x, idx_vert.pos.y);
							}
						}
						Raylib::rlEnd();
					}
					idxOffset += cmd.ElemCount;

					Raylib::rlDrawRenderBatchActive();
				}
			}

			Raylib::rlSetTexture(0);
			Raylib::rlDisableScissorTest();
			setBackfaceCulling(renderer->lastBackfaceCull);
		}
	}

	setRendererBlendMode(renderer->lastBlendMode);
}

void guiTexture(Texture *texture) {
	ImGui::Image(
		(ImTextureID)(intptr_t)&texture->backendTexture.raylibTexture.id,
		ImVec2(texture->width, texture->height),
		ImVec2(0, 1),
		ImVec2(1, 0)
	);
}
bool guiImageButton(Texture *texture) {
	return ImGui::ImageButton(
		(ImTextureID)(intptr_t)&texture->backendTexture.raylibTexture.id,
		ImVec2(texture->width, texture->height),
		ImVec2(0, 1),
		ImVec2(1, 0)
	);
}

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
