#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <cmath>
#include <stdio.h>
#include <time.h>
#define USES_THREADS 0

#define STRINGIFYX(val) #val
#define STRINGIFY(val) STRINGIFYX(val)

#define SK_GL
#include "include/core/SkCanvas.h"
#include "include/core/SkFont.h"
#include "include/core/SkTextBlob.h"
#include "include/core/SkGraphics.h"
#include "include/core/SkSurface.h"
#include "include/core/SkData.h"
#include "include/core/SkImage.h"
#include "include/core/SkStream.h"
#include "include/core/SkEncodedImageFormat.h"
#include "include/core/SkPicture.h"
#include "include/core/SkPictureRecorder.h"
#include "include/core/SkPixelRef.h"
#include "include/effects/SkGradientShader.h"
#include "include/effects/SkImageFilters.h"
#include "include/gpu/GrDirectContext.h"
#include "include/gpu/gl/GrGLInterface.h"
// #include "include/gpu/gl/GrGLDefines.h"


#ifdef RAYLIB_MODE

namespace Raylib {
#include "raylib.h"
#include "rlgl.h"
#include "raymath.h"

#define RLIGHTS_IMPLEMENTATION
#include "rlights.h"
};

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
// #include "curve-editor-lumix.hpp"
#include "bezier.cpp"

#ifndef STB_SPRINTF_OBJ
# define STB_SPRINTF_IMPLEMENTATION
#endif
#define STB_SPRINTF_NOUNALIGNED
#include "stb_sprintf.h"

#include "stb_image.h"

#include "stb_image_write.h"

#include "miniz.c"
#undef inflate
int inflate(mz_streamp pStream, int flush) { return mz_inflate(pStream, flush); }

#define IncMutex(mutex) //@todo
#define DecMutex(mutex) //@todo
#define FORCE_INLINE //@todo

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tchar.h>
#include <sys/stat.h> // Missing from the non-raylib version for some reason???
#include <io.h>
#elif defined(__EMSCRIPTEN__)
# include <emscripten.h>
# include <emscripten/html5.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <dirent.h>
# include <semaphore.h>
#endif

#define STB_VORBIS_HEADER_ONLY // This matters later for audio.cpp

//@hack
void logf(const char *msg, ...);
void loggerAssert(bool expr, const char *fileName, int lineNum);
void loggerPanic(const char *msg, const char *fileName, int lineNum);
#define Assert(expr) loggerAssert(expr, __FILE__, __LINE__)
#define Panic(msg) loggerPanic(msg, __FILE__, __LINE__)
#define PATH_MAX_LEN 256
char projectAssetDir[PATH_MAX_LEN] = {};
char filePathPrefix[PATH_MAX_LEN] = {};
char exeDir[PATH_MAX_LEN] = {};

//@hack new raylib only hack
void logLastOSErrorCode(const char *fileName, int lineNum);
#define logLastOSError() logLastOSErrorCode(__FILE__, __LINE__)

#include "memoryTools.cpp"
#include "mathTools.cpp"
#include "rnd.cpp"
#include "ds.cpp"

#include "logging.cpp"
#include "file.cpp"
#include "dataStream.cpp"
#include "platformRay.cpp"
#include "zip.cpp"
#include "textureSystem.cpp"
#include "audio.cpp"
#include "font.cpp"

#include "threads.cpp"
#include "stringSave.cpp"
#include "reflectionTool.cpp"
#include "animation.cpp"

#if (!FALLOW_COMMAND_LINE_ONLY && !__LINUX__)
#include "swf.cpp"
#include "skia.cpp"
#endif

#else

#include <signal.h>
#include <functional>
#include <memory>

#if defined(_WIN32) // <-----

#define WriteFence() _WriteBarrier(); _mm_sfence()
#define ReadFence() _ReadBarrier(); 
#define IncMutex(mutex) \
	do { \
		for (;;) if (InterlockedCompareExchange(mutex, 1, 0) == 0) break; \
	} while (0)

#define DecMutex(mutex) \
	do { \
		for (;;) if (InterlockedCompareExchange(mutex, 0, 1) == 1) break; \
	} while (0)

#elif defined(__EMSCRIPTEN__) // <-----

#include <wasm_simd128.h>
#define WriteFence() __sync_synchronize()
#define ReadFence() __sync_synchronize()
#define IncMutex(mutex) \
	do { \
		for (;;) if (__sync_val_compare_and_swap(mutex, 0, 1) == 0) break; \
	} while (0)

#define DecMutex(mutex) \
	do { \
		for (;;) if (__sync_val_compare_and_swap(mutex, 1, 0) == 1) break; \
	} while (0)

#elif defined(__linux__) // <-----
#define IncMutex(mutex) \
	do { \
		for (;;) if (__sync_val_compare_and_swap(mutex, 0, 1) == 0) break; \
	} while (0)

#define DecMutex(mutex) \
	do { \
		for (;;) if (__sync_val_compare_and_swap(mutex, 1, 0) == 1) break; \
	} while (0)

#define WriteFence() _mm_mfence()
#define ReadFence() _mm_lfence()

#endif // <-----

#ifdef _WIN32
# define FORCE_INLINE __forceinline
# define FORCE_NO_INLINE __declspec(noinline)
# define WIN32_LEAN_AND_MEAN

# include <windows.h>
# include <timeapi.h>
# include <wchar.h>
# include <tchar.h>
# include <io.h>

#endif

