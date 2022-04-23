pushd .
cd "%~dp0.."

set BIN_DIR=C:\bin
set PROJECT_DIR=%~dp0..
set INCLUDE_DIR=%PROJECT_DIR%\include
set LIB_DIR=%PROJECT_DIR%\lib
set SRC_DIR=%PROJECT_DIR%\src

if not exist %BIN_DIR%\webgl mkdir %BIN_DIR%\webgl

REM copyPastedBuildEmscriptenAssets
set ASSETS_DIR=%PROJECT_DIR%\%GAME_NAME%Assets
if "%GAME_NAME%" == "rtsClient" set ASSETS_DIR="C:\Dropbox\rtsGame\rtsClientAssets"
if "%GAME_NAME%" == "horseGame" set ASSETS_DIR="C:\Dropbox\FallowCandy\HorseProjects\horseGameAssets"
if "%GAME_NAME%" == "butt2GoGame" set ASSETS_DIR="C:\Dropbox\FallowCandy\Butt2Go\butt2GoGameAssets"

if [%INTERNAL_MODE%]==[1] (
	set COMPRESSION_LEVEL=0
) else (
	set COMPRESSION_LEVEL=9
)
if exist "C:\Program Files\7-Zip\7z.exe" set SEVEN_ZIP="C:\Program Files\7-Zip\7z.exe"

if [%CODE_ONLY%]==[1] (
	echo Building code only
) else (
	robocopy commonAssets\assets\common %ASSETS_DIR%\assets\common /MIR /W:5 /njh /njs /ndl /nc /ns /XO /nfl

	robocopy %ASSETS_DIR%/assets %BIN_DIR%/assets /MIR /W:1 /njh /njs /nc /ns /ndl /nfl /XO /XF __* *.psd
	for /d /r %BIN_DIR%\assets\ %%f in (__*) do rmdir /s /q %%f

	rmdir /s /q %BIN_DIR%\assetPacks
	mkdir %BIN_DIR%\assetPacks

	mkdir %BIN_DIR%\assetPacks\preloader

	robocopy %BIN_DIR%\assets %BIN_DIR%\assetPacks\preloader\assets /MIR /W:1 /njh /njs /ndl /nc /ns

	mkdir %BIN_DIR%\assetPacks\main\assets

	REM Move stuff from assetPacks\preloader\assets to other packs here I think

	pushd .
	cd %BIN_DIR%\assetPacks
	for /d %%f in (%BIN_DIR%\assetPacks\*) do (
		cd %%f
		echo Gonna pack %%f
		%SEVEN_ZIP% a -tzip -mx=%COMPRESSION_LEVEL% %BIN_DIR%\assetPacks\%%~nxf.zip .
		cd ..
	)
	popd

	rmdir /s /q %BIN_DIR%\assetsEmbed
	mkdir %BIN_DIR%\assetsEmbed
	move %BIN_DIR%\assetPacks\preloader.zip %BIN_DIR%\assetsEmbed\preloader.zip

	if exist %BIN_DIR%\webgl move %BIN_DIR%\assetPacks\*.zip %BIN_DIR%\webgl
)
REM /copyPastedBuildEmscriptenAssets

set PATH=%PATH%;c:\emsdk
set PATH=%PATH%;c:\emsdk\upstream\emscripten
set PATH=%PATH%;c:\emsdk\node\12.18.1_64bit\bin
set PATH=%PATH%;c:\emsdk\python\3.7.4-pywin32_64bit
set PATH=%PATH%;c:\emsdk\java\8.152_64bit\bin

set EMSDK=c:/emsdk
set EM_CONFIG=c:\emsdk\.emscripten
set EMSDK_NODE=c:\emsdk\node\12.18.1_64bit\bin\node.exe
set EMSDK_PYTHON=c:\emsdk\python\3.7.4-pywin32_64bit\python.exe
set JAVA_HOME=c:\emsdk\java\8.152_64bit
set EM_CACHE=c:/emsdk/upstream/emscripten\cache

if [%INTERNAL_MODE%]==[1] (
	set INTERNAL_ARGS=-DFALLOW_INTERNAL -s GL_ASSERTIONS=1 -s ASSERTIONS=1
) else (
	set INTERNAL_ARGS=
)

if [%DEBUG_MODE%]==[1] (
	set DEBUG_ARGS=-DFALLOW_DEBUG -g
) else (
	set DEBUG_ARGS=
)

if [%OPTIMIZED_MODE%]==[1] (
	set OPT_ARGS=-O2
) else (
	set OPT_ARGS=
)

