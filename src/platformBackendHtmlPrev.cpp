#include <GLES2/gl2.h>
#include <GL/gl.h>

///- Platform

struct NanoTime {
	timespec timeSpec;
};

enum PlatformKey {
	KEY_LEFT=600,
	KEY_RIGHT,
	KEY_UP,
	KEY_DOWN,
	KEY_SHIFT,
	KEY_RIGHT_SHIFT,
	KEY_BACKSPACE,
	KEY_CTRL,
	KEY_RIGHT_CTRL,
	KEY_ALT,
	KEY_RIGHT_ALT,
	KEY_BACKTICK,
	KEY_ESC,
	KEY_F1,
	KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6, KEY_F7, KEY_F8, KEY_F9, KEY_F10, KEY_F11, KEY_F12,
	KEY_ENTER,
	MOUSE_LEFT=500, //@incomplete mouse events
	MOUSE_RIGHT=501,
	KEYS_MAX,
};

enum KeyState {
	KEY_STATE_RELEASED,
	KEY_STATE_PRESSED,
	KEY_STATE_JUST_PRESSED,
	KEY_STATE_JUST_RELEASED,
};

enum PlatformEventType {
	PLATFORM_EVENT_KEY_DOWN,
	PLATFORM_EVENT_KEY_UP,
};
struct PlatformEvent { // Needed for HTML
	PlatformEventType type;
	int keyCode;
};

struct Platform {
	bool isDebugVersion;
	bool isInternalVersion;
	bool disableGui;

	bool running;

	int windowWidth;
	int windowHeight;
	float windowScaling;

	float elapsed;
	Vec2 mouse;
	int mouseWheel;

	bool hoveringGui; // Maybe move this to gui and use a headerHack function to check it
	bool typingGui; // Maybe move this to gui and use a headerHack function to check it

	float time;
	int frameCount;

	NanoTime frameNano;

#define PLATFORM_FRAME_TIMES_MAX 60
  float frameTimes[PLATFORM_FRAME_TIMES_MAX];
	float frameTimeAvg;
	float frameTimeHighest;

	int memoryUsage;

#define KEYS_MAX 1024
	KeyState keys[KEYS_MAX];
#define PLATFORM_EVENTS_MAX 128
	PlatformEvent events[PLATFORM_EVENTS_MAX];
	int eventsNum;

	void (*updateCallback)();
};

Platform *platform = NULL;

void initPlatform(int windowWidth, int windowHeight, char *windowTitle);

void platformUpdateLoop(void (*updateCallback)());
void platformUpdate();

void maximizeWindow() {}
void minimizeWindow() {}
void restoreWindow() {}
bool keyPressed(int key, bool ignoreImGui=false);
bool keyJustPressed(int key, bool ignoreImGui=false);
bool keyJustReleased(int key, bool ignoreImGui=false);
void setClipboard(char *str);
void resizeWindow(int width, int height);
void platformSleep(int ms);
NanoTime getNanoTime();
float getMsPassed(NanoTime startTime);
void navigateToUrl(const char *url);
void showErrorWindow(char *msg);

/// FUNCTIONS ^

