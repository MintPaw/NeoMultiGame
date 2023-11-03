BACKEND_TYPE=raylib

ifeq ($(shell echo $$HOSTNAME), MintPaw-PC)
# GAME_NAME=parametersGame
# GAME_NAME=petGame
# GAME_NAME=towerGame
# GAME_NAME=dynaGame
# GAME_NAME=rtsClient2
# GAME_NAME=testGame
# GAME_NAME=catsFirstGame
# GAME_NAME=interrogationGame
# GAME_NAME=zooBoundGame
# GAME_NAME=tower2Game
# GAME_NAME=deskGame
# GAME_NAME=concreteJungleGame
# GAME_NAME=rollerGame
# GAME_NAME=turnBasedGame
# GAME_NAME=destinyGame
# GAME_NAME=swfTestGame
# GAME_NAME=horseGame
# GAME_NAME=catCardGame
GAME_NAME=boxingGame
# GAME_NAME=gladiators2Game
# GAME_NAME=catAnimGame
endif

ifeq ($(shell echo $$HOSTNAME), mintpaw-lappy)
GAME_NAME=horseGame
endif

ifeq ($(shell echo $$HOSTNAME), MintPaw-tablet)
# GAME_NAME=concreteJungleGame
# GAME_NAME=butt2GoGame
# GAME_NAME=tower2Game
# GAME_NAME=dynaGame
# GAME_NAME=testGame
# GAME_NAME=horseGame
# GAME_NAME=catCardGame
GAME_NAME=boxingGame
# GAME_NAME=gladiators2Game
# GAME_NAME=catAnimGame
endif

-include ../multiGamePrivate/Makefile.in.start

SEVEN_ZIP="/c/Program Files/7-Zip/7z.exe"

WIN_BUILD_BAT=buildWin32Ray
EMSCRIPTEN_BUILD_BAT=buildEmscriptenRay

ifeq ($(shell echo $$HOSTNAME), MintPaw-PC)
	WIN_API_TRACE_DIR=d:/_tools/apitrace/bin
else
	WIN_API_TRACE_DIR=c:/_tools/apitrace/bin
endif

PYTHON3=/c/Users/MintPaw/AppData/Local/Programs/Python/Python37-32/python.exe
BLENDER="/c/Program Files/Blender Foundation/Blender 3.0/blender.exe"

WIN_CMD=MSYS2_ARG_CONV_EXCL="*" cmd /c

all:
	$(MAKE) b
	$(MAKE) r

ifneq (, $(findstring MSYS_NT, $(shell uname))) # -------------------------------------------------- Windows
bdebugrel:
	$(WIN_CMD) "\
		set GAME_NAME=$(GAME_NAME)&& \
		set DEBUG_MODE=1&& \
		set INTERNAL_MODE=0&& \
		set OPTIMIZED_MODE=1&& \
		set BACKEND_TYPE=$(BACKEND_TYPE)&& \
		buildSystem\$(WIN_BUILD_BAT).bat\
		"

brel:
	$(WIN_CMD) "\
		set GAME_NAME=$(GAME_NAME)&& \
		set DEBUG_MODE=0&& \
		set OPTIMIZED_MODE=1&& \
		set INTERNAL_MODE=0&& \
		set BACKEND_TYPE=$(BACKEND_TYPE)&& \
		buildSystem\$(WIN_BUILD_BAT).bat\
		"

rel:
	$(MAKE) brel
	$(MAKE) r

optiDebug:
	$(WIN_CMD) "\
		set GAME_NAME=$(GAME_NAME)&& \
		set INTERNAL_MODE=1&& \
		set DEBUG_MODE=1&& \
		set OPTIMIZED_MODE=1&& \
		set BACKEND_TYPE=$(BACKEND_TYPE)&& \
		buildSystem\$(WIN_BUILD_BAT).bat\
		"
	$(MAKE) r

b:
	echo -ne "\e]0;\a"
	$(WIN_CMD) "\
		set GAME_NAME=$(GAME_NAME)&& \
		set DEBUG_MODE=1&& \
		set INTERNAL_MODE=1&& \
		set CLANG_MODE=0&& \
		set BACKEND_TYPE=$(BACKEND_TYPE)&& \
		buildSystem\$(WIN_BUILD_BAT).bat\
		" && (echo -ne "\e]0;GOOD\a") || (echo -ne "\e]0;BAD\a"; exit 1)

r:
	@echo .
	@echo .
	@echo .
	@(cd /c/bin; ./$(GAME_NAME).exe)

glfw:
	$(MAKE) bglfw
	$(MAKE) r

bglfw:
	echo -ne "\e]0;\a"
	$(WIN_CMD) "\
		set GAME_NAME=$(GAME_NAME)&& \
		set DEBUG_MODE=1&& \
		set INTERNAL_MODE=1&& \
		set GLFW_MODE=1&& \
		buildSystem\$(WIN_BUILD_BAT).bat\
		" && (echo -ne "\e]0;GOOD\a") || (echo -ne "\e]0;BAD\a"; exit 1)

clang:
	$(MAKE) bclang
	$(MAKE) rclang

