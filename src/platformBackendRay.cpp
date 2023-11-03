#include "platformBackend.h"

#if defined(_WIN32)
#include "gl.h"
#elif defined(__EMSCRIPTEN__)
#include <GLES3/gl3.h>
#else
#include "gl.h"
#endif

#define IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#include "backends/imgui_impl_opengl3.cpp"

struct BackendNanoTime {
	double time;
};

#ifdef _WIN32
	HANDLE _processHandle;
#endif

void (*_platformFrontendUpdateCallback)();
bool _running;

void backendPlatformUpdateLoop();
PlatformEvent *createPlatformEvent(PlatformEventType type);

void backendPlatformInit(int windowWidth, int windowHeight, char *windowTitle) {
#ifdef _WIN32
	_processHandle = GetCurrentProcess();
#endif

	Raylib::SetTraceLogLevel(Raylib::LOG_WARNING);
	Raylib::SetWindowState(Raylib::FLAG_WINDOW_RESIZABLE);
	Raylib::InitWindow(windowWidth, windowHeight, windowTitle);
	Raylib::SetExitKey(0);
}

float backendPlatformGetWindowScaling() {
	return Raylib::GetWindowScaleDPI().x;
}

void backendPlatformStartUpdateLoop(void (*callback)()) {
	_platformFrontendUpdateCallback = callback;
#ifdef __EMSCRIPTEN__
  emscripten_set_main_loop(backendPlatformUpdateLoop, 0, 1);
#else
  Raylib::SetTargetFPS(60); //@todo Does targetFps matter?

	_running = true;
	for (;;) {
    backendPlatformUpdateLoop();
		if (!_running) break;
    if (Raylib::WindowShouldClose()) break;
  }
#endif
}

void backendPlatformExit() {
	_running = false;
}

