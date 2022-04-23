#ifdef GUI_HEADER
#undef GUI_HEADER

#if defined(FALLOW_IMGUI)

# include "imgui_impl_sdl.cpp"

# ifdef IMGUI_OBJ
#  include "imgui.h"
// #  include "implot.h"
// #  include "implot_internal.h"
#  include "imstb_truetype.h"
#  include "imstb_rectpack.h"
# else
#  include "imgui.cpp"
#  include "imgui_widgets.cpp"
#  include "imgui_draw.cpp"
#  include "imgui_tables.cpp"
#  include "imgui_demo.cpp"
// #  include "implot.cpp"
// #  include "implot_items.cpp"
# endif

# if defined(_MSC_VER) && _MSC_VER <= 1500 // MSVC 2008 or earlier
#  include <stddef.h>
# else
#  include <stdint.h>
# endif

# if defined(__APPLE__)
#  include "TargetConditionals.h"
# endif

# include <GL/glew.h>
#endif

void guiInit();
void guiStartFrame();

void guiUpdateEvents();
void guiProcessSdlEvent(SDL_Event *e);

void guiDraw();

void guiTexture(Texture *texture);
void guiTexture(RenderTexture *renderTexture) { guiTexture(renderTexture->texture); }
bool guiImageButton(Texture *texture);
bool guiImageButton(RenderTexture *texture) { return guiImageButton(texture->texture); }
// bool guiInputRgb(const char *name, int *argb, bool showInputs=false);
// bool guiInputArgb(const char *name, int *argb, bool showInputs=false);
// void guiPushStyleColor(ImGuiCol style, int color);
// void guiPopStyleColor(int amount=1);

bool useGuiIni = false;
struct Gui {
	GLuint vboId;
	GLuint vao;
	GLuint elementsId;
	GLuint fontTextureId;

	GLuint shaderId;
	GLuint arbTex;
	GLuint arbProjMat;
	GLuint arbPosition;
	GLuint arbUV;
	GLuint arbColor;

	bool hidden;
	bool needToDrawThisFrame;
	GLuint glVersion;

	ImFont *defaultFont;
	ImFont *bigFont;
};

Gui *gui = NULL;

#include "guiUtils.cpp"

IMGUI_IMPL_API bool ImGui_ImplOpenGL3_Init(const char* glsl_version=NULL);
IMGUI_IMPL_API void ImGui_ImplOpenGL3_Shutdown();
IMGUI_IMPL_API void ImGui_ImplOpenGL3_NewFrame();
IMGUI_IMPL_API void ImGui_ImplOpenGL3_RenderDrawData(ImDrawData* draw_data);

#endif // GUI_HEADER

#ifdef GUI_IMPL
#undef GUI_IMPL

IMGUI_IMPL_API bool ImGui_ImplOpenGL3_Init(const char* glsl_version) {
	return true;
}

IMGUI_IMPL_API void ImGui_ImplOpenGL3_Shutdown() {
	if (gui->vboId) glDeleteBuffers(1, &gui->vboId);
	if (gui->elementsId) glDeleteBuffers(1, &gui->elementsId);
	gui->vboId = gui->elementsId = 0;

	if (gui->shaderId) glDeleteProgram(gui->shaderId);
	gui->shaderId = 0;

	if (gui->fontTextureId) {
		ImGuiIO& io = ImGui::GetIO();
		io.Fonts->TexID = 0;

		glDeleteTextures(1, &gui->fontTextureId);
		gui->fontTextureId = 0;
	}
}