bclang:
	$(WIN_CMD) "\
		set GAME_NAME=$(GAME_NAME)&& \
		set DEBUG_MODE=1&& \
		set INTERNAL_MODE=1&& \
		set CLANG_MODE=1&& \
		set BACKEND_TYPE=$(BACKEND_TYPE)&& \
		buildSystem\$(WIN_BUILD_BAT).bat\
		"

rclang:
	# cd c:\bin; \
	# 	set ASAN_OPTIONS=windows_hook_rtl_allocators=true:suppressions=asanSupp:continue_on_error=1:help=1; \
	# 	./$(GAME_NAME).exe;
	$(WIN_CMD) "\
			set ASAN_OPTIONS=windows_hook_rtl_allocators=true:suppressions=asanSupp:continue_on_error=1 & \
			cd c:\bin & \
			$(GAME_NAME).exe >info 2> log & \
			type info & \
			type log & \
			"

wasm:
	$(MAKE) bwasm

bwasm:
	mkdir -p /c/bin/wasm
	clang++ --target=wasm32-unknown-wasi --sysroot buildSystem/wasiSysroot -O3 -o C:/bin/wasm/main.o -c \
	-I include/raylib \
	-I include/raylib/skia \
	src/main.cpp
	wasm-ld --no-entry --export-all --lto-O3 --allow-undefined --import-memory C:/bin/wasm/main.o -o C:/bin/wasm/main.wasm

w:
	$(MAKE) bw
	$(MAKE) rw

wf:
	$(MAKE) bwf
	$(MAKE) rw

bwf:
	echo $(GAME_NAME)
	$(WIN_CMD) "\
		set GAME_NAME=$(GAME_NAME)&& \
		set DEBUG_MODE=1&& \
		set INTERNAL_MODE=1&& \
		set CODE_ONLY=1&& \
		buildSystem\$(EMSCRIPTEN_BUILD_BAT).bat\
		"

bw:
	-$(WIN_CMD) "\
		set GAME_NAME=$(GAME_NAME)&& \
		set DEBUG_MODE=1&& \
		set INTERNAL_MODE=1&& \
		buildSystem\$(EMSCRIPTEN_BUILD_BAT).bat\
		"

bwdebugrel:
	-$(WIN_CMD) "\
		set GAME_NAME=$(GAME_NAME)&& \
		set DEBUG_MODE=1&& \
		set INTERNAL_MODE=0&& \
		set OPTIMIZED_MODE=1&& \
		buildSystem\$(EMSCRIPTEN_BUILD_BAT).bat\
		"

bwrel:
	echo $(GAME_NAME)
	$(WIN_CMD) "\
		set GAME_NAME=$(GAME_NAME)&& \
		set DEBUG_MODE=0&& \
		set INTERNAL_MODE=0&& \
		set OPTIMIZED_MODE=1&& \
		buildSystem\$(EMSCRIPTEN_BUILD_BAT).bat\
		"

rw:

	$(WIN_CMD) "C:\Users\MintPaw\AppData\Local\Chromium\Application\chrome.exe 127.0.0.1/game" &

debugVs:
	# $(MAKE) debugC
	$(WIN_CMD) "devenv C:/bin/$(GAME_NAME).exe"

debugC:
	-$(WIN_CMD) "D:\_tools\remedyBg\remedyBg.exe C:/bin/$(GAME_NAME).exe" &
	-$(WIN_CMD) "C:\_tools\remedyBg\remedyBg.exe C:/bin/$(GAME_NAME).exe" &
endif # ------------------------------------------------------------------------------------------ End Windows

ifneq (, $(findstring Linux, $(shell uname))) # -------------------------------------------------- Linux

b:
	mkdir -p bin
	clang -g \
		-I include/raylib \
		-I include/raylib/skia \
		-I include/linux_raylib_specific \
		-I /usr/include/GL \
		src/main.cpp \
		-DPLAYING_$(GAME_NAME) \
		-DRAYLIB_MODE=1 \
		-DPLATFORM=PLATFORM_DESKTOP \
		-DGRAPHICS=GRAPHICS_API_OPENGLES_20 \
		-x c++ \
		include/win64/stb_image.h \
		include/win64/stb_sprintf.h \
		include/win64/stb_image_write.h \
		include/win64/imstb_truetype.h \
		include/win64/imstb_rectpack.h \
		-o bin/$(GAME_NAME) \
		-fPIC \
		-L/usr/local/lib/raysan5 \
		-lm -lopenal -lGLESv2

r:
	(cd bin; ./$(GAME_NAME))
endif # ------------------------------------------------------------------------------------------ End Linux

debugGl:
	cd /c/bin; \
	rm -f gl.trace; \
	$(WIN_CMD) "$(WIN_API_TRACE_DIR)/apitrace trace -o gl.trace -v --api gl $(GAME_NAME).exe"; \
	$(WIN_CMD) "$(WIN_API_TRACE_DIR)/qapitrace gl.trace"

debugGlChrome:
	# $(WIN_CMD) "D:/_tools/chromium/chrome.exe --no-sandbox --disable-gpu-watchdog --gpu-startup-dialog"
	$(WIN_CMD) "set RENDERDOC_HOOK_EGL=0 && D:/_tools/chromium/chrome.exe --disable-gpu-sandbox --gpu-startup-dialog"