cd %BIN_DIR%
set _SRC_DIR=%SRC_DIR:\=/%
set _BIN_DIR=%BIN_DIR:\=/%
set _INCLUDE_DIR=%INCLUDE_DIR:\=/%
set _LIB_DIR=%LIB_DIR:\=/%

if not exist emscriptenObs\stb_image.o (
	echo Building stb libs...
	if not exist emscriptenObs mkdir emscriptenObs
	cd emscriptenObs
	set STB_PRE_ARGS=-std=c++11 -O2 -fno-rtti -fno-exceptions -c -I%_INCLUDE_DIR%/all -I%_INCLUDE_DIR%/emscripten -Wno-c++11-compat-deprecated-writable-strings -Wno-writable-strings

	call em++ %STB_PRE_ARGS% -D STB_IMAGE_IMPLEMENTATION %_INCLUDE_DIR%\emscripten\stb_image.c -c -o stb_image.o
	call em++ %STB_PRE_ARGS% -D STB_SPRINTF_NOUNALIGNED -D STB_SPRINTF_IMPLEMENTATION %_INCLUDE_DIR%\emscripten\stb_sprintf.c -c -o stb_sprintf.o
	call em++ %STB_PRE_ARGS% -D STB_IMAGE_WRITE_IMPLEMENTATION %_INCLUDE_DIR%\emscripten\stb_image_write.c -c -o stb_image_write.o
	call em++ %STB_PRE_ARGS% -D STB_TRUETYPE_IMPLEMENTATION %_INCLUDE_DIR%\emscripten\imstb_truetype.c -c -o imstb_truetype.o
	call em++ %STB_PRE_ARGS% -D STB_RECT_PACK_IMPLEMENTATION %_INCLUDE_DIR%\emscripten\imstb_rectpack.c -c -o imstb_rectpack.o

	cd ..
)

echo Building...
call em++ %DEBUG_ARGS% %INTERNAL_ARGS% %OPT_ARGS% %_LIB_DIR%\emscripten\libskia.a %_SRC_DIR%/main.cpp -o %_BIN_DIR%/webgl/index.html -I%_INCLUDE_DIR%/all -I%_INCLUDE_DIR%/emscripten ^
	-DPLAYING_%GAME_NAME% -DPROJECT_ASSET_DIR="." ^
	-fno-rtti -fno-exceptions ^
	%_BIN_DIR%\emscriptenObs\stb_image.o ^
	%_BIN_DIR%\emscriptenObs\stb_sprintf.o ^
	%_BIN_DIR%\emscriptenObs\stb_image_write.o ^
	%_BIN_DIR%\emscriptenObs\imstb_truetype.o ^
	%_BIN_DIR%\emscriptenObs\imstb_rectpack.o ^
	-D STB_SPRINTF_OBJ -D STB_IMAGE_WRITE_OBJ -D STB_IMAGE_OBJ -D STB_TRUETYPE_OBJ -D STB_RECTPACK_OBJ ^
	-Wno-c++11-compat-deprecated-writable-strings -Wno-writable-strings ^
	-s MIN_WEBGL_VERSION=2 -s MAX_WEBGL_VERSION=2 -s FULL_ES2=1 -s FULL_ES3=1 -s USE_SDL=2 -s ALLOW_MEMORY_GROWTH=1 --no-heap-copy ^
	-s DISABLE_DEPRECATED_FIND_EVENT_TARGET_BEHAVIOR=1 ^
	-lidbfs.js ^
	-s ERROR_ON_UNDEFINED_SYMBOLS=0 -s WASM_MEM_MAX=2048MB -lwebsocket.js ^
	--preload-file assetsEmbed@assets

REM -s USE_PTHREADS=1 ^
popd

if "%GAME_NAME%" == "rtsClient" copy C:\Dropbox\rtsGame\rtsClientAssets\assets\__raw\webgl\index.html C:\bin\webgl\index.html
if "%GAME_NAME%" == "horseGame" copy C:\Dropbox\FallowCandy\HorseProjects\raw\index.html C:\bin\webgl\index.html
if "%GAME_NAME%" == "interrogationGame" copy C:\Dropbox\CatFallowWing\interrogationGame\assets\__raw\index.html C:\bin\webgl\index.html

xcopy %LIB_DIR%\emscripten\*.js C:\bin\webgl\ /sy
xcopy %LIB_DIR%\emscripten\*.wasm C:\bin\webgl\ /sy
robocopy C:\bin\webgl C:\xampp\htdocs\game /MIR /W:5 /njh /njs /ndl /nc /ns /XO /nfl