void initPlatform(int windowWidth, int windowHeight, char *windowTitle) {
	platform = (Platform *)zalloc(sizeof(Platform));
	platform->windowWidth = windowWidth;
	platform->windowHeight = windowHeight;

#if defined(FALLOW_DEBUG)
	platform->isDebugVersion = true;
#endif

#if defined(FALLOW_INTERNAL)
	platform->isInternalVersion = true;
#endif

	// Create window

	// platform->windowScaling = Raylib::GetWindowScaleDPI().x;

	auto keyCallback = [](int eventType, const EmscriptenKeyboardEvent *keyEvent, void *userData)->EM_BOOL {
		if (platform->eventsNum > PLATFORM_EVENTS_MAX-1) {
			logf("Too many platform events!\n");
			return EM_TRUE;
		}

		if (eventType == EMSCRIPTEN_EVENT_KEYDOWN) {
			PlatformEvent *event = &platform->events[platform->eventsNum++];
			memset(event, 0, sizeof(PlatformEvent));
			event->type = PLATFORM_EVENT_KEY_DOWN;
			event->keyCode = toUpperCase(keyEvent->key[0]);
		} else if (eventType == EMSCRIPTEN_EVENT_KEYUP) {
			PlatformEvent *event = &platform->events[platform->eventsNum++];
			memset(event, 0, sizeof(PlatformEvent));
			event->type = PLATFORM_EVENT_KEY_UP;
			event->keyCode = toUpperCase(keyEvent->key[0]);
		}
		return EM_TRUE;
	};
	emscripten_set_keydown_callback("#canvas", nullptr, true, keyCallback);
	emscripten_set_keyup_callback("#canvas", nullptr, true, keyCallback);

	pushRndSeed(time(NULL));

  void imguiInit(); //@headerHack
	imguiInit();
}

void platformUpdateLoop(void (*updateCallback)()) {
	platform->updateCallback = updateCallback;
	platform->running = true;

	emscripten_set_main_loop(platformUpdate, 0, 1);
}

void platformUpdate() {
  platform->elapsed = 1/60.0;

	platform->frameNano = getNanoTime();

	// platform->windowWidth = Raylib::GetScreenWidth();
	// platform->windowHeight = Raylib::GetScreenHeight();

	{ /// Events
		for (int i = 0; i < KEYS_MAX; i++) {
			if (platform->keys[i] == KEY_STATE_JUST_PRESSED) platform->keys[i] = KEY_STATE_PRESSED;
			if (platform->keys[i] == KEY_STATE_JUST_RELEASED) platform->keys[i] = KEY_STATE_RELEASED;
		}

		for (int i = 0; i < platform->eventsNum; i++) {
			PlatformEvent *event = &platform->events[i];
			if (event->type == PLATFORM_EVENT_KEY_DOWN) {
				platform->keys[event->keyCode] = KEY_STATE_JUST_PRESSED;
			} else if (event->type == PLATFORM_EVENT_KEY_UP) {
				platform->keys[event->keyCode] = KEY_STATE_JUST_RELEASED;
			}
		}
		platform->eventsNum = 0;

		// platform->mouseWheel = Raylib::GetMouseWheelMove();
		// platform->mouseWheel *= -1;

		// platform->mouse.x = Raylib::GetMouseX();
		// platform->mouse.y = Raylib::GetMouseY();

		// if (platform->hoveringGui) {
		// 	platform->mouseWheel = 0;
		// }
	} ///

	void updateAudio(); //@headerHack
	updateAudio();

	void startRenderingFrame(); //@headerHack
	startRenderingFrame();

  void imguiStartFrame(); //@headerHack
  imguiStartFrame();

	void nguiStartFrame(); //@headerHack
	nguiStartFrame();

	platform->updateCallback();

  void imguiDraw(); //@headerHack
  imguiDraw();

	void endRenderingFrame(); //@headerHack
	endRenderingFrame();

	if (keyPressed(KEY_CTRL) && keyPressed('Q')) platform->running = false;

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

	platform->frameCount++;
	platform->time += platform->elapsed;
	if (logSys) logSys->time = platform->time;

	freeFrameMemory();
}

void setClipboard(char *str) {
	Panic("Not implemented");
}

void resizeWindow(int width, int height) {
	Panic("Not implemented");
}

bool keyPressed(int key, bool ignoreImGui) {
  if (!ignoreImGui) {
    if (platform->typingGui) return false;
    if (key == MOUSE_LEFT || key == MOUSE_RIGHT) {
      if (platform->hoveringGui) return false;
    }
  }

	return platform->keys[key] == KEY_STATE_PRESSED;
}

