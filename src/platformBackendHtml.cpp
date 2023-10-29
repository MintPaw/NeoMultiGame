#include "platformBackend.h"

#include <GLES3/gl3.h>

struct BackendNanoTime {
	timespec timeSpec;
};

void (*_platformFrontendUpdateCallback)();
PlatformEvent _heldPlatformEvents[PLATFORM_EVENTS_MAX];
int _heldPlatformEventsNum;

void doCanvasElementResize();
void backendPlatformUpdateLoop();

void backendPlatformInit(int windowWidth, int windowHeight, char *windowTitle) {
	EM_ASM({
		let ctx = Module.createContext(canvas, true, true);
		if (!ctx) console.log("Failed to create opengl context!\n");
	});

	auto keyCallback = [](int eventType, const EmscriptenKeyboardEvent *emEvent, void *userData)->EM_BOOL {
		if (_heldPlatformEventsNum > PLATFORM_EVENTS_MAX-1) {
			logf("Too many platform events!\n");
			return EM_TRUE;
		}

		if (eventType == EMSCRIPTEN_EVENT_KEYDOWN) {
			PlatformEvent *event = &_heldPlatformEvents[_heldPlatformEventsNum++];
			memset(event, 0, sizeof(PlatformEvent));
			event->type = PLATFORM_EVENT_KEY_DOWN;
			event->keyCode = toUpperCase(emEvent->key[0]);
		} else if (eventType == EMSCRIPTEN_EVENT_KEYUP) {
			PlatformEvent *event = &_heldPlatformEvents[_heldPlatformEventsNum++];
			memset(event, 0, sizeof(PlatformEvent));
			event->type = PLATFORM_EVENT_KEY_UP;
			event->keyCode = toUpperCase(emEvent->key[0]);
		}
		//@incomplete keyboard events
		return EM_TRUE;
	};
	emscripten_set_keydown_callback("#canvas", NULL, true, keyCallback);
	emscripten_set_keyup_callback("#canvas", NULL, true, keyCallback);

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
			float windowScaling = emscripten_get_device_pixel_ratio();
			event->position.x = emEvent->targetX * windowScaling;
			event->position.y = emEvent->targetY * windowScaling;
			// logf("screen: %f %f\n", (float)emEvent->screenX, (float)emEvent->screenY);
			// logf("client: %f %f\n", (float)emEvent->clientX, (float)emEvent->clientY);
			// logf("movement: %f %f\n", (float)emEvent->movementX, (float)emEvent->movementY);
			// logf("target: %f %f\n", (float)emEvent->targetX, (float)emEvent->targetY);
			// logf("canvas: %f %f\n", (float)emEvent->canvasX, (float)emEvent->canvasY);
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

	float windowScaling = emscripten_get_device_pixel_ratio();
	emscripten_set_canvas_element_size("#canvas", windowWidth, windowHeight);
	emscripten_set_element_css_size("#canvas", windowWidth/windowScaling, windowHeight/windowScaling);
}

float backendPlatformGetWindowScaling() {
	return emscripten_get_device_pixel_ratio();
}

void backendPlatformStartUpdateLoop(void (*callback)()) {
	_platformFrontendUpdateCallback = callback;
  emscripten_set_main_loop(backendPlatformUpdateLoop, 0, 1);
}

void backendPlatformExit() { }

void doCanvasElementResize() {
	double canvasWidth, canvasHeight;
	float windowScaling = emscripten_get_device_pixel_ratio();
	emscripten_get_element_css_size("#canvas", &canvasWidth, &canvasHeight);
	emscripten_set_canvas_element_size("#canvas", canvasWidth*windowScaling, canvasHeight*windowScaling);
}

float _lastWindowScaling = 1;
void backendPlatformUpdateLoop() {
	float windowScaling = emscripten_get_device_pixel_ratio();
	if (_lastWindowScaling != windowScaling) {
		_lastWindowScaling = windowScaling;
		doCanvasElementResize();
	}

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
	float windowScaling = emscripten_get_device_pixel_ratio();
  return canvasHeight * backendPlatformGetWindowScaling();
}

void backendPlatformMaximizeWindow() { }
void backendPlatformMinimizeWindow() { }
void backendPlatformMestoreWindow() { }

