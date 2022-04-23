call C:\emsdk\emsdk_env.bat

cd /d D:\_tools\_sdks\raylib-4.0.0\src
call emcc -c rcore.c -Os -Wall -DPLATFORM_WEB -DGRAPHICS_API_OPENGL_ES2
call emcc -c rshapes.c -Os -Wall -DPLATFORM_WEB -DGRAPHICS_API_OPENGL_ES2
call emcc -c rtextures.c -Os -Wall -DPLATFORM_WEB -DGRAPHICS_API_OPENGL_ES2
call emcc -c rtext.c -Os -Wall -DPLATFORM_WEB -DGRAPHICS_API_OPENGL_ES2
call emcc -c rmodels.c -Os -Wall -DPLATFORM_WEB -DGRAPHICS_API_OPENGL_ES2
call emcc -c utils.c -Os -Wall -DPLATFORM_WEB
call emcc -c raudio.c -Os -Wall -DPLATFORM_WEB

call emar rcs libraylib.a rcore.o rshapes.o rtextures.o rtext.o rmodels.o utils.o raudio.o

copy libraylib.a C:\Dropbox\MultiGame\multiGame\lib\raylib\emscripten

pause
