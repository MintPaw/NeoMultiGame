#if _WIN32
#define RAYLIB_GLSL_VERSION            330
#else
#define RAYLIB_GLSL_VERSION            100
#endif

#if _WIN32
#include "gl.h"
#else
#include <GLES3/gl3.h>
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

	bool running;

	int windowWidth;
	int windowHeight;
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
	KEY_BACKSPACE=Raylib::KEY_BACKSPACE,
	KEY_CTRL=Raylib::KEY_LEFT_CONTROL,
	KEY_BACKTICK=Raylib::KEY_GRAVE,
	KEY_ESC=Raylib::KEY_ESCAPE,
	KEY_F1=Raylib::KEY_F1,
	KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6, KEY_F7, KEY_F8, KEY_F9, KEY_F10, KEY_F11, KEY_F12,
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

void initPlatform(int windowWidth, int windowHeight, char *windowTitle);

void platformUpdateLoop(void (*updateCallback)());
void platformUpdate();

void minimizeWindow();
void maximizeWindow();
bool keyPressed(int key);
bool keyJustPressed(int key);
bool keyJustReleased(int key);
bool joyButtonPressed(int controllerId, int button);
bool joyButtonJustReleased(int controllerId, int button);
bool joyButtonJustPressed(int controllerId, int button);
Vec2 joyLeftStick(int controllerId);
Vec2 joyRightStick(int controllerId);
void setClipboard(char *str);
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

#ifdef _WIN32
	platform->processHandle = GetCurrentProcess();
