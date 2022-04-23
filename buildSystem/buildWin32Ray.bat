if exist "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\Common7\Tools\VsDevCmd.bat" (
	set VS_CMD_PATH="C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\Common7\Tools\VsDevCmd.bat"
)

if exist "C:\Program Files (x86)\Microsoft Visual Studio 14.0\Common7\Tools\VsDevCmd.bat" (
	set VS_CMD_PATH="C:\Program Files (x86)\Microsoft Visual Studio 14.0\Common7\Tools\VsDevCmd.bat"
)

if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\Common7\Tools\VsDevCmd.bat" (
	set VS_CMD_PATH="C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\Common7\Tools\VsDevCmd.bat"
)

if exist "C:\Users\AshyPie\Desktop\AshyPie\Tools\VS2019\IDE\Common7\Tools\VsDevCmd.bat" (
	set VS_CMD_PATH="C:\Users\AshyPie\Desktop\AshyPie\Tools\VS2019\IDE\Common7\Tools\VsDevCmd.bat"
)

set BIN_DIR=C:\bin
if not exist %BIN_DIR% mkdir %BIN_DIR%

where cl
if %errorlevel% neq 0 (
	call %VS_CMD_PATH% -arch=amd64 -host_arch=amd64 -vcvars_ver=14.2
)
cls

pushd .
cd "%~dp0.."

set BIN_DIR=C:\bin
set PROJECT_DIR="%~dp0.."
set INCLUDE_DIR=%PROJECT_DIR%\include
set LIB_DIR=%PROJECT_DIR%\lib
set SRC_DIR=%PROJECT_DIR%\src
set ASSETS_DIR=%PROJECT_DIR%\%GAME_NAME%Assets

robocopy commonAssets\assets\common %ASSETS_DIR%\assets\common /MIR /W:5 /njh /njs /ndl /nc /ns /XO

set DEFINES=/D PLAYING_%GAME_NAME% /D PROJECT_DIR=%PROJECT_DIR:\=/%
if [%DEBUG_MODE%]==[1] (
	set DEBUG_ARGS=-MT -Zi
	set DEFINES=%DEFINES% /D FALLOW_DEBUG
) else (
	set DEBUG_ARGS=-MT

	robocopy %ASSETS_DIR%/assets %BIN_DIR%/assets /MIR /W:1 /njh /njs /ndl /nc /ns /XO /XF __* *.psd
	for /d /r %BIN_DIR%\assets\ %%f in (__*) do rmdir /s /q %%f
)

if [%CLANG_MODE%]==[1] (
	set OPT_ARGS=-Od -RTCsu
) else (
	set OPT_ARGS=-Od
)

if [%OPTIMIZED_MODE%]==[1] (
	set OPT_ARGS=-O2
)

if [%INTERNAL_MODE%]==[1] (
	set DEFINES=%DEFINES% /D FALLOW_INTERNAL /D PROJECT_ASSET_DIR=%ASSETS_DIR:\=/%
	set LIBS=winmm.lib opengl32.lib user32.lib gdi32.lib shell32.lib dbghelp.lib
) else (
	set DEFINES=%DEFINES% /D PROJECT_ASSET_DIR="."
	set LIBS=winmm.lib opengl32.lib user32.lib gdi32.lib shell32.lib
)

@echo on
if [%CLANG_MODE%]==[1] (
	set COMPILER=clang-cl -fsanitize=address -Wno-writable-strings -Wno-deprecated-declarations -Wno-pragma-pack -Wno-format-security -Wno-pragma-pack -Wno-deprecated-declarations -D__BOOL_DEFINED 

) else (
	if [%FAST_CLANG_MODE%]==[1] (
		set COMPILER=clang-cl -Wno-writable-strings -Wno-deprecated-declarations
	) else (
		set COMPILER=cl
	)
)

cd /d %BIN_DIR%
if not exist winRayObj\stb_sprintf.obj (
	echo Building stb libs
	if not exist winRayObj mkdir winRayObj
	cd winRayObj

	%COMPILER% -MT -O2 -EHsc -nologo -c -I%INCLUDE_DIR%\raylib -D STB_SPRINTF_NOUNALIGNED -D STB_SPRINTF_IMPLEMENTATION /Tp %INCLUDE_DIR%\raylib\stb_sprintf.h | ^
%COMPILER% -MT -O2 -EHsc -nologo -c -I%INCLUDE_DIR%\raylib -D STB_TRUETYPE_IMPLEMENTATION /Tp %INCLUDE_DIR%\raylib\imstb_truetype.h | ^
%COMPILER% -MT -O2 -EHsc -nologo -c -I%INCLUDE_DIR%\raylib %INCLUDE_DIR%\raylib\imgui.cpp | ^
%COMPILER% -MT -O2 -EHsc -nologo -c -I%INCLUDE_DIR%\raylib %INCLUDE_DIR%\raylib\imgui_tables.cpp | ^
%COMPILER% -MT -O2 -EHsc -nologo -c -I%INCLUDE_DIR%\raylib %INCLUDE_DIR%\raylib\imgui_widgets.cpp | ^
%COMPILER% -MT -O2 -EHsc -nologo -c -I%INCLUDE_DIR%\raylib %INCLUDE_DIR%\raylib\imgui_demo.cpp | ^
%COMPILER% -MT -O2 -EHsc -nologo -c -I%INCLUDE_DIR%\raylib %INCLUDE_DIR%\raylib\imgui_draw.cpp

	cd..
)

%COMPILER% %DEBUG_ARGS% %OPT_ARGS% -EHsc -nologo ^
	-I%INCLUDE_DIR%\raylib -I%INCLUDE_DIR%\raylib\skia %LIB_DIR%\raylib\win64\*.obj %LIB_DIR%\raylib\win64\*.lib %LIBS% winRayObj\*.obj %SRC_DIR%\main.cpp ^
	%DEFINES% -DRAYLIB_MODE /D STB_SPRINTF_OBJ /D STB_IMAGE_WRITE_OBJ /D STB_IMAGE_OBJ /D STB_TRUETYPE_OBJ /D STB_RECTPACK_OBJ /D IMGUI_OBJ ^
	-link /DEBUG -out:%GAME_NAME%.exe > %PROJECT_DIR%\errors.err

if ERRORLEVEL 1 (
	type %PROJECT_DIR%\errors.err
	exit /b 1
)

type %PROJECT_DIR%\errors.err
copy %LIB_DIR%\win64\*.dll .

@copy %LIB_DIR%\win64\*.pdb . 1>nul 2>nul

popd