IMGUI_IMPL_API void ImGui_ImplOpenGL3_NewFrame() {
	if (gui->fontTextureId) return;

#ifdef GL_ES
	const GLchar *vertex_shader =
		"#version 100\n"
		"uniform mat4 ProjMtx;\n"
		"attribute vec2 Position;\n"
		"attribute vec2 UV;\n"
		"attribute vec4 Color;\n"
		"varying vec2 Frag_UV;\n"
		"varying vec4 Frag_Color;\n"
		"void main()\n"
		"{\n"
		"    Frag_UV = UV;\n"
		"    Frag_Color = Color;\n"
		"    gl_Position = ProjMtx * vec4(Position.xy,0,1);\n"
		"}\n";

	const GLchar *fragment_shader =
		"#version 100\n"
		"#ifdef GL_ES\n"
		"    precision mediump float;\n"
		"#endif\n"
		"uniform sampler2D Texture;\n"
		"varying vec2 Frag_UV;\n"
		"varying vec4 Frag_Color;\n"
		"void main()\n"
		"{\n"
		"    gl_FragColor = Frag_Color * texture2D(Texture, Frag_UV.st);\n"
		"}\n";
#else
	const GLchar *vertex_shader =
		"#version 330\n"
		"precision mediump float;\n"
		"layout (location = 0) in vec2 Position;\n"
		"layout (location = 1) in vec2 UV;\n"
		"layout (location = 2) in vec4 Color;\n"
		"uniform mat4 ProjMtx;\n"
		"out vec2 Frag_UV;\n"
		"out vec4 Frag_Color;\n"
		"void main()\n"
		"{\n"
		"    Frag_UV = UV;\n"
		"    Frag_Color = Color;\n"
		"    gl_Position = ProjMtx * vec4(Position.xy,0,1);\n"
		"}\n";

	const GLchar *fragment_shader =
		"#version 330\n"
		"precision mediump float;\n"
		"uniform sampler2D Texture;\n"
		"in vec2 Frag_UV;\n"
		"in vec4 Frag_Color;\n"
		"layout (location = 0) out vec4 Out_Color;\n"
		"void main()\n"
		"{\n"
		"    Out_Color = Frag_Color * texture(Texture, Frag_UV.st);\n"
		"}\n";
#endif

	// Create shaders
	GLuint vertHandle = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertHandle, 1, &vertex_shader, NULL);
	glCompileShader(vertHandle);

	static const int errLogLimit = 1024;
	char errLog[errLogLimit];
	int errLogNum;

	int vertReturn;
	glGetShaderiv(vertHandle, GL_COMPILE_STATUS, &vertReturn);
	if (!vertReturn) {
		glGetShaderInfoLog(vertHandle, errLogLimit, &errLogNum, errLog);
		logf("%s\ngui vertex shader result is: %d\nError(%d):\n%s\n", vertex_shader, vertReturn, errLogNum, errLog);
	}

	GLuint fragHandle = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragHandle, 1, &fragment_shader, NULL);
	glCompileShader(fragHandle);

	int fragReturn;
	glGetShaderiv(fragHandle, GL_COMPILE_STATUS, &fragReturn);
	if (!fragReturn) {
		glGetShaderInfoLog(fragHandle, errLogLimit, &errLogNum, errLog);
		logf("gui fragment shader result is: %d\nError(%d):\n%s\n", fragReturn, errLogNum, errLog);
	}

	gui->shaderId = glCreateProgram();
	glAttachShader(gui->shaderId, vertHandle);
	glAttachShader(gui->shaderId, fragHandle);
	glLinkProgram(gui->shaderId);

	int progReturn;
	glGetProgramiv(gui->shaderId, GL_LINK_STATUS, &progReturn);
	if (!progReturn) {
		glGetShaderInfoLog(gui->shaderId, errLogLimit, &errLogNum, errLog);
		logf("Program result is: %d\nError:\n%s\n", progReturn, errLog);
	}

	gui->arbTex = glGetUniformLocation(gui->shaderId, "Texture");
	gui->arbProjMat = glGetUniformLocation(gui->shaderId, "ProjMtx");
	gui->arbPosition = glGetAttribLocation(gui->shaderId, "Position");
	gui->arbUV = glGetAttribLocation(gui->shaderId, "UV");
	gui->arbColor = glGetAttribLocation(gui->shaderId, "Color");

	glGenBuffers(1, &gui->vboId);
	glGenBuffers(1, &gui->elementsId);

	/// ImGui_ImplOpenGL3_CreateFontsTexture
	// Build texture atlas
	ImGuiIO& io = ImGui::GetIO();
	unsigned char *pixels;
	int width, height;
	io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

	// Upload texture to graphics system
	glGenTextures(1, &gui->fontTextureId);
	bindTextureSlotId(0, gui->fontTextureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	// Store our identifier
	io.Fonts->TexID = (ImTextureID)(intptr_t)gui->fontTextureId;

	CheckGlErrorHigh();
}

