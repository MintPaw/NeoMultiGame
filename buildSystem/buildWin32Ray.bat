SetLocal EnableDelayedExpansion

if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat" (
	set VS_CMD_PATH="C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat"
)

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

where cl
if %errorlevel% neq 0 (
	call %VS_CMD_PATH% -arch=amd64 -host_arch=amd64 -vcvars_ver=14.2
)

set BIN_DIR=C:\bin
if not exist %BIN_DIR% mkdir %BIN_DIR%

set PROJECT_DIR="%~dp0.."
set INCLUDE_DIR=%PROJECT_DIR%\include\raylib
set SRC_DIR=%PROJECT_DIR%\src

if "%BACKEND_TYPE%"=="" set BACKEND_TYPE=raylib

if "%BACKEND_TYPE%"=="raylib" set LIB_DIR=%PROJECT_DIR%\lib\raylib
if "%BACKEND_TYPE%"=="glfw" set LIB_DIR=%PROJECT_DIR%\lib\glfw

cls
pushd .
cd %PROJECT_DIR%

call buildSystem/prepareAssets.bat

if "%GAME_NAME%"=="deskGame" set USES_CURL=1

set CURL_ARGS=
if [%USES_CURL%]==[1] set CURL_ARGS=-I%INCLUDE_DIR%\curl %LIB_DIR%\..\curl\*.a

set DEFINES=-D PLAYING_%GAME_NAME%
if [%DEBUG_MODE%]==[1] (
	set DEBUG_ARGS=-MT -Zi
	set DEFINES=!DEFINES! -D FALLOW_DEBUG
) else (
	set DEBUG_ARGS=-MT

	robocopy %ASSETS_DIR%/assets %BIN_DIR%/assets /MIR /W:1 /njh /njs /ndl /nc /ns /XO /XF __* *.psd
	for /d /r %BIN_DIR%\assets\ %%f in (__*) do rmdir /s /q %%f
)

if [%CLANG_MODE%]==[1] (
	set OPT_ARGS=-Od -D _DISABLE_VECTOR_ANNOTATION -D _DISABLE_STRING_ANNOTATION
) else (
	set OPT_ARGS=-Od -RTCsu
)

if [%OPTIMIZED_MODE%]==[1] (
	set OPT_ARGS=-O2
)

set LIBS=winmm.lib opengl32.lib user32.lib gdi32.lib shell32.lib
if [%INTERNAL_MODE%]==[1] (
	set DEFINES=!DEFINES! /D FALLOW_INTERNAL
) else (
	set DEFINES=!DEFINES!
	set LIBS=!LIBS! dbghelp.lib
)

if "%BACKEND_TYPE%"=="raylib" set DEFINES=!DEFINES! -D RAYLIB_MODE
if "%BACKEND_TYPE%"=="glfw" set DEFINES=!DEFINES! -D GLFW_MODE

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

	%COMPILER% -MT -O2 -EHsc -nologo -c -I%INCLUDE_DIR% -D STB_SPRINTF_NOUNALIGNED -D STB_SPRINTF_IMPLEMENTATION /Tp %INCLUDE_DIR%\stb_sprintf.h | ^
%COMPILER% -MT -O2 -EHsc -nologo -c -I%INCLUDE_DIR% -D STB_TRUETYPE_IMPLEMENTATION /Tp %INCLUDE_DIR%\imstb_truetype.h | ^
%COMPILER% -MT -O2 -EHsc -nologo -c -I%INCLUDE_DIR% %INCLUDE_DIR%\imgui.cpp | ^
%COMPILER% -MT -O2 -EHsc -nologo -c -I%INCLUDE_DIR% %INCLUDE_DIR%\imgui_tables.cpp | ^
%COMPILER% -MT -O2 -EHsc -nologo -c -I%INCLUDE_DIR% %INCLUDE_DIR%\imgui_widgets.cpp | ^
%COMPILER% -MT -O2 -EHsc -nologo -c -I%INCLUDE_DIR% %INCLUDE_DIR%\imgui_demo.cpp | ^
%COMPILER% -MT -O2 -EHsc -nologo -c -I%INCLUDE_DIR% %INCLUDE_DIR%\imgui_draw.cpp

	cd ..
)

%COMPILER% /std:c++17 %DEBUG_ARGS% %OPT_ARGS% -EHsc -nologo ^
	-I%INCLUDE_DIR% -I%INCLUDE_DIR%\skia %LIB_DIR%\win64\*.obj %LIB_DIR%\win64\*.lib %LIBS% winRayObj\*.obj %SRC_DIR%\main.cpp ^
	%DEFINES% %ALT_CORE_PATH_DEFINES% %CURL_ARGS% /D STB_SPRINTF_OBJ /D STB_IMAGE_WRITE_OBJ /D STB_IMAGE_OBJ /D STB_TRUETYPE_OBJ /D STB_RECTPACK_OBJ /D IMGUI_OBJ ^
	-link /DEBUG -out:%GAME_NAME%.exe > %PROJECT_DIR%\errors.err

if ERRORLEVEL 1 (
	type %PROJECT_DIR%\errors.err
	exit /b 1
)

type %PROJECT_DIR%\errors.err

@copy %LIB_DIR%\win64\*.dll . 1>nul 2>nul
@copy %LIB_DIR%\win64\*.pdb . 1>nul 2>nul
@copy %PROJECT_DIR%\buildSystem\asanSupp .

if [%USES_CURL%]==[1] (
	@copy %LIB_DIR%\..\curl\*.dll . 1>nul 2>nul
)

popd