void backendPlatformResizeWindow(int width, int height) {
	//@incomplete
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
	//@incomplete
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

int backendPlatformGetMemoryUsage() {
	return 0; //@incomplete
}

void backendPlatformImGuiInit() {
//@incomplete
// #if defined(FALLOW_COMMAND_LINE_ONLY)
// 	_imGuiDisable = true;
// #endif
// 	if (_imGuiDisable) return;

// 	_imGuiCurrentMouseCursor = ImGuiMouseCursor_COUNT;

// 	ImGui::CreateContext(NULL);

// 	ImGui::StyleColorsDark();

// 	ImGuiIO& io = ImGui::GetIO();
// 	io.BackendPlatformName = "imGui_impl_raylib";

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


//   auto raylibImGuiGetClipText = [] (void *)->const char *{ return Raylib::GetClipboardText(); };
//   auto raylibImGuiSetClipText = [](void *, const char *text) { Raylib::SetClipboardText(text); };
// 	io.SetClipboardTextFn = raylibImGuiSetClipText;
// 	io.GetClipboardTextFn = raylibImGuiGetClipText;

// 	io.ClipboardUserData = NULL;

// 	ImGui::GetStyle().ScaleAllSizes(windowScaling);
// 	ImGui::GetIO().FontGlobalScale *= windowScaling;

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

// 		if (_imGuiFontTexture.id != 0) Raylib::UnloadTexture(_imGuiFontTexture);

// 		_imGuiFontTexture = LoadTextureFromImage(image);
// 		Raylib::UnloadImage(image);
// 		io.Fonts->TexID = &_imGuiFontTexture;
// 	}
}

void backendPlatformImGuiStartFrame(int windowWidth, int windowHeight) {
//@incomplete
	// _imGuiNeedToDrawThisFrame = true;
	// _imGuiHoveringGui = false;
	// _imGuiTypingGui = false;
	// if (_imGuiDisable) return;

	// { /// rlImGuiNewFrame();
	// 	ImGuiIO& io = ImGui::GetIO();

	// 	io.DisplaySize.x = windowWidth;
	// 	io.DisplaySize.y = windowHeight;

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
	// 		if (imgui_cursor != _imGuiCurrentMouseCursor || io.MouseDrawCursor)
	// 		{
	// 			_imGuiCurrentMouseCursor = imgui_cursor;
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

	// 	_imGuiHoveringGui = io.WantCaptureMouse;
	// 	_imGuiTypingGui = io.WantTextInput;
	// }

	// ImGui::NewFrame();
}

void backendPlatformImGuiDraw() {
	//@incomplete
	// if (!_imGuiNeedToDrawThisFrame) return;
	// _imGuiNeedToDrawThisFrame = false;
	// if (_imGuiDisable) return;

  // { /// rlImGuiEnd();
    // void processBatchDraws(); //@headerHack
    // processBatchDraws();

    // ImGui::Render();

    // /// rlRenderData(ImGui::GetDrawData());
    // ImDrawData *data = ImGui::GetDrawData();
    // if (data->CmdListsCount == 0) return;

    // GLenum last_blend_src_rgb; glGetIntegerv(GL_BLEND_SRC_RGB, (GLint*)&last_blend_src_rgb);
    // GLenum last_blend_dst_rgb; glGetIntegerv(GL_BLEND_DST_RGB, (GLint*)&last_blend_dst_rgb);
    // GLenum last_blend_src_alpha; glGetIntegerv(GL_BLEND_SRC_ALPHA, (GLint*)&last_blend_src_alpha);
    // GLenum last_blend_dst_alpha; glGetIntegerv(GL_BLEND_DST_ALPHA, (GLint*)&last_blend_dst_alpha);
    // GLenum last_blend_equation_rgb; glGetIntegerv(GL_BLEND_EQUATION_RGB, (GLint*)&last_blend_equation_rgb);
    // GLenum last_blend_equation_alpha; glGetIntegerv(GL_BLEND_EQUATION_ALPHA, (GLint*)&last_blend_equation_alpha);
    // GLboolean last_enable_blend = glIsEnabled(GL_BLEND);
    // GLboolean last_enable_cull_face = glIsEnabled(GL_CULL_FACE);
    // GLboolean last_enable_depth_test = glIsEnabled(GL_DEPTH_TEST);
    // GLboolean last_enable_stencil_test = glIsEnabled(GL_STENCIL_TEST);
    // GLboolean last_enable_scissor_test = glIsEnabled(GL_SCISSOR_TEST);

    // glEnable(GL_BLEND);
    // glBlendEquation(GL_FUNC_ADD);
    // glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    // glDisable(GL_CULL_FACE);
    // glDisable(GL_DEPTH_TEST);
    // glDisable(GL_STENCIL_TEST);
    // glEnable(GL_SCISSOR_TEST);

    // for (int l = 0; l < data->CmdListsCount; ++l) {
      // int idxOffset = 0;

      // const ImDrawList* commandList = data->CmdLists[l];

      // for (const auto& cmd : commandList->CmdBuffer) {
        // { /// EnableScissor(...)
          // Raylib::rlEnableScissorTest();
          // float x = cmd.ClipRect.x - data->DisplayPos.x;
          // float y = cmd.ClipRect.y - data->DisplayPos.y;
          // float width = cmd.ClipRect.z - (cmd.ClipRect.x - data->DisplayPos.x);
          // float height = cmd.ClipRect.w - (cmd.ClipRect.y - data->DisplayPos.y);
          // Raylib::rlScissor((int)x, Raylib::GetScreenHeight() - (int)(y + height), (int)width, (int)height);
        // }
        // if (cmd.UserCallback != nullptr) {
          // cmd.UserCallback(commandList, &cmd);
          // idxOffset += cmd.ElemCount;
          // continue;
        // }

        // { /// rlImGuiRenderTriangles(cmd.ElemCount, idxOffset, commandList->IdxBuffer, commandList->VtxBuffer, cmd.TextureId);
          // unsigned int count = cmd.ElemCount;
          // int indexStart = idxOffset;
          // const ImVector<ImDrawIdx>& indexBuffer = commandList->IdxBuffer;
          // const ImVector<ImDrawVert>& vertBuffer = commandList->VtxBuffer;
          // Raylib::Texture* texture = (Raylib::Texture*)cmd.TextureId;

          // unsigned int textureId = (texture == nullptr) ? 0 : texture->id;

          // Raylib::rlBegin(RL_TRIANGLES);
          // Raylib::rlSetTexture(textureId);

          // for (unsigned int i = 0; i <= (count - 3); i += 3) {
            // if(Raylib::rlCheckRenderBatchLimit(3)) {
              // Raylib::rlBegin(RL_TRIANGLES);
              // Raylib::rlSetTexture(textureId);
            // }

            // ImDrawIdx indexA = indexBuffer[indexStart + i];
            // ImDrawIdx indexB = indexBuffer[indexStart + i + 1];
            // ImDrawIdx indexC = indexBuffer[indexStart + i + 2];

            // ImDrawVert verts[3] = {
              // vertBuffer[indexA],
              // vertBuffer[indexB],
              // vertBuffer[indexC],
            // };

            // for (int i = 0; i < 3; i++) {
              // ImDrawVert idx_vert = verts[i];
              // Raylib::Color *c = (Raylib::Color*)&idx_vert.col;
              // Raylib::rlColor4ub(c->r, c->g, c->b, c->a);
              // Raylib::rlTexCoord2f(idx_vert.uv.x, idx_vert.uv.y);
              // Raylib::rlVertex2f(idx_vert.pos.x, idx_vert.pos.y);
            // }
          // }
          // Raylib::rlEnd();
        // }
        // idxOffset += cmd.ElemCount;

        // processBatchDraws();
      // }
    // }

    // Raylib::rlSetTexture(0);
    // glBlendEquationSeparate(last_blend_equation_rgb, last_blend_equation_alpha);
    // glBlendFuncSeparate(last_blend_src_rgb, last_blend_dst_rgb, last_blend_src_alpha, last_blend_dst_alpha);
    // if (last_enable_blend) glEnable(GL_BLEND); else glDisable(GL_BLEND);
    // if (last_enable_cull_face) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
    // if (last_enable_depth_test) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
    // if (last_enable_stencil_test) glEnable(GL_STENCIL_TEST); else glDisable(GL_STENCIL_TEST);
    // if (last_enable_scissor_test) glEnable(GL_SCISSOR_TEST); else glDisable(GL_SCISSOR_TEST);
  // }
}