IMGUI_IMPL_API void ImGui_ImplOpenGL3_RenderDrawData(ImDrawData* draw_data) {
	if (gui->hidden) return;
	processBatchDraws();
	// Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
	ImGuiIO& io = ImGui::GetIO();
	int fb_width = (int)(draw_data->DisplaySize.x * io.DisplayFramebufferScale.x);
	int fb_height = (int)(draw_data->DisplaySize.y * io.DisplayFramebufferScale.y);
	if (fb_width <= 0 || fb_height <= 0)
		return;
	draw_data->ScaleClipRects(io.DisplayFramebufferScale);

	// Backup GL state
	// GLenum last_active_texture; glGetIntegerv(GL_ACTIVE_TEXTURE, (GLint*)&last_active_texture);
	glActiveTexture(GL_TEXTURE0);
	// GLint last_program; glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
	// GLint last_texture; glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
	// GLint last_array_buffer; glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
	// GLint last_vertex_array; glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);

	// GLint last_viewport[4]; glGetIntegerv(GL_VIEWPORT, last_viewport);
	GLint last_scissor_box[4]; glGetIntegerv(GL_SCISSOR_BOX, last_scissor_box);
	// setBlendMode(BLEND_IMGUI);
	// GLenum last_blend_src_rgb; glGetIntegerv(GL_BLEND_SRC_RGB, (GLint*)&last_blend_src_rgb);
	// GLenum last_blend_dst_rgb; glGetIntegerv(GL_BLEND_DST_RGB, (GLint*)&last_blend_dst_rgb);
	// GLenum last_blend_src_alpha; glGetIntegerv(GL_BLEND_SRC_ALPHA, (GLint*)&last_blend_src_alpha);
	// GLenum last_blend_dst_alpha; glGetIntegerv(GL_BLEND_DST_ALPHA, (GLint*)&last_blend_dst_alpha);
	// GLenum last_blend_equation_rgb; glGetIntegerv(GL_BLEND_EQUATION_RGB, (GLint*)&last_blend_equation_rgb);
	// GLenum last_blend_equation_alpha; glGetIntegerv(GL_BLEND_EQUATION_ALPHA, (GLint*)&last_blend_equation_alpha);
	GLboolean last_enable_blend = glIsEnabled(GL_BLEND);
	GLboolean last_enable_cull_face = glIsEnabled(GL_CULL_FACE);
	GLboolean last_enable_depth_test = glIsEnabled(GL_DEPTH_TEST);
	GLboolean last_enable_scissor_test = glIsEnabled(GL_SCISSOR_TEST);

	// Setup render state: alpha-blending enabled, no face culling, no depth testing, scissor enabled, polygon fill
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	// glDisable(GL_PROGRAM_POINT_SIZE);
	glEnable(GL_SCISSOR_TEST);
	// glDepthMask(true);

	// Setup viewport, orthographic projection matrix
	// Our visible imgui space lies from draw_data->DisplayPps (top left) to draw_data->DisplayPos+data_data->DisplaySize (bottom right). DisplayMin is typically (0,0) for single viewport apps.
	// Setup viewport, orthographic projection matrix
	// Our visible imgui space lies from draw_data->DisplayPos (top left) to draw_data->DisplayPos+data_data->DisplaySize (bottom right). DisplayPos is (0,0) for single viewport apps.
#if defined(GL_CLIP_ORIGIN)
	bool clip_origin_lower_left = true;
	if (gui->glVersion >= 450)
	{
		GLenum current_clip_origin = 0; glGetIntegerv(GL_CLIP_ORIGIN, (GLint*)&current_clip_origin);
		if (current_clip_origin == GL_UPPER_LEFT)
			clip_origin_lower_left = false;
	}
#endif

	// Setup viewport, orthographic projection matrix
	// Our visible imgui space lies from draw_data->DisplayPos (top left) to draw_data->DisplayPos+data_data->DisplaySize (bottom right). DisplayPos is (0,0) for single viewport apps.
	glViewport(0, 0, (GLsizei)fb_width, (GLsizei)fb_height);
	float Le = draw_data->DisplayPos.x;
	float Ri = draw_data->DisplayPos.x + draw_data->DisplaySize.x;
	float T = draw_data->DisplayPos.y;
	float B = draw_data->DisplayPos.y + draw_data->DisplaySize.y;
#if defined(GL_CLIP_ORIGIN)
	if (!clip_origin_lower_left) { float tmp = T; T = B; B = tmp; } // Swap top and bottom if origin is upper left
#endif
	const float ortho_projection[4][4] =
	{
		{ 2.0f/(Ri-Le),   0.0f,         0.0f,   0.0f },
		{ 0.0f,         2.0f/(T-B),   0.0f,   0.0f },
		{ 0.0f,         0.0f,        -1.0f,   0.0f },
		{ (Ri+Le)/(Le-Ri),  (T+B)/(B-T),  0.0f,   1.0f },
	};
	setShaderProgram(gui->shaderId);
	glUniform1i(gui->arbTex, 0);
	glUniformMatrix4fv(gui->arbProjMat, 1, GL_FALSE, &ortho_projection[0][0]);
	// Recreate the VAO every time
	// (This is to easily allow multiple GL contexts. VAO are not shared among GL contexts, and we don't track creation/deletion of windows so we don't have an obvious key to use to cache them.)
#ifdef IMGUI_IMPL_OPENGL_MAY_HAVE_BIND_SAMPLER
	if (gui->glVersion >= 330) glBindSampler(0, 0); // We use combined texture/sampler state. Applications using GL 3.3 may set that otherwise.
#endif
	GLuint vao;
	glGenVertexArrays(1, &vao);
	bindVertexArray(vao);
	bindVertexBuffer(gui->vboId);
	bindElementBuffer(gui->elementsId);
	glEnableVertexAttribArray(gui->arbPosition);
	glEnableVertexAttribArray(gui->arbUV);
	glEnableVertexAttribArray(gui->arbColor);
	glVertexAttribPointer(gui->arbPosition, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)IM_OFFSETOF(ImDrawVert, pos));
	glVertexAttribPointer(gui->arbUV, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)IM_OFFSETOF(ImDrawVert, uv));
	glVertexAttribPointer(gui->arbColor, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ImDrawVert), (GLvoid*)IM_OFFSETOF(ImDrawVert, col));

	// Draw
	ImVec2 clip_off = draw_data->DisplayPos;         // (0,0) unless using multi-viewports
	ImVec2 clip_scale = draw_data->FramebufferScale; // (1,1) unless using retina display which are often (2,2)

	// Render command lists
	for (int n = 0; n < draw_data->CmdListsCount; n++)
	{
		const ImDrawList *cmd_list = draw_data->CmdLists[n];

		// Upload vertex/index buffers
		glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)cmd_list->VtxBuffer.Size * (int)sizeof(ImDrawVert), (const GLvoid*)cmd_list->VtxBuffer.Data, GL_STREAM_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)cmd_list->IdxBuffer.Size * (int)sizeof(ImDrawIdx), (const GLvoid*)cmd_list->IdxBuffer.Data, GL_STREAM_DRAW);

		for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++) {
			const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
			if (pcmd->UserCallback != NULL) {
				logf("No gui user callbacks!\n");
				// // User callback, registered via ImDrawList::AddCallback()
				// // (ImDrawCallback_ResetRenderState is a special callback value used by the user to request the renderer to reset render state.)
				// if (pcmd->UserCallback == ImDrawCallback_ResetRenderState)
				// 	ImGui_ImplOpenGL3_SetupRenderState(draw_data, fb_width, fb_height, vertex_array_object);
				// else
				// 	pcmd->UserCallback(cmd_list, pcmd);
			} else {
				// Project scissor/clipping rectangles into framebuffer space
				ImVec4 clip_rect;
				clip_rect.x = (pcmd->ClipRect.x - clip_off.x) * clip_scale.x;
				clip_rect.y = (pcmd->ClipRect.y - clip_off.y) * clip_scale.y;
				clip_rect.z = (pcmd->ClipRect.z - clip_off.x) * clip_scale.x;
				clip_rect.w = (pcmd->ClipRect.w - clip_off.y) * clip_scale.y;

				if (clip_rect.x < fb_width && clip_rect.y < fb_height && clip_rect.z >= 0.0f && clip_rect.w >= 0.0f) {
					// Apply scissor/clipping rectangle
					glScissor((int)clip_rect.x, (int)(fb_height - clip_rect.w), (int)(clip_rect.z - clip_rect.x), (int)(clip_rect.w - clip_rect.y));

					// Bind texture, Draw
					glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd->TextureId);
					if (gui->glVersion >= 320) {
#ifndef __EMSCRIPTEN__
						glDrawElementsBaseVertex(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, (void*)(intptr_t)(pcmd->IdxOffset * sizeof(ImDrawIdx)), (GLint)pcmd->VtxOffset);
#endif
					} else {
						glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, (void*)(intptr_t)(pcmd->IdxOffset * sizeof(ImDrawIdx)));
					}
				}
			}
		}
	}

	// Restore modified GL state
	// glUseProgram(last_program);
	// glBindTexture(GL_TEXTURE_2D, last_texture);
	// glActiveTexture(last_active_texture);
	// glBindVertexArray(last_vertex_array);
	// glBindVertexArray(renderer->defaultVao);
	// glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
	// glBlendEquationSeparate(last_blend_equation_rgb, last_blend_equation_alpha);
	// glBlendFuncSeparate(last_blend_src_rgb, last_blend_dst_rgb, last_blend_src_alpha, last_blend_dst_alpha);
	glDeleteVertexArrays(1, &vao);
	bindVertexArray(renderer->defaultVao);
	if (last_enable_blend) glEnable(GL_BLEND); else glDisable(GL_BLEND);
	setRendererBlendMode(BLEND_NORMAL);
	if (last_enable_cull_face) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
	if (last_enable_depth_test) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
	if (last_enable_scissor_test) glEnable(GL_SCISSOR_TEST); else glDisable(GL_SCISSOR_TEST);
	// glViewport(last_viewport[0], last_viewport[1], (GLsizei)last_viewport[2], (GLsizei)last_viewport[3]);
	glViewport(renderer->currentViewport.x, renderer->currentViewport.y, renderer->currentViewport.width, renderer->currentViewport.height);
	glScissor(last_scissor_box[0], last_scissor_box[1], (GLsizei)last_scissor_box[2], (GLsizei)last_scissor_box[3]);
}