#endif

	initLoggingSystem();
	pushRndSeed(time(NULL));

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

	if (platform->isInternalVersion) logf("Starting raylib engine\n");

	Raylib::InitWindow(platform->windowWidth, platform->windowHeight, windowTitle);
	Raylib::SetWindowState(Raylib::FLAG_WINDOW_RESIZABLE);

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

	{ /// Events
		platform->mouseJustDown = Raylib::IsMouseButtonPressed(Raylib::MOUSE_BUTTON_LEFT);
		platform->mouseJustUp = Raylib::IsMouseButtonReleased(Raylib::MOUSE_BUTTON_LEFT);
		platform->mouseDown = Raylib::IsMouseButtonDown(Raylib::MOUSE_BUTTON_LEFT);
		platform->rightMouseJustDown = Raylib::IsMouseButtonPressed(Raylib::MOUSE_BUTTON_RIGHT);
		platform->rightMouseJustUp = Raylib::IsMouseButtonReleased(Raylib::MOUSE_BUTTON_RIGHT);
		platform->rightMouseDown = Raylib::IsMouseButtonDown(Raylib::MOUSE_BUTTON_RIGHT);
		platform->mouseWheel = Raylib::GetMouseWheelMove();

		Raylib::Vector2 mouse = Raylib::GetMousePosition();
		platform->mouse.x = mouse.x;
		platform->mouse.y = mouse.y;

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

	void updateAudio(); //@headerHack
	updateAudio();

	Raylib::BeginDrawing();
	guiStartFrame();

	platform->updateCallback();

	guiDraw();
	Raylib::rlDrawRenderBatchActive();

	if (keyPressed(KEY_CTRL) && keyPressed('Q')) platform->running = false;

	freeFrameMemory();

	void freeFrameTextures(); //@headerHack
	freeFrameTextures();

	{ // Calcuate frame times
		platform->frameTimes[platform->frameCount % platform->frameTimesMax] = getMsPassed(platform->frameNano);

		platform->frameTimeAvg = 0;
		for (int i = 0; i < platform->frameTimesMax; i++) platform->frameTimeAvg += platform->frameTimes[i];
		platform->frameTimeAvg /= platform->frameTimesMax;
	}

	{ // Calculate memory usage
#if _WIN32
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

void minimizeWindow() {
	Raylib::MinimizeWindow();
}

void maximizeWindow() {
	Raylib::MaximizeWindow();
}

void setClipboard(char *str) {
	Raylib::SetClipboardText(str);
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
	//@stub
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
enum BlendMode {
	BLEND_NORMAL,
	BLEND_MULTIPLY,
	BLEND_SCREEN,
	BLEND_ADD,
	BLEND_INVERT,
	BLEND_SKIA,
};

struct Texture {
	Raylib::Texture2D raylibTexture;
	int width;
	int height;

	char *path;
};

struct RenderTexture {
	Raylib::RenderTexture2D raylibRenderTexture;
	int width;
	int height;
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
};

#define _F_TD_FLIP_Y           (1 << 1)
#define _F_TD_SKIP_PREMULTIPLY (1 << 2)
// #define _F_TD_SRGB8            (1 << 3)
#define _F_TD_RGB16F           (1 << 4)
#define _F_TD_RGBA32           (1 << 5)
struct Renderer {
	bool in3dPass;
	bool disabled;
	int maxTextureUnits; // Does nothing

	Raylib::Shader lightingShader;
	int lightingShaderAlphaLoc;

	Raylib::Shader lightingAnimatedShader;
	int lightingAnimatedShaderBoneTransformsLoc;

	Raylib::Shader alphaDiscardShader;

	Raylib::Shader danmakuShader;
	int danmakuShaderHueShiftValueLoc;

	int width;
	int height;

#define TARGET_TEXTURE_LIMIT 16
	RenderTexture *targetTextureStack[TARGET_TEXTURE_LIMIT];
	int targetTextureStackNum;

#define CAMERA_2D_STACK_MAX 128
	Matrix3 camera2dStack[CAMERA_2D_STACK_MAX];
	int camera2dStackNum;

#define ALPHA_STACK_MAX 128
	float alphaStack[ALPHA_STACK_MAX];
	int alphaStackNum;

	Matrix3 baseMatrix2d;
	Matrix3 currentCameraMatrix; // Is the same as baseMatrix2d for raylib!

	Texture *whiteTexture;
	RenderTexture *circleTexture;
	Texture *linearGrad256;

	Raylib::Light lights[MAX_LIGHTS];
	Raylib::Light lightsAnimated[MAX_LIGHTS];
	Raylib::Model cubeModel;
	Raylib::Model coneModel;
	Raylib::Model sphereModel;

	void *tempTextureBuffer;
	int tempTextureBufferSize;
	void *tempTextureRowBuffer;
	int tempTextureRowBufferSize;
};

Renderer *renderer = NULL;

Raylib::Color toRaylibColor(int color) { return Raylib::GetColor(argbToRgba(color)); }
Raylib::Vector3 toRaylib(Vec3 vec) { return {vec.x, vec.y, vec.z}; }
Vec3 v3(Raylib::Vector3 vec) { return v3(vec.x, vec.y, vec.z); }
Raylib::Matrix toRaylib(Matrix4 matrix) {
	Raylib::Matrix raylibMatrix = {};
	memcpy(&raylibMatrix.m0, matrix.transpose().data, sizeof(float) * 16);
	return raylibMatrix;
}


void initRenderer(int width, int height);
void clearRenderer(int color=0);

Texture *createTexture(const char *path, int flags=0);
Texture *createTexture(int width, int height, void *data=NULL, int flags=0);
RenderTexture *createRenderTexture(const char *path, int flags=0);
RenderTexture *createRenderTexture(int width, int height, void *data=NULL, int flags=0);
Raylib::RenderTexture2D myLoadRenderTexture(int width, int height, int flags=0);
Texture *renderTextureToTexture(RenderTexture *renderTexture);
void setTextureSmooth(Texture *texture, bool smooth);
void setTextureSmooth(RenderTexture *renderTexture, bool smooth);
void setTextureClamped(Texture *texture, bool clamped);
void setTextureClamped(RenderTexture *renderTexture, bool clamped);
void setTextureData(RenderTexture *renderTexture, void *data, int width, int height, int flags=0);
void setTextureData(Texture *texture, void *data, int width, int height, int flags=0);
void setRaylibTextureData(Raylib::Texture raylibTexture, void *data, int width, int height, int flags);
u8 *getTextureData(RenderTexture *renderTexture, int flags=0);
u8 *getTextureData(Texture *texture, int flags=0);

void destroyTexture(Texture *texture);
void destroyTexture(RenderTexture *renderTexture);

void drawTexture(Texture *texture, RenderProps props);
void drawSimpleTexture(Texture *texture);
void drawSimpleTexture(RenderTexture *renderTexture);
void drawSimpleTexture(RenderTexture *renderTexture, Matrix3 matrix, Vec2 uv0=v2(0, 0), Vec2 uv1=v2(1, 1), float alpha=1);
void drawSimpleTexture(Texture *texture, Matrix3 matrix, Vec2 uv0=v2(0, 0), Vec2 uv1=v2(1, 1), float alpha=1);
void drawFxaaTexture(RenderTexture *renderTexture, Matrix3 matrix);
void drawFxaaTexture(Texture *texture, Matrix3 matrix);
void drawPixelArtFilterTexture(RenderTexture *renderTexture, Matrix3 matrix, Vec2 uv0=v2(0, 0), Vec2 uv1=v2(1, 1));
void drawPixelArtFilterTexture(Texture *texture, Matrix3 matrix, Vec2 uv0=v2(0, 0), Vec2 uv1=v2(1, 1));
void drawRect(Rect rect, int color, int flags=0);
void drawCircle(Vec2 position, float radius, int color);
void drawBillboard(Camera camera, RenderTexture *renderTexture, Vec3 position, Vec2 size=v2(), int tint=0xFFFFFFFF, Rect source=makeRect());
void drawBillboard(Camera camera, Texture *texture, Vec3 position, Vec2 size=v2(), int tint=0xFFFFFFFF, Rect source=makeRect());
void draw2dQuad(RenderTexture *renderTexture, Matrix3 matrix, Vec2 uv0, Vec2 uv1, Matrix3 uvMatrix, Vec4i tints, float alpha, int flags);
void draw2dQuad(Texture *texture, Matrix3 matrix, Vec2 uv0, Vec2 uv1, Matrix3 uvMatrix, Vec4i tints, float alpha, int flags);
void drawTriangle(Texture *texture, Vec3 *verts, Vec2 *uvs, int tint=0xFFFFFFFF, float alpha=1);
void drawRaylibTexture(Raylib::Texture texture, Matrix3 matrix, Vec2 uv0, Vec2 uv1, Matrix3 uvMatrix, Vec4i tints, float alpha, int flags);

void pushTargetTexture(RenderTexture *renderTexture);
void popTargetTexture();
void setTargetTexture(RenderTexture *renderTexture);

void pushCamera2d(Matrix3 mat);
void popCamera2d();
void refreshGlobalMatrices();

void setScissor(Rect rect);
void clearScissor();

void pushAlpha(float alpha);
void popAlpha();

void setRendererBlendMode(BlendMode blendMode);
void setDepthMask(bool enabled);

void updateLightingShader();

void resetRenderContext();

void start3d(Camera camera, Vec2 size, float nearCull, float farCull);
void end3d();
void startShader(Raylib::Shader shader);
void endShader();
void getMouseRay(Camera camera, Vec2 mouse, Vec3 *outPos, Vec3 *outDir);
void drawAABB(AABB aabb, int color);
void drawCone(Cone cone, int color);
void drawSphere(Sphere sphere, int color);

#include "rendererUtils.cpp"
bool usesAlphaDiscard = false;

void initRenderer(int width, int height) {
	renderer = (Renderer *)zalloc(sizeof(Renderer));
	renderer->width = width;
	renderer->height = height;

	pushCamera2d(mat3());
	pushAlpha(1);
	setRendererBlendMode(BLEND_NORMAL);

	{ /// Setup shaders
#ifdef __EMSCRIPTEN__
    char *glslFolder = "glsl100";
#else
    char *glslFolder = "glsl330";
#endif

		char *vs;
		char *fs;

		fs = (char *)readFile(frameSprintf("assets/common/shaders/raylib/%s/alphaDiscard.fs", glslFolder));
		renderer->alphaDiscardShader = Raylib::LoadShaderFromMemory(NULL, fs);
		free(fs);

		vs = (char *)readFile(frameSprintf("assets/common/shaders/raylib/%s/base_lighting.vs", glslFolder));
		fs = (char *)readFile(frameSprintf("assets/common/shaders/raylib/%s/lighting.fs", glslFolder));
		renderer->lightingShader = Raylib::LoadShaderFromMemory(vs, fs);
		free(vs);
		free(fs);

		renderer->lightingShader.locs[Raylib::SHADER_LOC_VECTOR_VIEW] = Raylib::GetShaderLocation(renderer->lightingShader, "viewPos");
		renderer->lightingShaderAlphaLoc = Raylib::GetShaderLocation(renderer->lightingShader, "alpha");

		int ambientLoc = Raylib::GetShaderLocation(renderer->lightingShader, "ambient");
		float ambientLightValue[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
		Raylib::SetShaderValue(renderer->lightingShader, ambientLoc, ambientLightValue, Raylib::SHADER_UNIFORM_VEC4);

		// renderer->lights[0] = Raylib::CreateLight(Raylib::LIGHT_DIRECTIONAL, { 200, 0, 0 }, {0, 0, 0}, Raylib::RED, renderer->lightingShader);
		// renderer->lights[1] = Raylib::CreateLight(Raylib::LIGHT_DIRECTIONAL, { 0, -200, 0 }, {0, 0, 0}, Raylib::GREEN, renderer->lightingShader);
		// renderer->lights[2] = Raylib::CreateLight(Raylib::LIGHT_DIRECTIONAL, { 0, 0, 200 }, {0, 0, 0}, Raylib::BLUE, renderer->lightingShader);
		// renderer->lights[0] = Raylib::CreateLight(Raylib::LIGHT_POINT, { 1000, 0, 0 }, {0, 0, 0}, Raylib::RED, renderer->lightingShader);
		// renderer->lights[1] = Raylib::CreateLight(Raylib::LIGHT_POINT, { 0, -1000, 0 }, {0, 0, 0}, Raylib::GREEN, renderer->lightingShader);
		// renderer->lights[2] = Raylib::CreateLight(Raylib::LIGHT_POINT, { 0, 0, 1000 }, {0, 0, 0}, Raylib::BLUE, renderer->lightingShader);
		renderer->lights[0] = Raylib::CreateLight(Raylib::LIGHT_DIRECTIONAL, { 1, -1, 1 }, {0, 0, 0}, Raylib::WHITE, renderer->lightingShader);

		{
			vs = (char *)readFile(frameSprintf("assets/common/shaders/raylib/%s/lightingAnimated.vs", glslFolder));
			fs = (char *)readFile(frameSprintf("assets/common/shaders/raylib/%s/lightingAnimated.fs", glslFolder));
			renderer->lightingAnimatedShader = Raylib::LoadShaderFromMemory(vs, fs);
			free(vs);
			free(fs);

			int ambientLoc = Raylib::GetShaderLocation(renderer->lightingAnimatedShader, "ambient");
			renderer->lightingAnimatedShaderBoneTransformsLoc = Raylib::GetShaderLocation(renderer->lightingAnimatedShader, "boneTransforms");

			Raylib::SetShaderValue(renderer->lightingAnimatedShader, ambientLoc, ambientLightValue, Raylib::SHADER_UNIFORM_VEC4);
			renderer->lightsAnimated[0] = Raylib::CreateLight(Raylib::LIGHT_DIRECTIONAL, { 1, -1, 1 }, {0, 0, 0}, Raylib::WHITE, renderer->lightingAnimatedShader);
		}

		fs = (char *)readFile(frameSprintf("assets/common/shaders/raylib/%s/danmakuShader.fs", glslFolder));
		renderer->danmakuShader = Raylib::LoadShaderFromMemory(NULL, fs);
		free(fs);

		renderer->danmakuShaderHueShiftValueLoc = Raylib::GetShaderLocation(renderer->danmakuShader, "hueShiftValue");
	} ///

	u64 whiteData = 0xFFFFFFFF;
	renderer->whiteTexture = createTexture(1, 1, &whiteData);

	renderer->circleTexture = createRenderTexture(1024, 1024, NULL);
	pushTargetTexture(renderer->circleTexture);
	Raylib::DrawCircle(renderer->circleTexture->width/2, renderer->circleTexture->height/2, renderer->circleTexture->width/2, toRaylibColor(0xFFFFFFFF));
	popTargetTexture();

	renderer->cubeModel = Raylib::LoadModelFromMesh(Raylib::GenMeshCube(1, 1, 1));
	renderer->cubeModel.materials[0].shader = renderer->lightingShader;

	renderer->coneModel = Raylib::LoadModelFromMesh(Raylib::GenMeshCone(1, 1, 16));
	renderer->coneModel.materials[0].shader = renderer->lightingShader;

	renderer->sphereModel = Raylib::LoadModelFromMesh(Raylib::GenMeshSphere(1, 8, 8));
	renderer->sphereModel.materials[0].shader = renderer->lightingShader;

	initRendererUtils();
}

void clearRenderer(int color) {
	Raylib::ClearBackground(toRaylibColor(color));
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

	Raylib::Image raylibImage = {};
	raylibImage.width = width;
	raylibImage.height = height;
	if (flags & _F_TD_RGBA32) {
		raylibImage.format = Raylib::PIXELFORMAT_UNCOMPRESSED_R32G32B32A32;
	} else {
		raylibImage.format = Raylib::PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
	}
	raylibImage.mipmaps = 1;
	raylibImage.data = (u8 *)zalloc(width * height * 4);

	texture->raylibTexture = Raylib::LoadTextureFromImage(raylibImage);

	free(raylibImage.data);

	if (data) setTextureData(texture, data, width, height, flags);
	setTextureSmooth(texture, true);

	return texture;
}

RenderTexture *createRenderTexture(const char *path, int flags) {
	int pngSize;
	void *pngData = readFile(path, &pngSize);

	int width, height, channels;
	stbi_set_flip_vertically_on_load(true);
	stbi_uc *img = stbi_load_from_memory((unsigned char *)pngData, pngSize, &width, &height, &channels, 4);

	if (!img) return NULL;

	RenderTexture *texture = createRenderTexture(width, height, img, flags);
	if (!texture) Panic(frameSprintf("Failed to load image %s\n", path));

	free(img);
	free(pngData);

	return texture;
}

Raylib::RenderTexture2D myLoadRenderTexture(int width, int height, int flags) {
	Raylib::RenderTexture2D target = { 0 };

	target.id = Raylib::rlLoadFramebuffer(width, height);   // Load an empty framebuffer

	if (target.id > 0)
	{
		Raylib::rlEnableFramebuffer(target.id);

		// Create color texture (default to RGBA)
		int format = Raylib::PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
		if (flags & _F_TD_RGBA32) {
			format = Raylib::PIXELFORMAT_UNCOMPRESSED_R32G32B32A32;
		} else {
			format = Raylib::PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
		}
		target.texture.id = Raylib::rlLoadTexture(NULL, width, height, format, 1);
		target.texture.width = width;
		target.texture.height = height;
		target.texture.format = format;
		target.texture.mipmaps = 1;

		// Create depth renderbuffer/texture
		target.depth.id = Raylib::rlLoadTextureDepth(width, height, true);
		target.depth.width = width;
		target.depth.height = height;
		target.depth.format = 19;       //DEPTH_COMPONENT_24BIT?
		target.depth.mipmaps = 1;

		// Attach color texture and depth renderbuffer/texture to FBO
		Raylib::rlFramebufferAttach(
			target.id,
			target.texture.id,
			Raylib::RL_ATTACHMENT_COLOR_CHANNEL0,
			Raylib::RL_ATTACHMENT_TEXTURE2D,
			0
		);
		Raylib::rlFramebufferAttach(
			target.id,
			target.depth.id,
			Raylib::RL_ATTACHMENT_DEPTH,
			Raylib::RL_ATTACHMENT_RENDERBUFFER,
			0
		);

		// Check if fbo is complete with attachments (valid)
		if (Raylib::rlFramebufferComplete(target.id)) TRACELOG(LOG_INFO, "FBO: [ID %i] Framebuffer object created successfully", target.id);

		Raylib::rlDisableFramebuffer();
	} else {
		TRACELOG(LOG_WARNING, "FBO: Framebuffer object can not be created");
	}

	return target;
}

RenderTexture *createRenderTexture(int width, int height, void *data, int flags) {
	RenderTexture *renderTexture = (RenderTexture *)zalloc(sizeof(RenderTexture));
	renderTexture->width = width;
	renderTexture->height = height;
	renderTexture->raylibRenderTexture = myLoadRenderTexture(width, height, flags);
	if (data) setTextureData(renderTexture, data, width, height, flags);
	return renderTexture;
}

Texture *renderTextureToTexture(RenderTexture *renderTexture) {
	u8 *data = getTextureData(renderTexture);
	Texture *texture = createTexture(renderTexture->width, renderTexture->height, data);
	free(data);
	destroyTexture(renderTexture);
	return texture;
}

void setTextureSmooth(Texture *texture, bool smooth) {
	Raylib::SetTextureFilter(texture->raylibTexture, smooth ? Raylib::TEXTURE_FILTER_BILINEAR : Raylib::TEXTURE_FILTER_POINT);
}
void setTextureSmooth(RenderTexture *renderTexture, bool smooth) {
	Raylib::SetTextureFilter(renderTexture->raylibRenderTexture.texture, smooth ? Raylib::TEXTURE_FILTER_BILINEAR : Raylib::TEXTURE_FILTER_POINT);
}

void setTextureClamped(Texture *texture, bool clamped) {
	Raylib::SetTextureWrap(texture->raylibTexture, clamped ? Raylib::TEXTURE_WRAP_CLAMP : Raylib::TEXTURE_WRAP_REPEAT);
}
void setTextureClamped(RenderTexture *renderTexture, bool clamped) {
	Raylib::SetTextureWrap(renderTexture->raylibRenderTexture.texture, clamped ? Raylib::TEXTURE_WRAP_CLAMP : Raylib::TEXTURE_WRAP_REPEAT);
}

void setTextureData(Texture *texture, void *data, int width, int height, int flags) {
	setRaylibTextureData(texture->raylibTexture, data, width, height, flags);
}

void setTextureData(RenderTexture *renderTexture, void *data, int width, int height, int flags) {
	setRaylibTextureData(renderTexture->raylibRenderTexture.texture, data, width, height, flags);
}

void setRaylibTextureData(Raylib::Texture raylibTexture, void *data, int width, int height, int flags) {
	int neededTextureBufferSize = width * height * 4;
	if (neededTextureBufferSize > renderer->tempTextureBufferSize) {
		if (renderer->tempTextureBuffer) free(renderer->tempTextureBuffer);

		renderer->tempTextureBufferSize = neededTextureBufferSize;
		renderer->tempTextureBuffer = malloc(renderer->tempTextureBufferSize);
	}

	void *newData = renderer->tempTextureBuffer;
	memcpy(newData, data, width * height * 4);
	data = newData;

	if ((flags & _F_TD_SKIP_PREMULTIPLY) == 0) {
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
#if 1
				float a = ((u8 *)data)[(y*width+x)*4 + 3] / 255.0;
				((u8 *)data)[(y*width+x)*4 + 2] *= a;
				((u8 *)data)[(y*width+x)*4 + 1] *= a;
				((u8 *)data)[(y*width+x)*4 + 0] *= a;
#else
				u8 a, r, g, b;
				int index = (y*width+x) * 4;
				a = ((u8 *)data)[index + 3];
				r = ((u8 *)data)[index + 2];
				g = ((u8 *)data)[index + 1];
				b = ((u8 *)data)[index + 0];

				r *= (float)a/255.0;
				g *= (float)a/255.0;
				b *= (float)a/255.0;

				((u8 *)data)[index + 3] = a;
				((u8 *)data)[index + 2] = r;
				((u8 *)data)[index + 1] = g;
				((u8 *)data)[index + 0] = b;
#endif
			}
		}
	}

	if (flags & _F_TD_FLIP_Y) {
		int neededTextureRowBufferSize = width * 4;
		if (neededTextureRowBufferSize > renderer->tempTextureRowBufferSize) {
			if (renderer->tempTextureRowBuffer) free(renderer->tempTextureRowBuffer);

			renderer->tempTextureRowBufferSize = neededTextureRowBufferSize;
			renderer->tempTextureRowBuffer = malloc(renderer->tempTextureRowBufferSize);
		}

		u8 *tempRow = (u8 *)renderer->tempTextureRowBuffer;
		for (int y = 0; y < height/2; y++) {
			int curTopRow = y;
			int curBottomRow = height - y - 1;
			u8 *topRowStart = (u8 *)data + curTopRow * width * 4;
			u8 *bottomRowStart = (u8 *)data + curBottomRow * width * 4;

			memcpy(tempRow, topRowStart, width * 4);
			memcpy(topRowStart, bottomRowStart, width * 4);
			memcpy(bottomRowStart, tempRow, width * 4);
		}
	}

	Raylib::UpdateTexture(raylibTexture, data);
}

u8 *getTextureData(RenderTexture *renderTexture, int flags) {
	Texture texture;
	texture.width = renderTexture->width;
	texture.height = renderTexture->height;
	texture.raylibTexture = renderTexture->raylibRenderTexture.texture;
	return getTextureData(&texture, flags);
}

u8 *getTextureData(Texture *texture, int flags) {
	Raylib::rlDrawRenderBatchActive();
	Raylib::Image raylibImage = Raylib::LoadImageFromTexture(texture->raylibTexture);
	u8 *colors = (u8 *)Raylib::LoadImageColors(raylibImage);

	u8 *data = (u8 *)zalloc(texture->width * texture->height * 4);
	memcpy(data, colors, texture->width * texture->height * 4);

	RL_FREE(colors);

	Raylib::UnloadImage(raylibImage);
	return data;
}

void destroyTexture(Texture *texture) {
	Raylib::UnloadTexture(texture->raylibTexture);
	free(texture);
}

void destroyTexture(RenderTexture *renderTexture) {
	Raylib::UnloadRenderTexture(renderTexture->raylibRenderTexture);
	free(renderTexture);
}

void drawTexture(RenderTexture *renderTexture, RenderProps props);
void drawTexture(RenderTexture *renderTexture, RenderProps props) {
	if (props.alpha == 0) return;
	if (props.disabled) return;
	if (!renderTexture) Panic("drawTexture called with null renderTexture!");

	if (props.srcWidth == 0) props.srcWidth = renderTexture->width;
	if (props.srcHeight == 0) props.srcHeight = renderTexture->height;
	props.matrix.SCALE(props.srcWidth, props.srcHeight);

	Vec4i tints = v4i(props.tint, props.tint, props.tint, props.tint);
	float alpha = props.alpha;
	int flags = props.flags;
	Vec2 uv0 = props.uv0;
	Vec2 uv1 = props.uv1;
	drawRaylibTexture(renderTexture->raylibRenderTexture.texture, props.matrix, uv0, uv1, props.uvMatrix, tints, alpha, flags);
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
	Vec2 uv0 = props.uv0;
	Vec2 uv1 = props.uv1;
	drawRaylibTexture(texture->raylibTexture, props.matrix, uv0, uv1, props.uvMatrix, tints, alpha, flags);
}

void drawSimpleTexture(RenderTexture *renderTexture) {
	Matrix3 matrix = mat3();
	matrix.SCALE(getSize(renderTexture));
	Matrix3 uvMatrix = mat3();
	Vec2 uv0 = v2(0, 0);
	Vec2 uv1 = v2(1, 1);
	Vec4i tints = v4i(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF);
	int alpha = 1;
	int flags = 0;
	drawRaylibTexture(renderTexture->raylibRenderTexture.texture, matrix, uv0, uv1, uvMatrix, tints, alpha, flags);
}
void drawSimpleTexture(Texture *texture) {
	Matrix3 matrix = mat3();
	matrix.SCALE(getSize(texture));
	Matrix3 uvMatrix = mat3();
	Vec2 uv0 = v2(0, 0);
	Vec2 uv1 = v2(1, 1);
	Vec4i tints = v4i(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF);
	int alpha = 1;
	int flags = 0;
	drawRaylibTexture(texture->raylibTexture, matrix, uv0, uv1, uvMatrix, tints, alpha, flags);
}
void drawSimpleTexture(RenderTexture *renderTexture, Matrix3 matrix, Vec2 uv0, Vec2 uv1, float alpha) {
	Matrix3 uvMatrix = mat3();
	Vec4i tints = v4i(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF);
	int flags = 0;
	drawRaylibTexture(renderTexture->raylibRenderTexture.texture, matrix, uv0, uv1, uvMatrix, tints, alpha, flags);
}
void drawSimpleTexture(Texture *texture, Matrix3 matrix, Vec2 uv0, Vec2 uv1, float alpha) {
	Matrix3 uvMatrix = mat3();
	Vec4i tints = v4i(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF);
	int flags = 0;
	drawRaylibTexture(texture->raylibTexture, matrix, uv0, uv1, uvMatrix, tints, alpha, flags);
}

void drawFxaaTexture(RenderTexture *renderTexture, Matrix3 matrix) {
	Vec2 uv0 = v2(0, 0);
	Vec2 uv1 = v2(1, 1);
	Matrix3 uvMatrix = mat3();
	Vec4i tints = v4i(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF);
	float alpha = 1;
	int flags = 0;
	drawRaylibTexture(renderTexture->raylibRenderTexture.texture, matrix, uv0, uv1, uvMatrix, tints, alpha, flags);
}
void drawFxaaTexture(Texture *texture, Matrix3 matrix) {
	Vec2 uv0 = v2(0, 0);
	Vec2 uv1 = v2(1, 1);
	Matrix3 uvMatrix = mat3();
	Vec4i tints = v4i(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF);
	float alpha = 1;
	int flags = 0;
	drawRaylibTexture(texture->raylibTexture, matrix, uv0, uv1, uvMatrix, tints, alpha, flags);
}

void drawPixelArtFilterTexture(RenderTexture *renderTexture, Matrix3 matrix, Vec2 uv0, Vec2 uv1) {
	drawSimpleTexture(renderTexture, matrix, uv0, uv1);
}
void drawPixelArtFilterTexture(Texture *texture, Matrix3 matrix, Vec2 uv0, Vec2 uv1) {
	drawSimpleTexture(texture, matrix, uv0, uv1);
}

void drawRect(Rect rect, int color, int flags) {
	unsigned char alphaByte = color >> 24; // This should be handled in drawRaylibTexture
	if (alphaByte == 0) return;

	Matrix3 matrix = mat3();
	matrix.TRANSLATE(rect.x, rect.y);
	matrix.SCALE(rect.width, rect.height);

	Matrix3 uvMatrix = mat3();
	float alpha = 1;
	Vec4i tints = v4i(color, color, color, color);
	drawRaylibTexture(renderer->whiteTexture->raylibTexture, matrix, v2(0, 0), v2(1, 1), uvMatrix, tints, alpha, flags);
}

void drawCircle(Vec2 position, float radius, int color) {
	unsigned char alphaByte = color >> 24;
	if (alphaByte == 0) return;

	Matrix3 matrix = mat3();
	matrix.TRANSLATE(position - radius);
	matrix.SCALE(radius*2);

	Matrix3 uvMatrix = mat3();
	float alpha = alphaByte/255.0;
	int flags = 0;
	Vec4i tints = v4i(color, color, color, color);
	drawRaylibTexture(renderer->circleTexture->raylibRenderTexture.texture, matrix, v2(0, 0), v2(1, 1), uvMatrix, tints, alpha, flags);
}

void drawBillboard(Camera camera, RenderTexture *renderTexture, Vec3 position, Vec2 size, int tint, Rect source) {
	Texture texture;
	texture.width = renderTexture->width;
	texture.height = renderTexture->height;
	texture.raylibTexture = renderTexture->raylibRenderTexture.texture;
	drawBillboard(camera, &texture, position, size, tint, source);
}

void drawBillboard(Camera camera, Texture *texture, Vec3 position, Vec2 size, int tint, Rect source) {
	if (renderer->disabled) return;
	if (isZero(source)) source = makeRect(0, 0, texture->width, texture->height);
	if (isZero(size)) size = v2(texture->width, texture->height);

	// I flipped uv.y
	Vec2 uv0; // bl For some reason
	uv0.x = source.x/texture->width;
	uv0.y = (source.y + source.height)/texture->height;

	Vec2 uv1; // tr For some reason
	uv1.x = (source.x + source.width)/texture->width;
	uv1.y = source.y/texture->height;

	if (size.x < 0) {
		float temp = uv0.x;
		uv0.x = uv1.x;
		uv1.x = temp;
		size.x *= -1;
	}

	if (size.y < 0) logf("No y flip\n");

	Raylib::Matrix matView = Raylib::MatrixLookAt(toRaylib(camera.position), toRaylib(camera.target), toRaylib(camera.up));

	Vec3 right = v3(matView.m0, matView.m4, matView.m8);
	Vec3 left = v3(-matView.m0, -matView.m4, -matView.m8);
	Vec3 up = v3(matView.m1, matView.m5, matView.m9);
	// Vec3 up = v3(0, 0, 1);

	Vec3 rightScaled = right * (size.x/2);
	Vec3 upScaled = up * (size.y/2);

	Vec3 p1 = rightScaled + upScaled;
	Vec3 p2 = rightScaled - upScaled;

	Vec3 topLeft = -p2;
	Vec3 topRight = p1;
	Vec3 bottomRight = p2;
	Vec3 bottomLeft = -p1;

	// Rotation would happen here!

	topLeft += position;
	topRight += position;
	bottomRight += position;
	bottomLeft += position;

	Raylib::rlCheckRenderBatchLimit(4);

	Raylib::rlSetTexture(texture->raylibTexture.id);

	Raylib::rlBegin(RL_QUADS);
	Raylib::Color raylibTint = toRaylibColor(tint);
	Raylib::rlColor4ub(raylibTint.r, raylibTint.g, raylibTint.b, raylibTint.a);

	Raylib::rlTexCoord2f(uv0.x, uv0.y);
	Raylib::rlVertex3f(topLeft.x, topLeft.y, topLeft.z);

	Raylib::rlTexCoord2f(uv0.x, uv1.y);
	Raylib::rlVertex3f(bottomLeft.x, bottomLeft.y, bottomLeft.z);

	Raylib::rlTexCoord2f(uv1.x, uv1.y);
	Raylib::rlVertex3f(bottomRight.x, bottomRight.y, bottomRight.z);

	Raylib::rlTexCoord2f(uv1.x, uv0.y);
	Raylib::rlVertex3f(topRight.x, topRight.y, topRight.z);

	Raylib::rlEnd();

	Raylib::rlSetTexture(0);
}

void draw2dQuad(RenderTexture *renderTexture, Matrix3 matrix, Vec2 uv0, Vec2 uv1, Matrix3 uvMatrix, Vec4i tints, float alpha, int flags) {
	drawRaylibTexture(renderTexture->raylibRenderTexture.texture, matrix, uv0, uv1, uvMatrix, tints, alpha, flags);
}
void draw2dQuad(Texture *texture, Matrix3 matrix, Vec2 uv0, Vec2 uv1, Matrix3 uvMatrix, Vec4i tints, float alpha, int flags) {
	drawRaylibTexture(texture->raylibTexture, matrix, uv0, uv1, uvMatrix, tints, alpha, flags);
}

void drawTriangle(Texture *texture, Vec3 *verts, Vec2 *uvs, int tint, float alpha) {
	alpha *= renderer->alphaStack[renderer->alphaStackNum-1];
	if (renderer->disabled) return;
	if (alpha == 0) return;

	if (!texture) texture = renderer->whiteTexture;

	// Matrix3 flipMatrix = {
	// 	1,  0,  0,
	// 	0, -1,  0,
	// 	0,  1,  1
	// };
	// uvMatrix = flipMatrix * uvMatrix;
	// for (int i = 0; i < ArrayLength(uvs); i++) {
	// 	uvs[i] = uvMatrix * uvs[i];
	// }

	int raylibPointCount = 3;

	{
		Raylib::Texture raylibTexture = texture->raylibTexture;
		Raylib::rlCheckRenderBatchLimit((raylibPointCount - 1)*4);

		Raylib::rlSetTexture(raylibTexture.id);

		Raylib::rlBegin(RL_TRIANGLES);

		int a, r, g, b;
		hexToArgb(tint, &a, &r, &g, &b);
		a *= alpha;
		r *= a/255.0;
		g *= a/255.0;
		b *= a/255.0;
		tint = argbToHex(a, r, g, b);
		Raylib::Color raylibTint = toRaylibColor(tint);
		Raylib::rlColor4ub(raylibTint.r, raylibTint.g, raylibTint.b, raylibTint.a);

		for (int i = 0; i < raylibPointCount; i++) {
			Raylib::rlTexCoord2f(uvs[i].x, uvs[i].y);
			Raylib::rlVertex3f(verts[i].x, verts[i].y, verts[i].z);
		}

		Raylib::rlEnd();

		Raylib::rlSetTexture(0);
	}
}

void drawRaylibTexture(Raylib::Texture raylibTexture, Matrix3 matrix, Vec2 uv0, Vec2 uv1, Matrix3 uvMatrix, Vec4i tints, float alpha,int flags) {
	alpha *= renderer->alphaStack[renderer->alphaStackNum-1];
	if (renderer->disabled) return;
	if (alpha == 0) return;

	Vec2 verts[] = {
		v2(0, 0),
		v2(0, 1),
		v2(1, 1),
		v2(1, 0),
	};

	matrix = renderer->baseMatrix2d * matrix;

	for (int i = 0; i < ArrayLength(verts); i++) {
		verts[i] = matrix * verts[i];
	}

	Vec2 uvs[] = {
		v2(uv0.x, uv0.y),
		v2(uv0.x, uv1.y),
		v2(uv1.x, uv1.y),
		v2(uv1.x, uv0.y),
	};

	Matrix3 flipMatrix = {
		1,  0,  0,
		0, -1,  0,
		0,  1,  1
	};
	uvMatrix = flipMatrix * uvMatrix;
	for (int i = 0; i < ArrayLength(uvs); i++) {
		uvs[i] = uvMatrix * uvs[i];
	}

	int raylibPointCount = 4;

	{
		Raylib::rlCheckRenderBatchLimit((raylibPointCount - 1)*4);

		Raylib::rlSetTexture(raylibTexture.id);

		Raylib::rlBegin(RL_QUADS);

		for (int i = 0; i < raylibPointCount; i++) {
			int tint = ((int *)&tints.x)[i];
			int a, r, g, b;
			hexToArgb(tint, &a, &r, &g, &b);
			a *= alpha;
			r *= a/255.0;
			g *= a/255.0;
			b *= a/255.0;
			tint = argbToHex(a, r, g, b);
			Raylib::Color raylibTint = toRaylibColor(tint);
			Raylib::rlColor4ub(raylibTint.r, raylibTint.g, raylibTint.b, raylibTint.a);

			Raylib::rlTexCoord2f(uvs[i].x, uvs[i].y);
			Raylib::rlVertex2f(verts[i].x, verts[i].y);
		}

		Raylib::rlEnd();

		Raylib::rlSetTexture(0);
	}
}

void pushTargetTexture(RenderTexture *renderTexture) {
	if (renderer->targetTextureStackNum >= TARGET_TEXTURE_LIMIT-1) Panic("Target texture overflow");

	renderer->targetTextureStack[renderer->targetTextureStackNum++] = renderTexture;

	setTargetTexture(renderTexture);
}

void popTargetTexture() {
	renderer->targetTextureStackNum--;

	if (renderer->targetTextureStackNum > 0) {
		setTargetTexture(renderer->targetTextureStack[renderer->targetTextureStackNum-1]);
	} else {
		setTargetTexture(NULL);
	}
}

void setTargetTexture(RenderTexture *renderTexture) {
	if (renderTexture == NULL) {
		Raylib::EndTextureMode();
	} else {
		Raylib::BeginTextureMode(renderTexture->raylibRenderTexture);
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

	renderer->currentCameraMatrix = mat3();
	for (int i = 0; i < renderer->camera2dStackNum; i++) renderer->currentCameraMatrix *= renderer->camera2dStack[i];

	renderer->baseMatrix2d *= renderer->currentCameraMatrix;
}

void setScissor(Rect rect) { //@todo Untested
	Raylib::BeginScissorMode(rect.x, rect.y, rect.width, rect.height);
}

void clearScissor() {
	Raylib::EndScissorMode();
}

void pushAlpha(float value) {
	if (renderer->alphaStackNum > ALPHA_STACK_MAX-1) Panic("alpha overflow");
	renderer->alphaStack[renderer->alphaStackNum++] = value;
}

void popAlpha() {
	if (renderer->alphaStackNum <= 1) Panic("alpha underflow");
	renderer->alphaStackNum--;
}

void setRendererBlendMode(BlendMode blendMode) {
	Raylib::rlSetBlendMode(Raylib::BLEND_ALPHA);
	Raylib::rlSetBlendMode(Raylib::BLEND_MULTIPLIED);

	if (blendMode == BLEND_NORMAL) {
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
	}
}

void setDepthMask(bool enabled) {
	if (enabled) Raylib::rlEnableDepthMask();
	else Raylib::rlDisableDepthMask();
}

void updateLightingShader(Camera camera) {
	Raylib::UpdateLightValues(renderer->lightingShader, renderer->lights[0]);
	Raylib::UpdateLightValues(renderer->lightingShader, renderer->lights[1]);
	Raylib::UpdateLightValues(renderer->lightingShader, renderer->lights[2]);
	Raylib::UpdateLightValues(renderer->lightingShader, renderer->lights[3]);
	Raylib::SetShaderValue(renderer->lightingShader, renderer->lightingShader.locs[Raylib::SHADER_LOC_VECTOR_VIEW], &camera.position.x, Raylib::SHADER_UNIFORM_VEC3);

	Raylib::UpdateLightValues(renderer->lightingAnimatedShader, renderer->lightsAnimated[0]);
	Raylib::UpdateLightValues(renderer->lightingAnimatedShader, renderer->lightsAnimated[1]);
	Raylib::UpdateLightValues(renderer->lightingAnimatedShader, renderer->lightsAnimated[2]);
	Raylib::UpdateLightValues(renderer->lightingAnimatedShader, renderer->lightsAnimated[3]);

	// This happens automatically because of drawMesh
	// Raylib::SetShaderValue(renderer->lightingAnimatedShader, renderer->lightingAnimatedShader.locs[Raylib::SHADER_LOC_VECTOR_VIEW], &camera.position.x, Raylib::SHADER_UNIFORM_VEC3);
}

void resetRenderContext() {
	Raylib::rlDrawRenderBatchActive();
	setRendererBlendMode(BLEND_NORMAL);
#ifndef __EMSCRIPTEN__
	glBindSampler(0, 0);
#endif
}

void start3d(Camera camera, Vec2 size, float nearCull, float farCull) {
	renderer->in3dPass = true;

	Raylib::rlDrawRenderBatchActive(); // Why do I need this?

	Raylib::rlMatrixMode(RL_PROJECTION);
	Raylib::rlPushMatrix();
	Raylib::rlLoadIdentity();

	if (!camera.isOrtho) logf("No perspective camera allowed\n");
	double top = size.y/2;
	double right = size.x/2;

	Raylib::rlOrtho(-right, right, -top, top, nearCull, farCull);

	Raylib::rlMatrixMode(RL_MODELVIEW);
	Raylib::rlLoadIdentity();

	Raylib::Matrix raylibLookAt = Raylib::MatrixLookAt(toRaylib(camera.position), toRaylib(camera.target), toRaylib(camera.up));
	Raylib::rlMultMatrixf(MatrixToFloat(raylibLookAt));

	Raylib::rlEnableDepthTest(); 
}

void end3d() {
	renderer->in3dPass = false;
	Raylib::EndMode3D();
}

void startShader(Raylib::Shader shader) {
	Raylib::BeginShaderMode(shader);
}
void endShader() {
	Raylib::EndShaderMode();
}

void getMouseRay(Camera camera, Vec2 mouse, Vec3 *outPos, Vec3 *outDir) {
	float x = (2.0f*mouse.x)/platform->windowWidth - 1.0f;
	float y = 1.0f - (2.0f*mouse.y)/platform->windowHeight;
	float z = 1.0f;

	Raylib::Vector3 deviceCoords = { x, y, z };

	Raylib::Matrix matProj = Raylib::rlGetMatrixProjection();
	Raylib::Matrix matView = Raylib::MatrixLookAt(toRaylib(camera.position), toRaylib(camera.target), toRaylib(camera.up));

	Raylib::Vector3 nearPoint = Raylib::Vector3Unproject({ deviceCoords.x, deviceCoords.y, 0.0f }, matProj, matView);
	Raylib::Vector3 farPoint = Raylib::Vector3Unproject({ deviceCoords.x, deviceCoords.y, 1.0f }, matProj, matView);

	Raylib::Vector3 cameraPlanePointerPos = Raylib::Vector3Unproject({ deviceCoords.x, deviceCoords.y, -1.0f }, matProj, matView);

	Raylib::Vector3 direction = Raylib::Vector3Normalize(Raylib::Vector3Subtract(farPoint, nearPoint));

	Raylib::Ray raylibScreenRay = {};
	raylibScreenRay.position = cameraPlanePointerPos;
	raylibScreenRay.direction = direction;

	*outPos = v3(raylibScreenRay.position.x, raylibScreenRay.position.y, raylibScreenRay.position.z);
	*outDir = v3(raylibScreenRay.direction.x, raylibScreenRay.direction.y, raylibScreenRay.direction.z);
}

void drawAABB(AABB aabb, int color) {
	if (!renderer->in3dPass) {
		logf("Doing 3d draw call outside pass\n");
		return;
	}

	Vec3 size = getSize(aabb);
	Vec3 pos = aabb.min + size/2;
	Raylib::DrawModelEx(renderer->cubeModel, toRaylib(pos), toRaylib(v3(0, 0, 1)), 0, toRaylib(size), toRaylibColor(color));
}

void drawCone(Cone cone, int color) {
	if (!renderer->in3dPass) {
		logf("Doing 3d draw call outside pass\n");
		return;
	}

	Vec3 origDir = v3(0, -1, 0);
	Vec3 newDir = cone.direction;
	float rotationAngle = acos(dot(origDir, newDir)/(length(origDir)*length(newDir)));
	Vec3 rotationAxis = cross(origDir, newDir);

	Vec3 position = cone.position;
	position += newDir * cone.length;

	Vec3 size = v3(cone.radius, cone.length, cone.radius);

	Raylib::rlMatrixMode(RL_MODELVIEW);
	Raylib::DrawModelEx(renderer->coneModel, toRaylib(position), toRaylib(rotationAxis), toDeg(rotationAngle), toRaylib(size), toRaylibColor(color));
}

void drawSphere(Vec3 position, float radius, int color) { drawSphere(makeSphere(position, radius), color); };
void drawSphere(Sphere sphere, int color) {
	if (!renderer->in3dPass) {
		logf("Doing 3d draw call outside pass\n");
		return;
	}

	Vec3 position = sphere.position;
	Vec3 size = v3(1, 1, 1) * sphere.radius;
	Raylib::DrawModelEx(renderer->sphereModel, toRaylib(position), toRaylib(v3(0, 0, 1)), 0, toRaylib(size), toRaylibColor(color));
}

///- Gui

void guiTexture(Texture *texture);
bool guiImageButton(Texture *texture);
bool guiImageButton(RenderTexture *texture);
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

	ImGui::GetStyle().ScaleAllSizes(platform->windowScaling);
	ImGui::GetIO().FontGlobalScale = platform->windowScaling;
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

		if (Raylib::GetMouseWheelMove() > 0)
			io.MouseWheel += 1;
		else if (Raylib::GetMouseWheelMove() < 0)
			io.MouseWheel -= 1;

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
			Raylib::rlEnableBackfaceCulling();
		}
	}

	setRendererBlendMode(BLEND_NORMAL);
}

void guiTexture(Texture *texture) {
	ImGui::Image((ImTextureID)(intptr_t)&texture->raylibTexture, ImVec2(texture->width, texture->height), ImVec2(0, 1), ImVec2(1, 0));
}
void guiTexture(RenderTexture *renderTexture) {
	ImGui::Image((ImTextureID)(intptr_t)&renderTexture->raylibRenderTexture.texture, ImVec2(renderTexture->width, renderTexture->height), ImVec2(0, 1), ImVec2(1, 0));
}

bool guiImageButton(Texture *texture) {
	return ImGui::ImageButton((ImTextureID)(intptr_t)&texture->raylibTexture, ImVec2(texture->width, texture->height), ImVec2(0, 1), ImVec2(1, 0));
}
bool guiImageButton(RenderTexture *renderTexture) {
	return ImGui::ImageButton((ImTextureID)(intptr_t)&renderTexture->raylibRenderTexture.texture, ImVec2(renderTexture->width, renderTexture->height), ImVec2(0, 1), ImVec2(1, 0));
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