void backendPlatformUpdateLoop() {
  int *keysToCheck = (int *)frameMalloc(sizeof(int) * KEYS_MAX);
  int keysToCheckNum = 0;

  for (int i = KEY_LEFT; i < KEYS_MAX; i++) keysToCheck[keysToCheckNum++] = i;
  for (int i = 'A'; i <= 'Z'; i++) keysToCheck[keysToCheckNum++] = i;
  for (int i = '0'; i <= '9'; i++) keysToCheck[keysToCheckNum++] = i;
	keysToCheck[keysToCheckNum++] = ' ';
	keysToCheck[keysToCheckNum++] = '-';
	keysToCheck[keysToCheckNum++] = '=';
	keysToCheck[keysToCheckNum++] = '`';
	keysToCheck[keysToCheckNum++] = '[';
	keysToCheck[keysToCheckNum++] = ']';
	keysToCheck[keysToCheckNum++] = '\\';
	keysToCheck[keysToCheckNum++] = ';';
	keysToCheck[keysToCheckNum++] = '\'';
	keysToCheck[keysToCheckNum++] = ',';
	keysToCheck[keysToCheckNum++] = '.';
	keysToCheck[keysToCheckNum++] = '/';

  for (int i = 0; i < keysToCheckNum; i++) {
    int key = keysToCheck[i];
    int raylibKey = key;
    if (raylibKey == KEY_LEFT) raylibKey = Raylib::KEY_LEFT;
    else if (raylibKey == KEY_RIGHT) raylibKey = Raylib::KEY_RIGHT;
    else if (raylibKey == KEY_UP) raylibKey = Raylib::KEY_UP;
    else if (raylibKey == KEY_DOWN) raylibKey = Raylib::KEY_DOWN;
    else if (raylibKey == KEY_SHIFT) raylibKey = Raylib::KEY_LEFT_SHIFT;
    else if (raylibKey == KEY_RIGHT_SHIFT) raylibKey = Raylib::KEY_RIGHT_SHIFT;
    else if (raylibKey == KEY_BACKSPACE) raylibKey = Raylib::KEY_BACKSPACE;
    else if (raylibKey == KEY_CTRL) raylibKey = Raylib::KEY_LEFT_CONTROL;
    else if (raylibKey == KEY_RIGHT_CTRL) raylibKey = Raylib::KEY_RIGHT_CONTROL;
    else if (raylibKey == KEY_ALT) raylibKey = Raylib::KEY_LEFT_ALT;
    else if (raylibKey == KEY_RIGHT_ALT) raylibKey = Raylib::KEY_RIGHT_ALT;
    else if (raylibKey == KEY_BACKTICK) raylibKey = Raylib::KEY_GRAVE;
    else if (raylibKey == KEY_TAB) raylibKey = Raylib::KEY_TAB;
    else if (raylibKey == KEY_ESC) raylibKey = Raylib::KEY_ESCAPE;
    else if (raylibKey == KEY_F1) raylibKey = Raylib::KEY_F1;
    else if (raylibKey == KEY_F2) raylibKey = Raylib::KEY_F2;
    else if (raylibKey == KEY_F3) raylibKey = Raylib::KEY_F3;
    else if (raylibKey == KEY_F4) raylibKey = Raylib::KEY_F4;
    else if (raylibKey == KEY_F5) raylibKey = Raylib::KEY_F5;
    else if (raylibKey == KEY_F6) raylibKey = Raylib::KEY_F6;
    else if (raylibKey == KEY_F7) raylibKey = Raylib::KEY_F7;
    else if (raylibKey == KEY_F8) raylibKey = Raylib::KEY_F8;
    else if (raylibKey == KEY_F9) raylibKey = Raylib::KEY_F9;
    else if (raylibKey == KEY_F10) raylibKey = Raylib::KEY_F10;
    else if (raylibKey == KEY_F11) raylibKey = Raylib::KEY_F11;
    else if (raylibKey == KEY_F12) raylibKey = Raylib::KEY_F12;
    else if (raylibKey == KEY_ENTER) raylibKey = Raylib::KEY_ENTER;
		else if (raylibKey == KEY_INSERT) raylibKey = Raylib::KEY_INSERT;
		else if (raylibKey == KEY_DELETE) raylibKey = Raylib::KEY_DELETE;
		else if (raylibKey == KEY_HOME) raylibKey = Raylib::KEY_HOME;
		else if (raylibKey == KEY_END) raylibKey = Raylib::KEY_END;
		else if (raylibKey == KEY_PAGE_UP) raylibKey = Raylib::KEY_PAGE_UP;
		else if (raylibKey == KEY_PAGE_DOWN) raylibKey = Raylib::KEY_PAGE_DOWN;
    else if (raylibKey == MOUSE_LEFT) continue; // Handled elsewhere
    else if (raylibKey == MOUSE_RIGHT) continue; // Handled elsewhere

    if (Raylib::IsKeyPressed(raylibKey)) {
      PlatformEvent *event = createPlatformEvent(PLATFORM_EVENT_KEY_DOWN);
      event->keyCode = key;
    }
    if (Raylib::IsKeyReleased(raylibKey)) {
      PlatformEvent *event = createPlatformEvent(PLATFORM_EVENT_KEY_UP);
      event->keyCode = key;
    }
  }

  if (Raylib::IsMouseButtonPressed(Raylib::MOUSE_BUTTON_LEFT)) {
    PlatformEvent *event = createPlatformEvent(PLATFORM_EVENT_KEY_DOWN);
    event->keyCode = MOUSE_LEFT;
  }

  if (Raylib::IsMouseButtonReleased(Raylib::MOUSE_BUTTON_LEFT)) {
    PlatformEvent *event = createPlatformEvent(PLATFORM_EVENT_KEY_UP);
    event->keyCode = MOUSE_LEFT;
  }

  if (Raylib::IsMouseButtonPressed(Raylib::MOUSE_BUTTON_RIGHT)) {
    PlatformEvent *event = createPlatformEvent(PLATFORM_EVENT_KEY_DOWN);
    event->keyCode = MOUSE_RIGHT;
  }

  if (Raylib::IsMouseButtonReleased(Raylib::MOUSE_BUTTON_RIGHT)) {
    PlatformEvent *event = createPlatformEvent(PLATFORM_EVENT_KEY_UP);
    event->keyCode = MOUSE_RIGHT;
  }

  if (Raylib::IsMouseButtonPressed(Raylib::MOUSE_BUTTON_MIDDLE)) {
    PlatformEvent *event = createPlatformEvent(PLATFORM_EVENT_KEY_DOWN);
    event->keyCode = MOUSE_MIDDLE;
  }

  if (Raylib::IsMouseButtonReleased(Raylib::MOUSE_BUTTON_MIDDLE)) {
    PlatformEvent *event = createPlatformEvent(PLATFORM_EVENT_KEY_UP);
    event->keyCode = MOUSE_MIDDLE;
  }

	{
		PlatformEvent *event = createPlatformEvent(PLATFORM_EVENT_MOUSE_MOVE);
		event->position = v2(Raylib::GetMouseX(), Raylib::GetMouseY());
	}

	{
		PlatformEvent *event = createPlatformEvent(PLATFORM_EVENT_MOUSE_WHEEL);
		event->wheelValue = Raylib::GetMouseWheelMove();
	}

	for (;;) {
		int character = Raylib::GetCharPressed();
		if (character == 0) break;
		if (character <= 0) continue;
		if (character >= 255) continue;

		PlatformEvent *event = createPlatformEvent(PLATFORM_EVENT_INPUT_CHARACTER);
		event->keyCode = character;
	}

	_platformFrontendUpdateCallback();
}