void guiInit() {
#if defined(FALLOW_IMGUI)
	gui = (Gui *)zalloc(sizeof(Gui));

	{
		GLint major = 0;
		GLint minor = 0;
		glGetIntegerv(GL_MAJOR_VERSION, &major);
		glGetIntegerv(GL_MINOR_VERSION, &minor);
		if (major == 0 && minor == 0)
		{
			// Query GL_VERSION in desktop GL 2.x, the string will start with "<major>.<minor>"
			const char* gl_version = (const char*)glGetString(GL_VERSION);
			sscanf(gl_version, "%d.%d", &major, &minor);
		}
		gui->glVersion = (GLuint)(major * 100 + minor * 10);
	}

	if (platform->isCommandLineOnly) platform->disableGui = true;
	if (platform->disableGui) return;

	ImGui::CreateContext();
#ifdef IMPLOT_VERSION 
	ImPlot::CreateContext();
#endif
	ImGuiIO &io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
	io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;

	int fontDataSize;
	u8 *fontData = (u8 *)readFile("assets/common/arial.ttf", &fontDataSize);
	gui->defaultFont = io.Fonts->AddFontFromMemoryTTF(fontData, fontDataSize, 13);

	fontData = (u8 *)readFile("assets/common/arial.ttf", &fontDataSize);
	gui->bigFont = io.Fonts->AddFontFromMemoryTTF(fontData, fontDataSize, 26);

#if 0
	ImGuiStyle *style = &ImGui::GetStyle();
	style->WindowPadding = ImVec2(4, 4);
	style->PopupRounding = 0;
	style->FramePadding = ImVec2(2, 2);
	style->ItemSpacing = ImVec2(8, 6);
	style->ItemInnerSpacing = ImVec2(2, 4);
	style->IndentSpacing = 21;
	style->ScrollbarSize = 16;
	style->GrabMinSize = 10;

	style->WindowRounding = 0;
	style->FrameRounding = 7;
	style->ScrollbarRounding = 12;
	style->GrabRounding = 7;

	style->Colors[ImGuiCol_FrameBg]                = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
	style->Colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.74f, 0.74f, 0.74f, 0.40f);
	style->Colors[ImGuiCol_FrameBgActive]          = ImVec4(0.77f, 0.77f, 0.77f, 0.67f);
	style->Colors[ImGuiCol_TitleBgActive]          = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
	style->Colors[ImGuiCol_CheckMark]              = ImVec4(0.54f, 0.54f, 0.54f, 1.00f);
	style->Colors[ImGuiCol_SliderGrab]             = ImVec4(0.42f, 0.42f, 0.42f, 1.00f);
	style->Colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.53f, 0.53f, 0.53f, 1.00f);
	style->Colors[ImGuiCol_Button]                 = ImVec4(0.56f, 0.56f, 0.56f, 0.40f);
	style->Colors[ImGuiCol_ButtonHovered]          = ImVec4(0.58f, 0.58f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_ButtonActive]           = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
	style->Colors[ImGuiCol_Header]                 = ImVec4(0.49f, 0.49f, 0.49f, 0.31f);
	style->Colors[ImGuiCol_HeaderHovered]          = ImVec4(0.52f, 0.52f, 0.52f, 0.80f);
	style->Colors[ImGuiCol_HeaderActive]           = ImVec4(0.45f, 0.45f, 0.45f, 1.00f);
	style->Colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.23f, 0.23f, 0.23f, 0.78f);
	style->Colors[ImGuiCol_SeparatorActive]        = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
	style->Colors[ImGuiCol_ResizeGrip]             = ImVec4(0.44f, 0.44f, 0.45f, 0.25f);
	style->Colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.48f, 0.49f, 0.49f, 0.67f);
	style->Colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.47f, 0.47f, 0.47f, 0.95f);
	style->Colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.64f, 0.64f, 0.64f, 0.35f);
	style->Colors[ImGuiCol_NavHighlight]           = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