#ifdef __linux__
# define FORCE_INLINE __attribute__((always_inline)) 
# define FORCE_NO_INLINE 
# include <dirent.h>
# define GL_ES
# include <semaphore.h>
# include <pthread.h>

// #undef register
// #define register 
// # include "mongoose.c"
// #undef register
// #define register register
#endif

#ifdef __EMSCRIPTEN__
# define FORCE_INLINE __attribute__((always_inline)) 
# include <emscripten.h>
# include <emscripten/html5.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <dirent.h>
# include <semaphore.h>

# ifdef __EMSCRIPTEN_PTHREADS__ 
#  include <pthread.h>
# endif

# define GL_ES
#endif

#include <GL/glew.h>
#define IMGUI_IMPL_OPENGL_LOADER_GLEW
#define FALLOW_IMGUI

#ifndef STB_IMAGE_WRITE_OBJ
# define STB_IMAGE_WRITE_IMPLEMENTATION
#endif
#include "stb_image_write.h"

#ifndef STB_SPRINTF_OBJ
# define STB_SPRINTF_IMPLEMENTATION
#endif
#define STB_SPRINTF_NOUNALIGNED
#include "stb_sprintf.h"

#ifndef STB_IMAGE_OBJ
# define STB_IMAGE_IMPLEMENTATION
#endif
#include "stb_image.h"

#if !defined(FALLOW_IMGUI) //@incomplete fix this by building all the stb libs yourself
#ifndef STB_TRUETYPE_OBJ
# define STB_TRUETYPE_IMPLEMENTATION
#endif
#include "imstb_truetype.h"

#ifndef STB_RECTPACK_OBJ
# define STB_RECTPACK_IMPLEMENTATION
#endif
#include "imstb_rectpack.h"
#endif

#include "miniz.c"
#undef inflate
int inflate(mz_streamp pStream, int flush) { return mz_inflate(pStream, flush); }

#include "cJSON.c"

struct LogfBuffer;
void logf(const char *msg, ...); //@hack Because everything needs logf, actually in the logging code
void infof(const char *msg, ...); //@hack Because everything needs info, actually in the logging code

///
/// //@hack This is here because stuff like memoryTools need it
///
void loggerAssert(bool expr, const char *fileName, int lineNum);
void loggerPanic(const char *msg, const char *fileName, int lineNum);
#define Assert(expr) loggerAssert(expr, __FILE__, __LINE__)
#define Panic(msg) loggerPanic(msg, __FILE__, __LINE__)
#define PATH_MAX_LEN 256
char projectAssetDir[PATH_MAX_LEN] = {};
char filePathPrefix[PATH_MAX_LEN] = {};
char exeDir[PATH_MAX_LEN] = {};

#include "memoryTools.cpp"
#include "mathTools.cpp"
#include "rnd.cpp"
#include "perlin.cpp"
#include "ds.cpp"
#include "compression.cpp"
#include "logging.cpp"

#include "platform.cpp"
#include "threads.cpp"

#include "file.cpp"
#include "dataStream.cpp"
#include "zip.cpp"
#include "platformUtils.cpp"
#include "renderer.cpp"
#include "textureSystem.cpp"
#include "audio.cpp"
#include "stringSave.cpp"
#include "reflectionTool.cpp"
#include "skeleton.cpp"
#include "font.cpp"
#include "draw3d.cpp"
#include "models.cpp"
#include "tilemapGenerator.cpp"
#include "waveGenerator.cpp"

#include "animation.cpp"
#include "spine.cpp"
#include "dragonBones.cpp"
#include "networking.cpp"

#define GUI_IMPL
#include "gui.cpp"

#if (!FALLOW_COMMAND_LINE_ONLY && !__LINUX__)
#include "swf.cpp"
#include "skia.cpp"
#endif

#endif // RAYLIB_MODE

NanoTime mainNano;

#if defined(PLAYING_fpsGame)
# include "fpsGame.cpp"
#endif

#if defined(PLAYING_drawTestGame)
# include "drawTestGame.cpp"
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

#if defined(PLAYING_turnBasedGame)
# include "turnBasedGame.cpp"
#endif

#if defined(PLAYING_parametersGame)
# include "parametersGame.cpp"
#endif

#if defined(PLAYING_petGame)
# include "petGame.cpp"
#endif

#if defined(PLAYING_stockGame)
# include "stockGame.cpp"
#endif

#if defined(PLAYING_towerGame)
# include "towerGame.cpp"
#endif

#if defined(PLAYING_dynaGame)
# include "dynaGame/dynaGame.cpp"
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

#if defined(PLAYING_chessGame)
# include "chessGame.cpp"
#endif

#if defined(PLAYING_zooBoundGame)
# include "zooBoundGame.cpp"
#endif

#if defined(PLAYING_tdSlasherGame)
# include "tdSlasherGame.cpp"
#endif

#if defined(PLAYING_gladiators2Game)
# include "../../multiGamePrivate/src/gladiators2Game.cpp"
#endif

#if defined(PLAYING_concreteJungleGame)
# include "concreteJungleGame.cpp"
#endif

#if defined(PLAYING_animationToolsGame)
# include "../../multiGamePrivate/src/animationToolsGame.cpp"
#endif

#if defined(PLAYING_rayGame)
# include "rayGame.cpp"
#endif

#if defined(PLAYING_rollerGame)
# include "rollerGame.cpp"
#endif

#if defined(PLAYING_butt2GoGame)
# include "../../multiGamePrivate/src/butt2GoGame.cpp"
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