bool keyJustPressed(int key, bool ignoreImGui) {
  if (!ignoreImGui) {
    if (platform->typingGui) return false;
    if (key == MOUSE_LEFT || key == MOUSE_RIGHT) {
      if (platform->hoveringGui) return false;
    }
  }

	return platform->keys[key] == KEY_STATE_JUST_PRESSED;
}

bool keyJustReleased(int key, bool ignoreImGui) {
  if (!ignoreImGui) {
    if (platform->typingGui) return false;
    if (key == MOUSE_LEFT || key == MOUSE_RIGHT) {
      if (platform->hoveringGui) return false;
    }
  }

	return platform->keys[key] == KEY_STATE_JUST_RELEASED;
}

void platformSleep(int ms) {
	Panic("Not implemented");

	timespec ts;
	ts.tv_sec = ms / 1000;
	ts.tv_nsec = (ms % 1000) * 1000000;
	nanosleep(&ts, NULL);
}

NanoTime getNanoTime() {
	NanoTime time = {};
	clock_gettime(CLOCK_REALTIME, &time.timeSpec);
	return time;
}

float getMsPassed(NanoTime startTime) {
	NanoTime endTime = getNanoTime();

	float seconds = ((float)endTime.timeSpec.tv_sec - (float)startTime.timeSpec.tv_sec) + ((float)endTime.timeSpec.tv_nsec - (float)startTime.timeSpec.tv_nsec) / 1000000000.0;
	float ms = seconds * 1000.0;
	if (ms < 0) ms = 0;
	return ms;
}

void navigateToUrl(const char *url) {
	Panic("Not implemented");
	// Raylib::OpenURL(url);
}

void showErrorWindow(char *msg) {
	logf("Error window: %s\n", msg);
	EM_ASM({
		var str = UTF8ToString($0);
		str = str.split("\n").join("<br />");
		crashlogElement.innerHTML += str;
	}, msg);
}

void logLastOSErrorCode(const char *fileName, int lineNum) {
	int myErrno = errno;
	if (myErrno != 0) logf("Linux errorno (%s:%d): %s (%d)\n", fileName, lineNum, strerror(myErrno), myErrno);
}

///- Renderer
#include "rendererBackendWebgl.cpp"
#include "rendererFrontend.cpp"

///- ImGui

void guiTexture(Texture *texture, Vec2 scale=v2(1, 1));
bool guiImageButton(Texture *texture);

static const char *raylibImGuiGetClipText(void *) { Panic("Not implemented"); return NULL; }
static void raylibImGuiSetClipText(void *, const char *text) { Panic("Not implemented"); } 

struct Gui {
	// Raylib::Texture2D FontTexture;

	// ImGuiMouseCursor CurrentMouseCursor;

	bool needToDrawThisFrame;
};


Gui *gui = NULL;

#include "guiUtils.cpp"