#else
	ImGui::StyleColorsDark();
#endif
#ifdef IMPLOT_VERSION 
	ImPlot::GetStyle().AntiAliasedLines = true;
#endif

	if (useGuiIni) {
		int iniNameLen = strlen(platform->gameName) + 16;
		char *iniName = (char *)malloc(iniNameLen);
		snprintf(iniName, iniNameLen, "%s.ini", platform->gameName);
		io.IniFilename = iniName;
	} else {
		io.IniFilename = NULL;
	}

	// gui->vao = 0;
	// glGenVertexArrays(1, &gui->vao);
	ImGui_ImplSDL2_InitForOpenGL(platform->sdlWindow, platform->sdlContext);
# ifdef GL_ES
	ImGui_ImplOpenGL3_Init("#version 300 es");
# else
	ImGui_ImplOpenGL3_Init("#version 330");
# endif

#endif
}

void guiStartFrame() {
	gui->needToDrawThisFrame = true;
#if defined(FALLOW_IMGUI)
	if (platform->disableGui) return;
	ImGui_ImplOpenGL3_NewFrame();

	// logf("Mouse: %.1f %.1f\n", platform->mouse.x, platform->mouse.y);
	ImGui_ImplSDL2_NewFrame(platform->sdlWindow);
	ImGuiIO &io = ImGui::GetIO();
	io.MousePos.x = platform->mouse.x;
	io.MousePos.y = platform->mouse.y;

	ImGui::NewFrame();
#endif
}

