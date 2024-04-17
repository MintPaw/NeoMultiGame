cd /d "%~dp0.."

set BIN_DIR=C:\bin
set PROJECT_DIR=%~dp0..
set INCLUDE_DIR=%PROJECT_DIR%\include\raylib
set LIB_DIR=%PROJECT_DIR%\lib\raylib\emscripten
set SRC_DIR=%PROJECT_DIR%\src

if not exist C:\bin\webgl mkdir C:\bin\webgl

call buildSystem/prepareAssets.bat

set COMPRESSION_LEVEL=9
if [%INTERNAL_MODE%]==[1] set COMPRESSION_LEVEL=0

if exist "C:\Program Files\7-Zip\7z.exe" set SEVEN_ZIP="C:\Program Files\7-Zip\7z.exe"

del /s /q C:\bin\webgl\index.html

if [%CODE_ONLY%]==[1] (
	echo Building code only
) else (
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

cd /d C:\bin\webgl

@echo on

if [%INTERNAL_MODE%]==[1] (
	REM set INTERNAL_ARGS=-DFALLOW_INTERNAL -s GL_ASSERTIONS=1 -s ASSERTIONS=1 -s SAFE_HEAP=1 -s STACK_OVERFLOW_CHECK=2 -s DEMANGLE_SUPPORT=1 -s VERBOSE=1
	set INTERNAL_ARGS=-DFALLOW_INTERNAL -s GL_ASSERTIONS=1 -s ASSERTIONS=1 -s DEMANGLE_SUPPORT=1
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

call em++ -std=c++17 %DEBUG_ARGS% %INTERNAL_ARGS% %OPT_ARGS% -o index.html ^
	%SRC_DIR%\main.cpp %LIB_DIR%\libskia.a %LIB_DIR%\libbox2d.a ^
	-I%INCLUDE_DIR% -I%INCLUDE_DIR%\skia -L%LIB_DIR% ^
	-lidbfs.js ^
	-fno-rtti -fno-exceptions -Wno-c++11-compat-deprecated-writable-strings -Wno-writable-strings ^
	-DPLAYING_%GAME_NAME% ^
	-s MAX_WEBGL_VERSION=2 ^
	-s ALLOW_MEMORY_GROWTH=1 ^
	-s LLD_REPORT_UNDEFINED=1 ^
	-s ERROR_ON_UNDEFINED_SYMBOLS=0 ^
	-s INITIAL_MEMORY=64MB ^
	-s STACK_SIZE=10MB ^
	--preload-file ..\assetsEmbed@assets

xcopy %LIB_DIR%\*.js . /sy
xcopy %LIB_DIR%\*.wasm . /sy
copy %PROJECT_DIR%\buildSystem\debugIndex.html index.html
robocopy C:\bin\webgl C:\xampp\htdocs\game /MIR /W:5 /njh /njs /ndl /nc /ns /XO /nfl
exit /b 0