void imguiInit() {
	gui = (Gui *)zalloc(sizeof(Gui));
	platform->disableGui = true;

// #if defined(FALLOW_COMMAND_LINE_ONLY)
// 	platform->disableGui = true;
// #endif
// 	if (platform->disableGui) return;

// 	gui->CurrentMouseCursor = ImGuiMouseCursor_COUNT;

// 	ImGui::CreateContext(NULL);

// 	ImGui::StyleColorsDark();

// 	ImGuiIO& io = ImGui::GetIO();
// 	io.BackendPlatformName = "imgui_impl_raylib";

// 	io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;

// 	io.KeyMap[ImGuiKey_Tab] = Raylib::KEY_TAB;
// 	io.KeyMap[ImGuiKey_LeftArrow] = Raylib::KEY_LEFT;
// 	io.KeyMap[ImGuiKey_RightArrow] = Raylib::KEY_RIGHT;
// 	io.KeyMap[ImGuiKey_UpArrow] = Raylib::KEY_UP;
// 	io.KeyMap[ImGuiKey_DownArrow] = Raylib::KEY_DOWN;
// 	io.KeyMap[ImGuiKey_PageUp] = Raylib::KEY_PAGE_DOWN;
// 	io.KeyMap[ImGuiKey_PageDown] = Raylib::KEY_PAGE_UP;
// 	io.KeyMap[ImGuiKey_Home] = Raylib::KEY_HOME;
// 	io.KeyMap[ImGuiKey_End] = Raylib::KEY_END;
// 	io.KeyMap[ImGuiKey_Insert] = Raylib::KEY_INSERT;
// 	io.KeyMap[ImGuiKey_Delete] = Raylib::KEY_DELETE;
// 	io.KeyMap[ImGuiKey_Backspace] = Raylib::KEY_BACKSPACE;
// 	io.KeyMap[ImGuiKey_Space] = Raylib::KEY_SPACE;
// 	io.KeyMap[ImGuiKey_Enter] = Raylib::KEY_ENTER;
// 	io.KeyMap[ImGuiKey_Escape] = Raylib::KEY_ESCAPE;
// 	io.KeyMap[ImGuiKey_KeyPadEnter] = Raylib::KEY_KP_ENTER;
// 	io.KeyMap[ImGuiKey_A] = Raylib::KEY_A;
// 	io.KeyMap[ImGuiKey_C] = Raylib::KEY_C;
// 	io.KeyMap[ImGuiKey_V] = Raylib::KEY_V;
// 	io.KeyMap[ImGuiKey_X] = Raylib::KEY_X;
// 	io.KeyMap[ImGuiKey_Y] = Raylib::KEY_Y;
// 	io.KeyMap[ImGuiKey_Z] = Raylib::KEY_Z;

// 	io.MousePos = ImVec2(0, 0);

// 	io.SetClipboardTextFn = raylibImGuiSetClipText;
// 	io.GetClipboardTextFn = raylibImGuiGetClipText;

// 	io.ClipboardUserData = NULL;

// 	ImGui::GetStyle().ScaleAllSizes(platform->windowScaling);
// 	ImGui::GetIO().FontGlobalScale *= platform->windowScaling;

// 	ImFont *imFont;
// 	{
// 		int ttfDataSize;
// 		void *ttfData = readFile("assets/common/arial.ttf", &ttfDataSize);
// 		ImGuiIO &io = ImGui::GetIO();
// 		imFont = io.Fonts->AddFontFromMemoryTTF(ttfData, ttfDataSize, 13);
// 		io.Fonts->Build();
// 	}

// 	{ /// Reload fonts
// 		ImGuiIO &io = ImGui::GetIO();
// 		u8 *pixels = NULL;

// 		int width, height;
// 		io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height, NULL);
// 		Raylib::Image image = Raylib::GenImageColor(width, height, Raylib::BLANK);
// 		memcpy(image.data, pixels, width * height * 4);

// 		if (gui->FontTexture.id != 0) Raylib::UnloadTexture(gui->FontTexture);

// 		gui->FontTexture = LoadTextureFromImage(image);
// 		Raylib::UnloadImage(image);
// 		io.Fonts->TexID = &gui->FontTexture;
// 	}
}