int backendPlatformGetWindowWidth() { return Raylib::GetScreenWidth(); }
int backendPlatformGetWindowHeight() { return Raylib::GetScreenHeight(); }

void backendPlatformMaximizeWindow() { Raylib::MaximizeWindow(); }
void backendPlatformMinimizeWindow() { Raylib::MinimizeWindow(); }
void backendPlatformRestoreWindow() { Raylib::RestoreWindow(); }
void backendPlatformResizeWindow(int width, int height) { Raylib::SetWindowSize(width, height); }
void backendHideCursor() { Raylib::HideCursor(); }
void backendShowCursor() { Raylib::RaylibShowCursor(); }

void backendPlatformSleep(int ms) {
#ifdef _WIN32
	Sleep(ms);
#else
	timespec ts;
	ts.tv_sec = ms / 1000;
	ts.tv_nsec = (ms % 1000) * 1000000;
	nanosleep(&ts, NULL);
#endif
}

BackendNanoTime backendPlatformGetNanoTime() {
	BackendNanoTime time = {};
	time.time = Raylib::GetTime();
	return time;
}

float backendPlatformGetMsPassed(BackendNanoTime startTime) {
	BackendNanoTime endTime = backendPlatformGetNanoTime();
	return (endTime.time - startTime.time) * 1000;
}

void backendPlatformSetClipboard(char *str) { Raylib::SetClipboardText(str); }
void backendPlatformNavigateToUrl(char *url) { Raylib::OpenURL(url); }

void backendPlatformShowErrorWindow(char *msg) {
#if defined(_WIN32)
	int result = MessageBoxA(NULL, msg, "Error", MB_OK);
#elif defined(__EMSCRIPTEN__)
	EM_ASM({
		var str = UTF8ToString($0);
		str = str.split("\n").join("<br />");
		crashlogElement.innerHTML += str;
	}, msg);
#else
	printf("Can't show error window on this platform\n");
#endif
}

char *backendPlatformGetLastErrorMessage() {
#if defined(_WIN32)
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
#else
	int myErrno = errno;
	if (myErrno != 0) return frameStringClone(strerror(myErrno));
#endif

  return "No error";
}

int backendPlatformGetMemoryUsage() {
#if defined(_WIN32) && defined(FALLOW_DEBUG)
  PROCESS_MEMORY_COUNTERS pmc;
  if (GetProcessMemoryInfo(_processHandle, &pmc, sizeof(pmc))) return pmc.WorkingSetSize;
  return 0;
#endif
}

