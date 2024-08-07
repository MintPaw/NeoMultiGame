#pragma warning(error: 4553)

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <cmath>
#include <stdio.h>
#include <time.h>
#define USES_THREADS 1

#define STRINGIFYX(val) #val
#define STRINGIFY(val) STRINGIFYX(val)

#if (FALLOW_COMMAND_LINE_ONLY || __LINUX__)
# ifndef NO_SKIA
#  define NO_SKIA
# endif
#endif

#ifndef NO_SKIA
#define SK_GL
#define SK_GANESH
#define SK_EMBEDDED_FONTS
#include "include/core/SkFont.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkBitmap.h"
#include "include/core/SkTextBlob.h"
#include "include/core/SkGraphics.h"
#include "include/core/SkSurface.h"
#include "include/gpu/ganesh/SkSurfaceGanesh.h"
#include "include/gpu/ganesh/gl/GrGLBackendSurface.h"
#include "include/gpu/GrBackendSurface.h"
#include "include/core/SkData.h"
#include "include/core/SkImage.h"
#include "include/core/SkStream.h"
// #include "include/core/SkEncodedImageFormat.h"
#include "include/core/SkPicture.h"
#include "include/core/SkPictureRecorder.h"
#include "include/core/SkPixelRef.h"
#include "include/core/SkPixelRef.h"
#include "include/core/SkColorFilter.h"
#include "include/core/SkColorSpace.h"
#include "include/effects/SkGradientShader.h"
#include "include/effects/SkImageFilters.h"
#include "include/gpu/GrDirectContext.h"
#include "include/gpu/gl/GrGLInterface.h"
#include "include/pathops/SkPathOps.h"

// #include "include/gpu/GrBackendSurface.h"
// #include "include/gpu/ganesh/SkSurfaceGanesh.h"
#endif

#ifdef RAYLIB_MODE
namespace Raylib {
#include "raylib.h"
#include "rlgl.h"
#include "raymath.h"
};
#endif

#undef RED
#undef ORANGE
#undef YELLOW
#undef LIME
#undef GREEN
#undef BLUE
#undef PURPLE
#undef VIOLET
#undef MAGENTA

#include "imconfig.h"
#ifdef IMGUI_OBJ
#include "imgui.h"
#include "imstb_rectpack.h"
#include "imstb_truetype.h"
#else
#include "imgui.cpp"
#include "imgui_widgets.cpp"
#include "imgui_draw.cpp"
#include "imgui_tables.cpp"
#include "imgui_demo.cpp"
#endif

#include "imguiNodes/crude_json.cpp"
#include "imguiNodes/imgui_bezier_math.inl"
#include "imguiNodes/imgui_canvas.cpp"
#include "imguiNodes/imgui_extra_math.inl"
#include "imguiNodes/imgui_node_editor.cpp"
#include "imguiNodes/imgui_node_editor_api.cpp"
#include "imguiNodes/imgui_node_editor_internal.inl"
namespace ImNode = ax::NodeEditor;

#include "ImGradientHDR.cpp"
#include "imgui_curve.hpp"

#ifndef STB_SPRINTF_OBJ
# define STB_SPRINTF_IMPLEMENTATION
#endif
#define STB_SPRINTF_NOUNALIGNED
#include "stb_sprintf.h"

#ifndef RAYLIB_MODE
#define STB_IMAGE_IMPLEMENTATION
#endif
#include "stb_image.h"

#include "stb_image_write.h"

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tchar.h>
#include <sys/stat.h> // Missing from the non-raylib version for some reason???
#include <io.h>
#include <psapi.h>
#include <direct.h>
#define timegm _mkgmtime
#elif defined(__EMSCRIPTEN__)
# include <emscripten.h>
# include <emscripten/html5.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <dirent.h>
# include <semaphore.h>
#endif

#include "miniz.h"
#undef inflate
int inflate(mz_streamp pStream, int flush) { return mz_inflate(pStream, flush); }

#include "zip.c"

#define IncMutex(mutex) //@todo
#define DecMutex(mutex) //@todo

#ifdef _WIN32
# define FORCE_INLINE __forceinline
#else 
# define FORCE_INLINE __attribute__((always_inline)) 
#endif


#define LOGGING_HEADER
#include "logging.cpp"

#define PATH_MAX_LEN 256
char projectAssetDir[PATH_MAX_LEN] = {};
char filePathPrefix[PATH_MAX_LEN] = {};
char exeDir[PATH_MAX_LEN] = {};

//@hack
void logLastOSError();

#include "memoryTools.cpp"
#include "mathTools.cpp"

#define DATA_STREAM_HEADER
#include "dataStream.cpp"

#include "rnd.cpp"
#include "perlin.cpp"
#include "ds.cpp"
#include "compression.cpp"
#include "memoryTracking.cpp"
#include "file.cpp"
#include "dataStream.cpp"

#ifdef __EMSCRIPTEN__
	#include "platformBackendHtml.cpp"
#else
	#ifdef GLFW_MODE
		#include "platformBackendGlfw.cpp"
	#else
		#include "platformBackendRay.cpp"
	#endif