void guiDraw() {
	if (!gui->needToDrawThisFrame) return;
	gui->needToDrawThisFrame = false;
#if defined(FALLOW_IMGUI)
	if (platform->disableGui) return;
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#endif
}

void guiProcessSdlEvent(SDL_Event *e) {
# if defined(FALLOW_IMGUI)
	if (platform->disableGui) return;
	ImGui_ImplSDL2_ProcessEvent(e);
# endif
}

void guiUpdateEvents() {
#if defined(FALLOW_IMGUI)
	if (platform->disableGui) {
		platform->hoveringGui = false;
		platform->typingGui = false;
		return;
	}
	ImGuiIO &io = ImGui::GetIO();
	platform->hoveringGui = io.WantCaptureMouse;
	platform->typingGui = io.WantTextInput;
#endif
}

// bool guiInputRgb(const char *name, int *argb, bool showInputs) {
// 	int a;
// 	int intCol[3];
// 	hexToArgb(*argb, &a, &intCol[0], &intCol[1], &intCol[2]);

// 	float floatCol[3];
// 	floatCol[0] = intCol[0]/255.0;
// 	floatCol[1] = intCol[1]/255.0;
// 	floatCol[2] = intCol[2]/255.0;

// 	int flags = ImGuiColorEditFlags_DisplayHex;
// 	if (!showInputs) flags |= ImGuiColorEditFlags_NoInputs;
// 	bool ret = ImGui::ColorEdit3(name, floatCol, flags);