void imguiStartFrame() {
	gui->needToDrawThisFrame = true;
	platform->hoveringGui = false;
	platform->typingGui = false;
	if (platform->disableGui) return;

	// { /// rlImGuiNewFrame();
	// 	ImGuiIO& io = ImGui::GetIO();

	// 	io.DisplaySize.x = platform->windowWidth;
	// 	io.DisplaySize.y = platform->windowHeight;

	// 	io.DeltaTime = Raylib::GetFrameTime();

	// 	io.KeyCtrl = Raylib::IsKeyDown(Raylib::KEY_RIGHT_CONTROL) || Raylib::IsKeyDown(Raylib::KEY_LEFT_CONTROL);
	// 	io.KeyShift = Raylib::IsKeyDown(Raylib::KEY_RIGHT_SHIFT) || Raylib::IsKeyDown(Raylib::KEY_LEFT_SHIFT);
	// 	io.KeyAlt = Raylib::IsKeyDown(Raylib::KEY_RIGHT_ALT) || Raylib::IsKeyDown(Raylib::KEY_LEFT_ALT);
	// 	io.KeySuper = Raylib::IsKeyDown(Raylib::KEY_RIGHT_SUPER) || Raylib::IsKeyDown(Raylib::KEY_LEFT_SUPER);

	// 	if (io.WantSetMousePos)
	// 	{
	// 		Raylib::SetMousePosition((int)io.MousePos.x, (int)io.MousePos.y);
	// 	}
	// 	else
	// 	{
	// 		io.MousePos.x = Raylib::GetMouseX();
	// 		io.MousePos.y = Raylib::GetMouseY();
	// 	}

	// 	io.MouseDown[0] = Raylib::IsMouseButtonDown(Raylib::MOUSE_LEFT_BUTTON);
	// 	io.MouseDown[1] = Raylib::IsMouseButtonDown(Raylib::MOUSE_RIGHT_BUTTON);
	// 	io.MouseDown[2] = Raylib::IsMouseButtonDown(Raylib::MOUSE_MIDDLE_BUTTON);

// #ifdef __EMSCRIPTEN__
	// 	if (Raylib::GetMouseWheelMove() > 0) io.MouseWheel -= 1;
	// 	else if (Raylib::GetMouseWheelMove() < 0) io.MouseWheel += 1;
// #else
	// 	if (Raylib::GetMouseWheelMove() > 0) io.MouseWheel += 1;
	// 	else if (Raylib::GetMouseWheelMove() < 0) io.MouseWheel -= 1;
// #endif

	// 	if ((io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange) == 0)
	// 	{
	// 		ImGuiMouseCursor imgui_cursor = ImGui::GetMouseCursor();
	// 		if (imgui_cursor != gui->CurrentMouseCursor || io.MouseDrawCursor)
	// 		{
	// 			gui->CurrentMouseCursor = imgui_cursor;
	// 			if (io.MouseDrawCursor || imgui_cursor == ImGuiMouseCursor_None)
	// 			{
	// 				Raylib::HideCursor();
	// 			}
	// 			else
	// 			{
	// 				Raylib::RaylibShowCursor();

	// 				if (!(io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange))
	// 				{
	// 					if (imgui_cursor == ImGuiMouseCursor_Arrow) Raylib::SetMouseCursor(Raylib::MOUSE_CURSOR_ARROW);
	// 					else if (imgui_cursor == ImGuiMouseCursor_TextInput) Raylib::SetMouseCursor(Raylib::MOUSE_CURSOR_IBEAM);
	// 					else if (imgui_cursor == ImGuiMouseCursor_Hand) Raylib::SetMouseCursor(Raylib::MOUSE_CURSOR_POINTING_HAND);
	// 					else if (imgui_cursor == ImGuiMouseCursor_ResizeAll) Raylib::SetMouseCursor(Raylib::MOUSE_CURSOR_RESIZE_ALL);
	// 					else if (imgui_cursor == ImGuiMouseCursor_ResizeEW) Raylib::SetMouseCursor(Raylib::MOUSE_CURSOR_RESIZE_EW);
	// 					else if (imgui_cursor == ImGuiMouseCursor_ResizeNESW) Raylib::SetMouseCursor(Raylib::MOUSE_CURSOR_RESIZE_NESW);
	// 					else if (imgui_cursor == ImGuiMouseCursor_ResizeNS) Raylib::SetMouseCursor(Raylib::MOUSE_CURSOR_RESIZE_NS);
	// 					else if (imgui_cursor == ImGuiMouseCursor_ResizeNWSE) Raylib::SetMouseCursor(Raylib::MOUSE_CURSOR_RESIZE_NWSE);
	// 					else if (imgui_cursor == ImGuiMouseCursor_NotAllowed) Raylib::SetMouseCursor(Raylib::MOUSE_CURSOR_NOT_ALLOWED);
	// 					else Raylib::SetMouseCursor(Raylib::MOUSE_CURSOR_DEFAULT);
	// 				}
	// 			}
	// 		}
	// 	}
	// }

	// { /// rlImGuiEvents();
    // ImGuiIO& io = ImGui::GetIO();
	// 	for (int i = 0; i < ArrayLength(raylibKeysThatImGuiCaresAbout); i++) {
	// 		Raylib::KeyboardKey raylibKey = raylibKeysThatImGuiCaresAbout[i];
	// 		io.KeysDown[raylibKey] = Raylib::IsKeyDown(raylibKey);
	// 	}

    // unsigned int pressed = Raylib::GetCharPressed();
    // if (pressed != 0) io.AddInputCharacter(pressed);

	// 	platform->hoveringGui = io.WantCaptureMouse;
	// 	platform->typingGui = io.WantTextInput;
	// }

	// ImGui::NewFrame();
}