#endif

#include "platformFrontend.cpp"

#ifdef __EMSCRIPTEN__
#include "rendererBackendOpengl.cpp"
#else
	#ifdef GLFW_MODE
		#include "rendererBackendOpengl.cpp"
	#else
		#include "rendererBackendRay.cpp"
	#endif
#endif

#include "rendererFrontend.cpp"

#include "logging.cpp"
#include "platformUtils.cpp"
#include "zip.cpp"
#include "audio.cpp"
#include "skeleton.cpp"
// #include "mesh.cpp"
// #include "models.cpp"
#include "font.cpp"

#include "threads.cpp"
#include "stringSave.cpp"
#include "reflectionTool.cpp"
#include "animation.cpp"
#include "spine.cpp"

#include "saveLoadVersioning.cpp"
#include "ngui.cpp"

#ifdef SK_GL
# include "swf.cpp"
# include "skia.cpp"
#endif
#include "spriteSheets.cpp"

// #if defined(PLAYING_boxingGame)
// #include "oldEmitter.cpp"
// #else
#include "emitter.cpp"
// #endif

#include "equations.cpp"

NanoTime mainNano;

#if defined(PLAYING_fpsGame)
# include "fpsGame.cpp"
#endif

#if defined(PLAYING_horseGame)
# include "../../multiGamePrivate/src/horseGame.cpp"
#endif

#if defined(PLAYING_bulletHellGame)
# include "bulletHellGame/bulletHellGame.cpp"
#endif

#if defined(PLAYING_bulletHellGameServer)
# include "bulletHellGame/bulletHellGameServer.cpp"
#endif

#if defined(PLAYING_parametersGame)
# include "parametersGame.cpp"
#endif

#if defined(PLAYING_petGame)
# include "petGame.cpp"
#endif

#if defined(PLAYING_towerGame)
# include "towerGame.cpp"
#endif

#if defined(PLAYING_dynaGame)
# include STRINGIFY(ALT_CORE_PATH)
#endif

#if defined(PLAYING_rtsClient2)
# include "rtsClient2.cpp"
#endif

#if defined(PLAYING_testGame)
# include "testGame.cpp"
#endif

#if defined(PLAYING_catsFirstGame)
# include "../../multiGamePrivate/src/catsFirstGame.cpp"
#endif

#if defined(PLAYING_interrogationGame)
# include "../../multiGamePrivate/src/interrogationGame.cpp"
#endif

#if defined(PLAYING_zooBoundGame)
# include "zooBoundGame.cpp"
#endif

#if defined(PLAYING_gladiators2Game)
# include "../../multiGamePrivate/src/gladiators2Game.cpp"
#endif

#if defined(PLAYING_concreteJungleGame)
# include "concreteJungleGame.cpp"
#endif

#if defined(PLAYING_rollerGame)
# include "rollerGame.cpp"
#endif

#if defined(PLAYING_butt2GoGame)
# include "../../multiGamePrivate/src/butt2GoGame.cpp"
#endif

#if defined(PLAYING_pkGame)
# include "../../multiGamePrivate/src/pkGame.cpp"
#endif

#if defined(PLAYING_cacheViz)
# include STRINGIFY(ALT_CORE_PATH)
#endif

#if defined(PLAYING_tower2Game)
# include "tower2Game/tower2Game.cpp"
#endif

#if defined(PLAYING_deskGame)
# include "../../multiGamePrivate/src/deskGame.cpp"
#endif

#if defined(PLAYING_remoteGame)
# include "../../multiGamePrivate/src/remoteGame.cpp"
#endif

#if defined(PLAYING_turnBasedGame)
# include "turnBasedGame.cpp"
#endif

#if defined(PLAYING_catCardGame)
# include "../../multiGamePrivate/src/catCardGame.cpp"
#endif

#if defined(PLAYING_destinyGame)
# include "../../multiGamePrivate/src/destinyGame.cpp"
#endif

#if defined(PLAYING_swfTestGame)
# include "swfTestGame.cpp"
#endif

#if defined(PLAYING_boxingGame)
# include "../../multiGamePrivate/src/boxingGame.cpp"
#endif

#if defined(PLAYING_catAnimGame)
# include "../../multiGamePrivate/src/catAnimGame.cpp"
#endif

#if defined(PLAYING_neoTestGame)
# include "neoTestGame.cpp"
#endif

#if defined(PLAYING_ALT_CORE)
# include STRINGIFY(ALT_CORE_PATH)
#endif

#ifdef _WIN32
INT WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow)
#else
int main(int argc, char **argv)
#endif
{
	mainNano = getNanoTime();

#ifdef _WIN32
	{
		HMODULE hModule = GetModuleHandleW(NULL);
		GetModuleFileNameA(hModule, exeDir, PATH_MAX_LEN);

		char *lastSlash = strrchr(exeDir, '\\');
		if (!lastSlash) Panic("No last slash found in exe path");
		*lastSlash = 0;
	}
#endif

	runGame();
}