mtune:
	$(WIN_CMD) "D:\_tools\mtuner\mtuner.exe C:\bin\$(GAME_NAME).exe" &
	$(WIN_CMD) "C:\_tools\mtuner\mtuner.exe C:\bin\$(GAME_NAME).exe" &

clean:
	rm -rf bin/*
	rm -rf /c/bin/*

optimizePngs:
	find $(GAME_NAME)Assets/assets -iname *.png -print0 | xargs -0 -P 8 -n 4 optipng -o3

optimizePngsHard:
	find $(GAME_NAME)Assets/assets -iname *.png -print0 | xargs -0 -P 8 -n 4 optipng -o5

optimizePngsInsane:
	find $(GAME_NAME)Assets/assets -iname *.png -print0 | xargs -0 -P 8 -n 4 optipng -o7

countEngine:
	cloc --by-file --not-match-d ".*Game.*" --not-match-f ".*GameData.*|.*Game.cpp|.*rts.*" src

countHorseGame:
	cloc --by-file ../multiGamePrivate/src/horseGame.cpp ../multiGamePrivate/src/horseGame/

shipZooBoundToDropbox:
	-$(MAKE) clean
	$(WIN_CMD) "\
		set GAME_NAME=zooBoundGame&& \
		set DEBUG_MODE=1&& \
		set INTERNAL_MODE=1&& \
		set OPTIMIZED_MODE=1&& \
		set BACKEND_TYPE=$(BACKEND_TYPE)&& \
		buildSystem\$(WIN_BUILD_BAT).bat\
		"
	cd /c/bin; \
		rm -rf pack; \
		mkdir pack; \
		cd pack; \
		cp /c/bin/*.exe .; \
		cp /c/bin/*.dll .; \
		cp -r /c/bin/pack/* "/c/Dropbox/ZooBound/game"

shipZooBoundToSelf:
	-$(MAKE) clean
	$(WIN_CMD) "\
		set GAME_NAME=zooBoundGame&& \
		set DEBUG_MODE=1&& \
		set INTERNAL_MODE=0&& \
		set OPTIMIZED_MODE=1&& \
		set BACKEND_TYPE=$(BACKEND_TYPE)&& \
		buildSystem\$(WIN_BUILD_BAT).bat\
		"
	cd /c/bin; \
		rm -rf pack; \
		mkdir pack; \
		cd pack; \
		cp /c/bin/*.exe .; \
		cp /c/bin/*.dll .; \
		rsync -at \
			--exclude '__*' --exclude '*.psd' --exclude '*.blend' --exclude '*.blend1' \
			/c/Dropbox/ZooBound/zooBoundGameAssets/assets .; \
		rsync -a /c/bin/pack/* /c/bin/selfShip
		cp /c/bin/*.pdb /c/bin/selfShip

shipHorseGameToSelf:
	$(WIN_CMD) "\
		set GAME_NAME=horseGame&& \
		set DEBUG_MODE=0&& \
		set INTERNAL_MODE=0&& \
		set OPTIMIZED_MODE=1&& \
		set BACKEND_TYPE=$(BACKEND_TYPE)&& \
		buildSystem\$(WIN_BUILD_BAT).bat\
		"
	cd /c/bin; \
		rm -rf pack; \
		mkdir pack; \
		cd pack; \
		cp /c/bin/*.exe .; \
		cp /c/bin/*.dll .; \
		rsync -at \
			--exclude '__*' --exclude '*.psd' --exclude '*_baked.png' --exclude 'Lightmaps' --exclude 'LilySurface' \
			--exclude '*.blend' --exclude '*.blend1' \
			/c/Dropbox/FallowCandy/HorseProjects/horseGameAssets/assets .; \
		rsync -a /c/bin/pack/* /c/bin/selfShip
		# cp /c/bin/*.pdb /c/bin/selfShip
		
		cd /c/bin; \
			$(SEVEN_ZIP) a -tzip selfShip.zip selfShip

encodeHorseGameAudio:
	rm -rf /c/Dropbox/FallowCandy/HorseProjects/horseGameAssets/assets/audio
	cp -r /c/Dropbox/FallowCandy/HorseProjects/raw/audio /c/Dropbox/FallowCandy/HorseProjects/horseGameAssets/assets/audio
	N=32; \
		( \
		OIFS="$$IFS"; \
		IFS=$$'\n'; \
		for file in `find "/c/Dropbox/FallowCandy/HorseProjects/horseGameAssets/assets/audio" -type f -name "*.wav"`; do \
		((i=i%N)); ((i++==0)) && wait ; \
		echo encoding "$$file"; \
		ffmpeg -hide_banner -loglevel error -i "$$file" -ar 44100 "$${file%.*}.ogg" & \
		done; \
		)
	sleep 2
	find /c/Dropbox/FallowCandy/HorseProjects/horseGameAssets/assets/audio -type f -name "*.wav" -delete

-include ../multiGamePrivate/Makefile.in