// 	intCol[0] = floatCol[0] * 255.0;
// 	intCol[1] = floatCol[1] * 255.0;
// 	intCol[2] = floatCol[2] * 255.0;
// 	*argb = argbToHex(255, intCol[0], intCol[1], intCol[2]);

// 	return ret;
// }

// bool guiInputArgb(const char *name, int *argb, bool showInputs) {
// 	int intCol[4];
// 	hexToArgb(*argb, &intCol[0], &intCol[1], &intCol[2], &intCol[3]);

// 	float floatCol[4];
// 	floatCol[0] = intCol[1]/255.0;
// 	floatCol[1] = intCol[2]/255.0;
// 	floatCol[2] = intCol[3]/255.0;
// 	floatCol[3] = intCol[0]/255.0;

// 	int flags = 0;
// 	if (showInputs) {
// 		flags |= ImGuiColorEditFlags_DisplayHex;
// 	} else {
// 		flags |= ImGuiColorEditFlags_NoInputs;
// 	}
// 	bool ret = ImGui::ColorEdit4(name, floatCol);

// 	intCol[0] = floatCol[3] * 255.0;
// 	intCol[1] = floatCol[0] * 255.0;
// 	intCol[2] = floatCol[1] * 255.0;
// 	intCol[3] = floatCol[2] * 255.0;
// 	*argb = argbToHex(intCol[0], intCol[1], intCol[2], intCol[3]);

// 	return ret;
// }

// void guiPushStyleColor(ImGuiCol style, int color) {
// 	Vec4 vecColor = hexToArgbFloat(color);
// 	ImGui::PushStyleColor(style, ImVec4(vecColor.y, vecColor.z, vecColor.w, vecColor.x));
// }

// void guiPopStyleColor(int amount) {
// 	ImGui::PopStyleColor(amount);
// }

void guiTexture(Texture *texture) {
	ImGui::Image((ImTextureID)(intptr_t)texture->id, ImVec2(texture->width, texture->height), ImVec2(0, 1), ImVec2(1, 0));
}

bool guiImageButton(Texture *texture) {
	return ImGui::ImageButton((ImTextureID)(intptr_t)texture->id, ImVec2(texture->width, texture->height), ImVec2(0, 1), ImVec2(1, 0));
}

#ifdef IMPLOT_VERSION 
void plotPushStyleColor(ImPlotCol style, int color);
void plotPushStyleColor(ImPlotCol style, int color) {
	Vec4 vecColor = hexToArgbFloat(color);
	ImPlot::PushStyleColor(style, ImVec4(vecColor.y, vecColor.z, vecColor.w, vecColor.x));
}

void plotPopStyleColor(int amount=1);
void plotPopStyleColor(int amount) {
	ImPlot::PopStyleColor(amount);
}
#endif
#endif // GUI_IMPL