Raylib::Texture2D _imGuiFontTexture;
void backendPlatformImGuiInit() {
	ImGuiIO& io = ImGui::GetIO();

  auto raylibImGuiGetClipText = [] (void *)->const char *{ return Raylib::GetClipboardText(); };
  auto raylibImGuiSetClipText = [](void *, const char *text) { Raylib::SetClipboardText(text); };
	io.SetClipboardTextFn = raylibImGuiSetClipText;
	io.GetClipboardTextFn = raylibImGuiGetClipText;

	io.ClipboardUserData = NULL;

#if 1
	ImGui_ImplOpenGL3_Init("#version 300 es");
#else
	{ /// Reload fonts
		ImGuiIO &io = ImGui::GetIO();
		u8 *pixels = NULL;

		int width, height;
		io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height, NULL);
		Raylib::Image image = Raylib::GenImageColor(width, height, Raylib::BLANK);
		memcpy(image.data, pixels, width * height * 4);

		if (_imGuiFontTexture.id != 0) Raylib::UnloadTexture(_imGuiFontTexture);

		_imGuiFontTexture = LoadTextureFromImage(image);
		Raylib::UnloadImage(image);
		io.Fonts->TexID = &_imGuiFontTexture;
	}
#endif
}

void backendPlatformImGuiStartFrame(int windowWidth, int windowHeight) {
	ImGui_ImplOpenGL3_NewFrame();
}

void backendPlatformImGuiDraw() {
#if 1
	ImDrawData *data = ImGui::GetDrawData();
	if (data->CmdListsCount == 0) return;

	ImGui_ImplOpenGL3_RenderDrawData(data);
#else
  { /// rlImGuiEnd();
    void processBatchDraws(); //@headerHack
    processBatchDraws();
    /// rlRenderData(ImGui::GetDrawData());
    ImDrawData *data = ImGui::GetDrawData();
    if (data->CmdListsCount == 0) return;

    GLenum last_blend_src_rgb; glGetIntegerv(GL_BLEND_SRC_RGB, (GLint*)&last_blend_src_rgb);
    GLenum last_blend_dst_rgb; glGetIntegerv(GL_BLEND_DST_RGB, (GLint*)&last_blend_dst_rgb);
    GLenum last_blend_src_alpha; glGetIntegerv(GL_BLEND_SRC_ALPHA, (GLint*)&last_blend_src_alpha);
    GLenum last_blend_dst_alpha; glGetIntegerv(GL_BLEND_DST_ALPHA, (GLint*)&last_blend_dst_alpha);
    GLenum last_blend_equation_rgb; glGetIntegerv(GL_BLEND_EQUATION_RGB, (GLint*)&last_blend_equation_rgb);
    GLenum last_blend_equation_alpha; glGetIntegerv(GL_BLEND_EQUATION_ALPHA, (GLint*)&last_blend_equation_alpha);
    GLboolean last_enable_blend = glIsEnabled(GL_BLEND);
    GLboolean last_enable_cull_face = glIsEnabled(GL_CULL_FACE);
    GLboolean last_enable_depth_test = glIsEnabled(GL_DEPTH_TEST);
    GLboolean last_enable_stencil_test = glIsEnabled(GL_STENCIL_TEST);
    GLboolean last_enable_scissor_test = glIsEnabled(GL_SCISSOR_TEST);

    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);
    glEnable(GL_SCISSOR_TEST);

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
							Vec2 vert = v2(idx_vert.pos.x, idx_vert.pos.y);
							vert = getProjectionMatrix(Raylib::GetScreenWidth(), Raylib::GetScreenHeight()) * vert;
              Raylib::rlVertex2f(vert.x, vert.y);
            }
          }
          Raylib::rlEnd();
        }
        idxOffset += cmd.ElemCount;

        processBatchDraws();
      }
    }

    Raylib::rlSetTexture(0);
    glBlendEquationSeparate(last_blend_equation_rgb, last_blend_equation_alpha);
    glBlendFuncSeparate(last_blend_src_rgb, last_blend_dst_rgb, last_blend_src_alpha, last_blend_dst_alpha);
    if (last_enable_blend) glEnable(GL_BLEND); else glDisable(GL_BLEND);
    if (last_enable_cull_face) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
    if (last_enable_depth_test) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
    if (last_enable_stencil_test) glEnable(GL_STENCIL_TEST); else glDisable(GL_STENCIL_TEST);
    if (last_enable_scissor_test) glEnable(GL_SCISSOR_TEST); else glDisable(GL_SCISSOR_TEST);
  }
#endif
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
};
