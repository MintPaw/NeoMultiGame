pushd .

if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat" (
	set VS_CMD_PATH="C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat"
) else (
	if exist "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\Common7\Tools\VsDevCmd.bat" (
		set VS_CMD_PATH="C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\Common7\Tools\VsDevCmd.bat"
	) else (
		set VS_CMD_PATH="C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\Common7\Tools\VsDevCmd.bat"
	)
)

call %VS_CMD_PATH% -arch=amd64 -host_arch=amd64 >NUL:

call C:\emsdk\emsdk_env.bat
call emsdk activate 3.1.44

popd

C:\msys64\msys2_shell.cmd -where . -full-path
