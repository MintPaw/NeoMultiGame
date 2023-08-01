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

REM set PATH=%PATH%;c:\emsdk
REM set PATH=%PATH%;c:\emsdk\upstream\emscripten

REM set EMSDK=c:/emsdk
REM set EM_CONFIG=c:/emsdk/.emscripten
REM set EMSDK_NODE=C:/emsdk/node/16.20.0_64bit/bin/node.exe
REM set EMSDK_PYTHON=C:/emsdk/python/3.9.2-nuget_64bit/python.exe
REM set JAVA_HOME=C:/emsdk/java/8.152_64bit
REM set EM_CACHE=c:/emsdk/upstream/emscripten/cache

cd /d "%~dp0.."

set BIN_DIR=C:\bin
set PROJECT_DIR=%~dp0..
set INCLUDE_DIR=%PROJECT_DIR%\include\raylib
set LIB_DIR=%PROJECT_DIR%\lib\raylib\emscripten
set SRC_DIR=%PROJECT_DIR%\src

if not exist C:\bin\webgl mkdir C:\bin\webgl
REM copyPastedBuildEmscriptenAssets
set ASSETS_DIR=%PROJECT_DIR%\%GAME_NAME%Assets
if "%GAME_NAME%" == "rtsClient" set ASSETS_DIR="C:\Dropbox\rtsGame\rtsClientAssets"
if "%GAME_NAME%" == "horseGame" set ASSETS_DIR="C:\Dropbox\FallowCandy\HorseProjects\horseGameAssets"
if "%GAME_NAME%" == "butt2GoGame" set ASSETS_DIR="C:\Dropbox\FallowCandy\Butt2Go\butt2GoGameAssets"
if "%GAME_NAME%" == "concreteJungleGame" set ASSETS_DIR="C:\Dropbox\concreteJungle\concreteJungleGameAssets"
if "%GAME_NAME%" == "gladiators2Game" set ASSETS_DIR="C:\Dropbox\MultiGame\multiGamePrivate\gladiators2GameAssets"

if [%INTERNAL_MODE%]==[1] (
	set COMPRESSION_LEVEL=0
) else (
	set COMPRESSION_LEVEL=9
)
if exist "C:\Program Files\7-Zip\7z.exe" set SEVEN_ZIP="C:\Program Files\7-Zip\7z.exe"

del /s /q C:\bin\webgl\index.html

if [%CODE_ONLY%]==[1] (
	echo Building code only
) else (
	robocopy commonAssets\assets\common %ASSETS_DIR%\assets\common /MIR /W:5 /njh /njs /ndl /nc /ns /XO /nfl

	robocopy %ASSETS_DIR%/assets %BIN_DIR%/assets /MIR /W:1 /njh /njs /nc /ns /ndl /nfl /XO /XF __* *.psd
	for /d /r %BIN_DIR%\assets\ %%f in (__*) do rmdir /s /q %%f

	rmdir /s /q %BIN_DIR%\assetPacks
	mkdir %BIN_DIR%\assetPacks

	mkdir %BIN_DIR%\assetPacks\preloader

	robocopy %BIN_DIR%\assets %BIN_DIR%\assetPacks\preloader\assets /MIR /W:1 /njh /njs /ndl /nc /ns /nfl

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

cd /d C:\bin\webgl

@echo on

if [%INTERNAL_MODE%]==[1] (
	set INTERNAL_ARGS=-DFALLOW_INTERNAL -s GL_ASSERTIONS=1 -s ASSERTIONS=1
) else (
	set INTERNAL_ARGS=
)

if [%DEBUG_MODE%]==[1] (
	set DEBUG_ARGS=-DFALLOW_DEBUG -g
	REM set DEBUG_ARGS=-DFALLOW_DEBUG -gsource-map --source-map-base http://localhost:80/game/
) else (
	set DEBUG_ARGS=
)

if [%OPTIMIZED_MODE%]==[1] (
	set OPT_ARGS=-O2
) else (
	set OPT_ARGS=
)

call em++ -std=c++17 %DEBUG_ARGS% %INTERNAL_ARGS% %OPT_ARGS% -o index.html %SRC_DIR%\main.cpp ^
	%LIB_DIR%\libraylib.a %LIB_DIR%\libskia.a %LIB_DIR%\libbox2d.a -I%INCLUDE_DIR% -I%INCLUDE_DIR%\skia -L%LIB_DIR% ^
	-lidbfs.js ^
	-fno-rtti -fno-exceptions -Wno-c++11-compat-deprecated-writable-strings -Wno-writable-strings ^
	-DPLAYING_%GAME_NAME% -DPROJECT_ASSET_DIR="." -DRAYLIB_MODE=1 -DPLATFORM_WEB ^
	-s USE_GLFW=3 ^
	-s ALLOW_MEMORY_GROWTH=1 ^
	-s LLD_REPORT_UNDEFINED=1 ^
	-s ERROR_ON_UNDEFINED_SYMBOLS=0 ^
	-s INITIAL_MEMORY=64MB ^
	--preload-file ..\assetsEmbed@assets

xcopy %LIB_DIR%\*.js . /sy
xcopy %LIB_DIR%\*.wasm . /sy
robocopy C:\bin\webgl C:\xampp\htdocs\game /MIR /W:5 /njh /njs /ndl /nc /ns /XO /nfl
exit /b 0