void imguiDraw() {
	if (!gui->needToDrawThisFrame) return;
	gui->needToDrawThisFrame = false;
	if (platform->disableGui) return;

  // { /// rlImGuiEnd();
  //   processBatchDraws();
  //   ImGui::Render();

  //   /// rlRenderData(ImGui::GetDrawData());
  //   ImDrawData *data = ImGui::GetDrawData();
  //   if (data->CmdListsCount == 0) return;

  //   GLenum last_blend_src_rgb; glGetIntegerv(GL_BLEND_SRC_RGB, (GLint*)&last_blend_src_rgb);
  //   GLenum last_blend_dst_rgb; glGetIntegerv(GL_BLEND_DST_RGB, (GLint*)&last_blend_dst_rgb);
  //   GLenum last_blend_src_alpha; glGetIntegerv(GL_BLEND_SRC_ALPHA, (GLint*)&last_blend_src_alpha);
  //   GLenum last_blend_dst_alpha; glGetIntegerv(GL_BLEND_DST_ALPHA, (GLint*)&last_blend_dst_alpha);
  //   GLenum last_blend_equation_rgb; glGetIntegerv(GL_BLEND_EQUATION_RGB, (GLint*)&last_blend_equation_rgb);
  //   GLenum last_blend_equation_alpha; glGetIntegerv(GL_BLEND_EQUATION_ALPHA, (GLint*)&last_blend_equation_alpha);
  //   GLboolean last_enable_blend = glIsEnabled(GL_BLEND);
  //   GLboolean last_enable_cull_face = glIsEnabled(GL_CULL_FACE);
  //   GLboolean last_enable_depth_test = glIsEnabled(GL_DEPTH_TEST);
  //   GLboolean last_enable_stencil_test = glIsEnabled(GL_STENCIL_TEST);
  //   GLboolean last_enable_scissor_test = glIsEnabled(GL_SCISSOR_TEST);

  //   glEnable(GL_BLEND);
  //   glBlendEquation(GL_FUNC_ADD);
  //   glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
  //   glDisable(GL_CULL_FACE);
  //   glDisable(GL_DEPTH_TEST);
  //   glDisable(GL_STENCIL_TEST);
  //   glEnable(GL_SCISSOR_TEST);

  //   for (int l = 0; l < data->CmdListsCount; ++l) {
  //     int idxOffset = 0;

  //     const ImDrawList* commandList = data->CmdLists[l];

  //     for (const auto& cmd : commandList->CmdBuffer) {
  //       { /// EnableScissor(...)
  //         Raylib::rlEnableScissorTest();
  //         float x = cmd.ClipRect.x - data->DisplayPos.x;
  //         float y = cmd.ClipRect.y - data->DisplayPos.y;
  //         float width = cmd.ClipRect.z - (cmd.ClipRect.x - data->DisplayPos.x);
  //         float height = cmd.ClipRect.w - (cmd.ClipRect.y - data->DisplayPos.y);
  //         Raylib::rlScissor((int)x, Raylib::GetScreenHeight() - (int)(y + height), (int)width, (int)height);
  //       }
  //       if (cmd.UserCallback != nullptr) {
  //         cmd.UserCallback(commandList, &cmd);
  //         idxOffset += cmd.ElemCount;
  //         continue;
  //       }

  //       { /// rlImGuiRenderTriangles(cmd.ElemCount, idxOffset, commandList->IdxBuffer, commandList->VtxBuffer, cmd.TextureId);
  //         unsigned int count = cmd.ElemCount;
  //         int indexStart = idxOffset;
  //         const ImVector<ImDrawIdx>& indexBuffer = commandList->IdxBuffer;
  //         const ImVector<ImDrawVert>& vertBuffer = commandList->VtxBuffer;
  //         Raylib::Texture* texture = (Raylib::Texture*)cmd.TextureId;

  //         unsigned int textureId = (texture == nullptr) ? 0 : texture->id;

  //         Raylib::rlBegin(RL_TRIANGLES);
  //         Raylib::rlSetTexture(textureId);

  //         for (unsigned int i = 0; i <= (count - 3); i += 3) {
  //           if(Raylib::rlCheckRenderBatchLimit(3)) {
  //             Raylib::rlBegin(RL_TRIANGLES);
  //             Raylib::rlSetTexture(textureId);
  //           }

  //           ImDrawIdx indexA = indexBuffer[indexStart + i];
  //           ImDrawIdx indexB = indexBuffer[indexStart + i + 1];
  //           ImDrawIdx indexC = indexBuffer[indexStart + i + 2];

  //           ImDrawVert verts[3] = {
  //             vertBuffer[indexA],
  //             vertBuffer[indexB],
  //             vertBuffer[indexC],
  //           };

  //           for (int i = 0; i < 3; i++) {
  //             ImDrawVert idx_vert = verts[i];
  //             Raylib::Color *c = (Raylib::Color*)&idx_vert.col;
  //             Raylib::rlColor4ub(c->r, c->g, c->b, c->a);
  //             Raylib::rlTexCoord2f(idx_vert.uv.x, idx_vert.uv.y);
  //             Raylib::rlVertex2f(idx_vert.pos.x, idx_vert.pos.y);
  //           }
  //         }
  //         Raylib::rlEnd();
  //       }
  //       idxOffset += cmd.ElemCount;

  //       processBatchDraws();
  //     }
  //   }

  //   Raylib::rlSetTexture(0);
  //   glBlendEquationSeparate(last_blend_equation_rgb, last_blend_equation_alpha);
  //   glBlendFuncSeparate(last_blend_src_rgb, last_blend_dst_rgb, last_blend_src_alpha, last_blend_dst_alpha);
  //   if (last_enable_blend) glEnable(GL_BLEND); else glDisable(GL_BLEND);
  //   if (last_enable_cull_face) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
  //   if (last_enable_depth_test) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
  //   if (last_enable_stencil_test) glEnable(GL_STENCIL_TEST); else glDisable(GL_STENCIL_TEST);
  //   if (last_enable_scissor_test) glEnable(GL_SCISSOR_TEST); else glDisable(GL_SCISSOR_TEST);
  // }
}

void guiTexture(Texture *texture, Vec2 scale) {
	if (!texture) return;
	Panic("Not implemented");
	// ImGui::Image(
	// 	(ImTextureID)(intptr_t)&texture->backendTexture.raylibTexture.id,
	// 	ImVec2(texture->width * scale.x, texture->height * scale.y),
	// 	ImVec2(0, 1),
	// 	ImVec2(1, 0)
	// );
}
bool guiImageButton(Texture *texture) {
	if (!texture) {
		logf("No texture for guiImageButton");
		return false;
	}
	Panic("Not implemented"); return false;
	// return ImGui::ImageButton(
	// 	(ImTextureID)(intptr_t)&texture->backendTexture.raylibTexture.id,
	// 	ImVec2(texture->width, texture->height),
	// 	ImVec2(0, 1),
	// 	ImVec2(1, 0)
	// );
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
